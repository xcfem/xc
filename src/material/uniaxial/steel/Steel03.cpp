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
                                                                        
// $Revision: 1.2 $
// $Date: 2005/11/09 00:30:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Steel03.cpp,v $
                                                                        
// Written: mackie
// Created: 06/2005
// Revision: A
//
// Description: This file contains the class implementation for 
// Steel03. Steel03 is XC::Steel01 verbatim but with added Giuffre-Menegotto-Pinto 
// transitions on the loading and unloading loops.  
// references:
// 1.) 	Menegotto, M., and Pinto, P.E. (1973). Method of analysis of cyclically loaded 
//	RC plane frames including changes in geometry and non-elastic behavior of 
//	elements under normal force and bending. Preliminary Report IABSE, vol 13. 
// 2.)	Dhakal, R.J., and Maekawa, K. (2002). Path-dependent cyclic stress-strain relationship
//	of reinforcing bar including buckling. Engineering Structures, 24(11): 1383-96.
// 3.)	Gomes, A., and Appleton, J. (1997). Nonlinear cyclic stress-strain relationship of 
//	reinforcing bars including buckling. Engineering Structures, 19(10): 822-6.
//
// What: "@(#) Steel03.C, revA"


#include <material/uniaxial/steel/Steel03.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include <domain/mesh/element/utils/Information.h>
#include <cmath>
#include <cfloat>

//! @brief Sets all history and state variables to initial values
int XC::Steel03::setup_parameters(void)
  {
    SteelBase0103::setup_parameters();

    CbStrain = 0.0;
    CbStress = 0.0;
    CrStrain = 0.0;
    CrStress = 0.0;
    Cplastic = 0.0;

    TbStrain = 0.0;
    TbStress = 0.0;
    TrStrain = 0.0;
    TrStress = 0.0;
    Tplastic = 0.0;


    CcurR = getR(0);
    TcurR = getR(0);
    return 0;
  }

XC::Steel03::Steel03(int tag, double FY, double E, double B, double R,double R1, double R2, 
 double A1, double A2, double A3, double A4)
  : SteelBase0103(tag,MAT_TAG_Steel03,FY,E0,B,A1,A2,A3,A4), r(R), cR1(R1), cR2(R2)
  {
    setup_parameters();
  }

XC::Steel03::Steel03(int tag)
  : SteelBase0103(tag,MAT_TAG_Steel03),r(0.0), cR1(0.0), cR2(0.0) {}

XC::Steel03::Steel03(void)
  : SteelBase0103(MAT_TAG_Steel03),r(0.0), cR1(0.0), cR2(0.0) {}

int XC::Steel03::setTrialStrain(double strain, double strainRate)
  {
    // Reset history variables to last converged state
    TbStrain = CbStrain;
    TbStress = CbStress;
    TrStrain = CrStrain;
    TrStress = CrStress;
    Tplastic = Cplastic;
    TcurR = CcurR;

    return SteelBase0103::setTrialStrain(strain,strainRate);
  }

int XC::Steel03::setTrial(double strain, double &stress, double &tangent, double strainRate)
  { return setTrialStrain(strain,strainRate); }

double XC::Steel03::getR(double x_in)
  {
    // maybe modify this later, but it gives us better degradation at smaller strains
    x_in = fabs(x_in);
    double temp_r = r;
    
    // new input parameters are supposed to match Steel02 which look like 
    // Dhakal and Maekawa values: cr1 = 0.925, cr2 = 0.15
    // where 0.925 comes from 18.5/20.0 where R0=20 and 18.5 is the old coefficient provided
    //
    // so for old Dhakal and Maekawa R0 = 20, cr1 = 18.5/R0 = 0.925, cr2 = 0.15
    // so for old Gomes and Appleton R0 = 20, cr1 = 19.0/R0 = 0.95, cR2 = 0.3
    // so for my old model, now just use R0 = 20, cR1 = 0, cR2 = 0
    if(cR1 < 0.1 && cR2 < 0.1)
      {
        // Mackie, rough trilinear fit to the tangent to the x_in-r first 
        // quadrant circle.  Try using with values of R0 like 20 to 30
        temp_r = r*2.0/20.0;
        double t1 = -x_in/7+15/7*temp_r;
        double t2 = -4*x_in+6*temp_r;
        if (t1 > temp_r)
            temp_r = t1;
        if (t2 > temp_r)
            temp_r = t2;
        //std::cerr << "xin = " << x_in << " rout = " << temp_r << std::endl;
      }
    else
      {
    	temp_r = r * (1.0 - cR1*x_in/(cR2+x_in));
        if(temp_r < 0)
          temp_r = 1.0e-8;
      }
    return temp_r;
  }

//! @brief Calculates the trial state variables based on the trial strain
void XC::Steel03::determineTrialState (double dStrain)
  {
      double fyOneMinusB = fy * (1.0 - b);

      double Esh = b*E0;
      double epsy = fy/E0;
      
      double c1 = Esh*Tstrain;
      double c2 = TshiftN*fyOneMinusB;
      double c3 = TshiftP*fyOneMinusB;
      double c = Cstress + E0*dStrain;
      
      //
      // Determine if a load reversal has occurred due to the trial strain
      //

      // Determine initial loading condition
      if (Tloading == 0 && dStrain != 0.0) {
          TmaxStrain = epsy;
          TminStrain = -epsy;
	  if (dStrain > 0.0) {
	    Tloading = 1;
            TbStrain = TmaxStrain;
            TbStress = fy;
            Tplastic = TmaxStrain;
          }
	  else {
	    Tloading = -1;
            TbStrain = TminStrain;
            TbStress = -fy;
            Tplastic = TminStrain;
          }

          double intval = 1+pow(fabs(Tstrain/epsy),TcurR);
          Tstress = c1+(1-b)*E0*Tstrain/pow(intval,1/TcurR);
          Ttangent = Esh+E0*(1-b)/pow(intval,1+1/TcurR);
      }
          
      // Transition from loading to unloading, i.e. positive strain increment
      // to negative strain increment
      if (Tloading == 1 && dStrain < 0.0) {
	  Tloading = -1;
	  if (Cstrain > TmaxStrain)
	    TmaxStrain = Cstrain;
          Tplastic = TminStrain;
	  TshiftN = 1 + a1*pow((TmaxStrain-TminStrain)/(2.0*a2*epsy),0.8);
          TrStrain = Cstrain;
          TrStress = Cstress;
          TbStrain = (c2+c)/E0/(b-1)+Tstrain/(1-b);
          TbStress = 1/(b-1)*(b*c2+b*c-c1)-c2;
          TcurR = getR((TbStrain-TminStrain)/epsy);
      }

      // Transition from unloading to loading, i.e. negative strain increment
      // to positive strain increment
      if (Tloading == -1 && dStrain > 0.0) {
	  Tloading = 1;
	  if (Cstrain < TminStrain)
	    TminStrain = Cstrain;
          Tplastic = TmaxStrain;
	  TshiftP = 1 + a3*pow((TmaxStrain-TminStrain)/(2.0*a4*epsy),0.8);
          TrStrain = Cstrain;
          TrStress = Cstress;
          TbStrain = (c3-c)/E0/(1-b)+Tstrain/(1-b);
          TbStress = 1/(1-b)*(b*c3-b*c+c1)+c3;
          TcurR = getR((TmaxStrain-TbStrain)/epsy);
      }
      
      if (Cloading != 0) {
          double c4 = TbStrain - TrStrain;
          double c5 = TbStress - TrStress;
          double c6 = Tstrain - TrStrain;
          double c4c5 = c5/c4;
          double intval = 1+pow(fabs(c6/c4),TcurR);
          
          Tstress = TrStress+b*c4c5*c6+(1-b)*c4c5*c6/pow(intval,1/TcurR);
          Ttangent = c4c5*b+c4c5*(1-b)/pow(intval,1+1/TcurR);
      }
}


int XC::Steel03::commitState(void)
  {  
    // History variables
    CbStrain = TbStrain;
    CbStress = TbStress;
    CrStrain = TrStrain;
    CrStress = TrStress;
    Cplastic = Tplastic;
    CcurR = TcurR;
    return SteelBase0103::commitState();
  }

//! @brief Reset material to last committed state
int XC::Steel03::revertToLastCommit(void)
  {
    // Reset trial history variables to last committed state
    TbStrain = CbStrain;
    TbStress = CbStress;
    TrStrain = CrStrain;
    TrStress = CrStress;
    Tplastic = Cplastic;

    // Reset trial state variables to last committed state
    TcurR = CcurR;
    return SteelBase0103::revertToLastCommit();
  }

XC::UniaxialMaterial* XC::Steel03::getCopy(void) const
  { return new Steel03(*this); }

//! @brief Send object members through the channel being passed as parameter.
int XC::Steel03::sendData(CommParameters &cp)
  {
    int res= SteelBase0103::sendData(cp);
    res+= cp.sendDoubles(r,cR1,cR2,CbStrain,CbStress,getDbTagData(),CommMetaData(8));
    res+= cp.sendDoubles(CrStrain,CrStress,Cplastic,CcurR,TcurR,getDbTagData(),CommMetaData(9));
    res+= cp.sendDoubles(TbStrain,TbStress,TrStrain,TrStress,Tplastic,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Steel03::recvData(const CommParameters &cp)
  {
    int res= SteelBase0103::recvData(cp);
    res+= cp.receiveDoubles(r,cR1,cR2,CbStrain,CbStress,getDbTagData(),CommMetaData(8));
    res+= cp.receiveDoubles(CrStrain,CrStress,Cplastic,CcurR,TcurR,getDbTagData(),CommMetaData(9));
    res+= cp.receiveDoubles(TbStrain,TbStress,TrStrain,TrStress,Tplastic,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Steel03::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(11);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Steel03::recvSelf(const CommParameters &cp)
  {
    inicComm(11);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::Steel03::Print (std::ostream& s, int flag)
  {
    s << "Steel03 tag: " << this->getTag() << std::endl;
    s << " fy: " << fy << " ";
    s << "  E0: " << E0 << " ";
    s << "  b: " << b << " ";
    s << "  r:  " << r << " cR1: " << cR1 << " cR2: " << cR2 << std::endl;
    s << "  a1: " << a1 << " ";
    s << "  a2: " << a2 << " ";
    s << "  a3: " << a3 << " ";
    s << "  a4: " << a4 << " ";
  }

