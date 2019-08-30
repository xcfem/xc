//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.14 $
// $Date: 2005/01/25 21:55:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Steel01.cpp,v $

// Written: MHS
// Created: 06/99
// Revision: A
//
// Description: This file contains the class implementation for
// Steel01.
//
// What: "@(#) Steel01.C, revA"


#include <material/uniaxial/steel/Steel01.h>
#include "domain/component/Parameter.h"
#include <utility/matrix/Vector.h>

#include <domain/mesh/element/utils/Information.h>
#include <cmath>
#include <cfloat>
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/MovableMatrix.h"
#include "utility/actor/actor/MatrixCommMetaData.h"

//! @brief Sets all history and state variables to initial values
int XC::Steel01::setup_parameters(void)
  {
    // History variables
    CminStrain= 0.0;
    CmaxStrain= 0.0;
    CshiftP= 1.0;
    CshiftN= 1.0;
    Cloading= 0;

    TminStrain= 0.0;
    TmaxStrain= 0.0;
    TshiftP= 1.0;
    TshiftN= 1.0;
    Tloading= 0;

    // State variables
    Cstrain= 0.0;
    Cstress= 0.0;
    Ctangent= E0;

    Tstrain= 0.0;
    Tstress= 0.0;
    Ttangent= E0;
    return 0;
  }

//! @brief Constructor.
XC::Steel01::Steel01(int tag, double FY, double E, double B,double A1, double A2, double A3, double A4)
  : SteelBase0103(tag,MAT_TAG_Steel01,FY,E,B,A1,A2,A3,A4), parameterID(0), SHVs()
  {
    // Sets all history and state variables to initial values
    setup_parameters();
  }

//! @brief Constructor.
XC::Steel01::Steel01(int tag)
  :SteelBase0103(tag,MAT_TAG_Steel01), parameterID(0), SHVs() {}

//! @brief Constructor.
XC::Steel01::Steel01(void)
  :SteelBase0103(MAT_TAG_Steel01), parameterID(0), SHVs() {}

XC::Steel01::~Steel01(void)
  {}

//! @brief Calculates the trial state variables based on the trial strain
void XC::Steel01::determineTrialState(double dStrain)
  {
    const double fyOneMinusB= fy * (1.0 - b);
    const double Esh= getEsh();
    const double epsy= getEpsy();

    const double c1= Esh*Tstrain;
    const double c2= TshiftN*fyOneMinusB;
    const double c3= TshiftP*fyOneMinusB;
    const double c= Cstress + E0*dStrain;

//     /**********************************************************
//        removal of the following lines due to problems with
//        optimization may be required (e.g. on gnucc compiler
//        with optimization turned on & -ffloat-store option not
//        used) .. replace them with line that follows but which
//        now requires 2 function calls to achieve same result !!
//     ************************************************************/

//     const double c1c3= c1 + c3;

//     if(c1c3<c)
//       Tstress = c1c3;
//     else
//       Tstress = c;

//     const double c1c2= c1-c2;

//     if(c1c2 > Tstress)
//       Tstress = c1c2;

//     /* ***********************************************************
//     and replace them with:
//     Tstress = fmax((c1-c2), fmin((c1+c3),c));
//     **************************************************************/
    Tstress= std::max((c1-c2), std::min((c1+c3),c));

    if(fabs(Tstress-c)<DBL_EPSILON)
      Ttangent = E0;
    else
      Ttangent = Esh;

    //
    // Determine if a load reversal has occurred due to the trial strain
    //

    // Determine initial loading condition:  1 = loading (positive strain increment)
                                         // -1 = unloading (negative strain increment)
                                         // 0 initially
    if(Tloading == 0 && dStrain != 0.0)
      {
        if(dStrain > 0.0)
          Tloading = 1;
        else
          Tloading = -1;
      }

    // Transition from loading to unloading, i.e. positive strain increment
    // to negative strain increment
    if(Tloading == 1 && dStrain < 0.0)
      {
        Tloading = -1;
        if(Cstrain > TmaxStrain)
          TmaxStrain = Cstrain;
        TshiftN= 1 + a1*pow((TmaxStrain-TminStrain)/(2.0*a2*epsy),0.8);
      }

    // Transition from unloading to loading, i.e. negative strain increment
    // to positive strain increment
    if(Tloading == -1 && dStrain > 0.0)
      {
        Tloading = 1;
        if(Cstrain < TminStrain)
          TminStrain = Cstrain;
        TshiftP = 1 + a3*pow((TmaxStrain-TminStrain)/(2.0*a4*epsy),0.8);
      }
  }

//! @brief Determines if a load reversal has occurred based on the trial strain
void XC::Steel01::detectLoadReversal(double dStrain)
  {
    // Determine initial loading condition
    if(Tloading == 0 && dStrain != 0.0)
      {
        if(dStrain > 0.0)
          Tloading = 1;
        else
          Tloading = -1;
      }

   const double epsy= getEpsy();

   // Transition from loading to unloading, i.e. positive strain increment
   // to negative strain increment
   if(Tloading == 1 && dStrain < 0.0)
     {
       Tloading = -1;
       if(Cstrain > TmaxStrain)
         TmaxStrain = Cstrain;
       TshiftN= 1 + a1*pow((TmaxStrain-TminStrain)/(2.0*a2*epsy),0.8);
     }

   // Transition from unloading to loading, i.e. negative strain increment
   // to positive strain increment
   if(Tloading == -1 && dStrain > 0.0)
     {
       Tloading = 1;
       if(Cstrain < TminStrain)
         TminStrain = Cstrain;
       TshiftP = 1 + a3*pow((TmaxStrain-TminStrain)/(2.0*a4*epsy),0.8);
     }
  }

int XC::Steel01::revertToStart(void)
  {
    SteelBase0103::revertToStart();
// AddingSensitivity:BEGIN /////////////////////////////////
    if(!SHVs.isEmpty()) SHVs.Zero();
// AddingSensitivity:END //////////////////////////////////
    return 0;
  }

//! @brief Virtual constructor.
XC::UniaxialMaterial* XC::Steel01::getCopy(void) const
  { return new Steel01(*this); }

//! @brief Send object members through the channel being passed as parameter.
int XC::Steel01::sendData(CommParameters &cp)
  {
    int res= SteelBase0103::sendData(cp);
    const double pid= parameterID;
    res+= cp.sendDouble(pid,getDbTagData(),CommMetaData(8));
    res+= cp.sendMatrix(SHVs,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Steel01::recvData(const CommParameters &cp)
  {
    int res= SteelBase0103::recvData(cp);
    double pid;
    res+= cp.receiveDouble(pid,getDbTagData(),CommMetaData(7));
    parameterID= pid;
    res+= cp.receiveMatrix(SHVs,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Steel01::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(13);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Steel01::recvSelf(const CommParameters &cp)
  {
    inicComm(13);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Print stuff.
void XC::Steel01::Print(std::ostream& s, int flag)
  {
    s << "Steel01 tag: " << this->getTag() << std::endl;
    s << "  fy: " << fy << " ";
    s << "  E0: " << E0 << " ";
    s << "  b:  " << b << " ";
    s << "  a1: " << a1 << " ";
    s << "  a2: " << a2 << " ";
    s << "  a3: " << a3 << " ";
    s << "  a4: " << a4 << " ";
  }

// AddingSensitivity:BEGIN ///////////////////////////////////
int XC::Steel01::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const int sp= SteelBase::setParameter(argv,param);
    if(sp<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; WARNING: could not set parameter. "
		<< std::endl;
    return sp;
  }


int XC::Steel01::updateParameter(int parameterID, Information &info)
  {
    const int up= SteelBase::updateParameter(parameterID,info);
    Ttangent = E0;          // Initial stiffness
    return up;
  }




int XC::Steel01::activateParameter(int passedParameterID)
  {
    parameterID = passedParameterID;
    return 0;
  }



double XC::Steel01::getStressSensitivity(int gradNumber, bool conditional)
  {
    // Initialize return value
    double gradient = 0.0;

    // Pick up sensitivity history variables
    double CstrainSensitivity = 0.0;
    double CstressSensitivity = 0.0;
    if(!SHVs.isEmpty())
      {
        CstrainSensitivity= (SHVs)(0,(gradNumber-1));
        CstressSensitivity= (SHVs)(1,(gradNumber-1));
      }

    // Assign values to parameter derivatives (depending on what's random)
    double fySensitivity = 0.0;
    double E0Sensitivity = 0.0;
    double bSensitivity = 0.0;
    if(parameterID == 1)
      { fySensitivity = 1.0; }
    else if(parameterID == 2)
      { E0Sensitivity = 1.0; }
    else if(parameterID == 3)
      { bSensitivity = 1.0; }

    // Compute min and max stress
    double Tstress;
    const double dStrain = Tstrain-Cstrain;
    const double sigmaElastic = Cstress + E0*dStrain;
    const double fyOneMinusB = fy * (1.0 - b);
    const double Esh = b*E0;
    const double c1 = Esh*Tstrain;
    const double c2 = TshiftN*fyOneMinusB;
    const double c3 = TshiftP*fyOneMinusB;
    const double sigmaMax = c1+c3;
    const double sigmaMin = c1-c2;

    // Evaluate stress sensitivity
    if( (sigmaMax < sigmaElastic) && (fabs(sigmaMax-sigmaElastic)>1e-5) )
      {
        Tstress = sigmaMax;
        gradient = E0Sensitivity*b*Tstrain
                   + E0*bSensitivity*Tstrain
                   + TshiftP*(fySensitivity*(1-b)-fy*bSensitivity);
      }
    else
      {
        Tstress = sigmaElastic;
        gradient = CstressSensitivity
                   + E0Sensitivity*(Tstrain-Cstrain)
                   - E0*CstrainSensitivity;
      }
    if(sigmaMin > Tstress)
      {
        gradient = E0Sensitivity*b*Tstrain
                   + E0*bSensitivity*Tstrain
                   - TshiftN*(fySensitivity*(1-b)-fy*bSensitivity);
      }
    return gradient;
  }


double XC::Steel01::getInitialTangentSensitivity(int gradNumber)
  {
    // For now, assume that this is only called for initial stiffness
    if(parameterID == 2)
      { return 1.0; }
    else
      { return 0.0; }
  }


int XC::Steel01::commitSensitivity(double TstrainSensitivity, int gradNumber, int numGrads)
  {
    if(SHVs.isEmpty())
      SHVs= Matrix(2,numGrads);

    // Initialize unconditaional stress sensitivity
    double gradient= 0.0;

    // Pick up sensitivity history variables
    double CstrainSensitivity = 0.0;
    double CstressSensitivity         = 0.0;
    if(!SHVs.isEmpty())
      {
        CstrainSensitivity= SHVs(0,(gradNumber-1));
        CstressSensitivity= SHVs(1,(gradNumber-1));
      }

    // Assign values to parameter derivatives (depending on what's random)
    double fySensitivity = 0.0;
    double E0Sensitivity = 0.0;
    double bSensitivity = 0.0;
    if(parameterID == 1)
      { fySensitivity = 1.0; }
    else if(parameterID == 2)
      { E0Sensitivity = 1.0; }
    else if(parameterID == 3)
      { bSensitivity = 1.0; }

    // Compute min and max stress
    double Tstress;
    const double dStrain = Tstrain-Cstrain;
    const double sigmaElastic = Cstress + E0*dStrain;
    const double fyOneMinusB = fy * (1.0 - b);
    const double Esh = b*E0;
    const double c1 = Esh*Tstrain;
    const double c2 = TshiftN*fyOneMinusB;
    const double c3 = TshiftP*fyOneMinusB;
    const double sigmaMax = c1+c3;
    const double sigmaMin = c1-c2;

    // Evaluate stress sensitivity ('gradient')
    if( (sigmaMax < sigmaElastic) && (fabs(sigmaMax-sigmaElastic)>1e-5) )
      {
        Tstress = sigmaMax;
        gradient = E0Sensitivity*b*Tstrain
                   + E0*bSensitivity*Tstrain
                   + E0*b*TstrainSensitivity
                   + TshiftP*(fySensitivity*(1-b)-fy*bSensitivity);
      }
    else
      {
        Tstress = sigmaElastic;
        gradient = CstressSensitivity
                   + E0Sensitivity*(Tstrain-Cstrain)
                   + E0*(TstrainSensitivity-CstrainSensitivity);
      }
    if(sigmaMin > Tstress)
      {
        gradient = E0Sensitivity*b*Tstrain
                   + E0*bSensitivity*Tstrain
                   + E0*b*TstrainSensitivity
                   - TshiftN*(fySensitivity*(1-b)-fy*bSensitivity);
      }

    // Commit history variables
    SHVs(0,(gradNumber-1)) = TstrainSensitivity;
    SHVs(1,(gradNumber-1)) = gradient;
    return 0;
  }

// AddingSensitivity:END /////////////////////////////////////////////

