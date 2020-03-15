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

// $Revision: 1.11 $
// $Date: 2003/03/11 03:49:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/HardeningMaterial.cpp,v $

// Written: MHS
// Created: May 2000
// Revision: A
//
// Description: This file contains the class implementation for 
// HardeningMaterial. 

#include "HardeningMaterial.h"
#include "FEProblem.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/domain/Domain.h"
#include "domain/component/Parameter.h"
#include <utility/matrix/Vector.h>
#include <cmath>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/utils/Information.h>
#include <cfloat>
#include "utility/actor/actor/MatrixCommMetaData.h"

XC::HardeningMaterial::HardeningMaterial(int tag, double e, double s,
                                     double hi, double hk, double n)
  :UniaxialMaterial(tag,MAT_TAG_Hardening), E(e), sigmaY(s), Hiso(hi), Hkin(hk), eta(n), parameterID(0), SHVs(nullptr)
  {
    // Initialize variables
    this->revertToStart();
  }

XC::HardeningMaterial::HardeningMaterial(int tag)
:UniaxialMaterial(tag,MAT_TAG_Hardening), E(0.0), sigmaY(0.0), Hiso(0.0), Hkin(0.0), eta(0.0), parameterID(0), SHVs(nullptr)
  {
    // Initialize variables
    this->revertToStart();
  }

XC::HardeningMaterial::HardeningMaterial(void)
:UniaxialMaterial(0,MAT_TAG_Hardening), E(0.0), sigmaY(0.0), Hiso(0.0), Hkin(0.0), eta(0.0), parameterID(0), SHVs(nullptr)
  {
    // Initialize variables
    this->revertToStart();
  }

XC::HardeningMaterial::HardeningMaterial(const HardeningMaterial &other)
:UniaxialMaterial(other), E(other.E), sigmaY(other.sigmaY), Hiso(other.Hiso),
 Hkin(other.Hkin), eta(other.eta),
 CplasticStrain(other.CplasticStrain), CbackStress(other.CbackStress),
 Chardening(other.Chardening), TplasticStrain(other.TplasticStrain),
 TbackStress(other.TbackStress), Thardening(other.Thardening),
 Tstrain(other.Tstrain), Tstress(other.Tstress), Ttangent(other.Ttangent),
 parameterID(0), SHVs(nullptr)
  {}


XC::HardeningMaterial &XC::HardeningMaterial::operator=(const HardeningMaterial &other)
  {
    UniaxialMaterial::operator=(other);
    E= other.E;
    sigmaY= other.sigmaY;
    Hiso= other.Hiso;
    Hkin= other.Hkin;
    eta= other.eta;
    CplasticStrain= other.CplasticStrain;
    CbackStress= other.CbackStress;
    Chardening= other.Chardening;
    TplasticStrain= other.TplasticStrain;
    TbackStress= other.TbackStress;
    Thardening= other.Thardening;
    Tstrain= other.Tstrain;
    Tstress= other.Tstress;
    Ttangent= other.Ttangent;
    return *this;
  }


XC::HardeningMaterial::~HardeningMaterial(void)
  {
// AddingSensitivity:BEGIN /////////////////////////////////////
     if(SHVs) 
       delete SHVs;
     SHVs= nullptr;
// AddingSensitivity:END //////////////////////////////////////
  }

int XC::HardeningMaterial::setTrialStrain(double strain, double strainRate)
  {

    if(fabs(Tstrain-strain) < DBL_EPSILON)
      return 0;

    // Set total strain
    Tstrain = strain;
    
    // Elastic trial stress
    Tstress = E * (Tstrain-CplasticStrain);

    // Compute trial stress relative to committed back stress
    double xsi = Tstress - CbackStress;

    // Compute yield criterion
    double f = fabs(xsi) - (sigmaY + Hiso*Chardening);

    // Elastic step ... no updates required
//    if (f <= -DBL_EPSILON * E)
    if (f <= 1.0e-8)
    {
        // Set trial tangent
        Ttangent = E;
    }

    // Plastic step ... perform return mapping algorithm
    else
      {
        double etadt = 0.0;
        const double dT= FEProblem::theActiveDomain->getTimeTracker().getDt();
      if (eta != 0.0 || dT != 0)
          etadt = eta/dT;

      // Compute consistency parameter
      double dGamma = f / (E+Hiso+Hkin+etadt);

      // Find sign of xsi
      int sign = (xsi < 0) ? -1 : 1;

      // Bring trial stress back to yield surface
      Tstress -= dGamma*E*sign;
        
      // Update plastic strain
      TplasticStrain = CplasticStrain + dGamma*sign;
        
      // Update back stress
      TbackStress = CbackStress + dGamma*Hkin*sign;
        
      // Update internal hardening variable
      Thardening = Chardening + dGamma;
        
      // Set trial tangent
      Ttangent = E*(Hkin+Hiso+etadt) / (E+Hkin+Hiso+etadt);
    }

    return 0;
}

double XC::HardeningMaterial::getStress(void) const
  { return Tstress; }

double XC::HardeningMaterial::getTangent(void) const
  { return Ttangent; }

double XC::HardeningMaterial::getStrain(void) const
  { return Tstrain; }

int XC::HardeningMaterial::commitState(void)
{
    // Commit trial history variables
    CplasticStrain = TplasticStrain;
    CbackStress = TbackStress;
    Chardening = Thardening;
    
    return 0;
}

int XC::HardeningMaterial::revertToLastCommit(void)
  { return 0; }

int XC::HardeningMaterial::revertToStart(void)
  {
    // Reset committed history variables
    CplasticStrain = 0.0;
    CbackStress = 0.0;
    Chardening = 0.0;

    // Reset trial history variables
    TplasticStrain = 0.0;
    TbackStress = 0.0;
    Thardening = 0.0;

        // Initialize state variables
        Tstrain = 0.0;
        Tstress = 0.0;
        Ttangent = E;

// AddingSensitivity:BEGIN /////////////////////////////////
        if (SHVs != 0) 
                SHVs->Zero();
// AddingSensitivity:END //////////////////////////////////

    return 0;
  }

XC::UniaxialMaterial *XC::HardeningMaterial::getCopy(void) const
  { return new HardeningMaterial(*this); }

//! @brief Send object members through the channel being passed as parameter.
int XC::HardeningMaterial::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendDoubles(E,sigmaY,Hiso,Hkin,eta,getDbTagData(),CommMetaData(2));
    res+= cp.sendDoubles(CplasticStrain,CbackStress,Chardening,TplasticStrain,TbackStress,Thardening,getDbTagData(),CommMetaData(3));
    res+= cp.sendDoubles(Tstrain,Tstress,Ttangent,getDbTagData(),CommMetaData(4));
    setDbTagDataPos(5,parameterID);
    res+= cp.sendMatrixPtr(SHVs,getDbTagData(),MatrixCommMetaData(6,7,8,9));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::HardeningMaterial::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveDoubles(E,sigmaY,Hiso,Hkin,eta,getDbTagData(),CommMetaData(2));
    res+= cp.receiveDoubles(CplasticStrain,CbackStress,Chardening,TplasticStrain,TbackStress,Thardening,getDbTagData(),CommMetaData(3));
    res+= cp.receiveDoubles(Tstrain,Tstress,Ttangent,getDbTagData(),CommMetaData(4));
    parameterID= getDbTagDataPos(5);
    SHVs= cp.receiveMatrixPtr(SHVs,getDbTagData(),MatrixCommMetaData(6,7,8,9));
    return res;
  }

int XC::HardeningMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(10); 
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "HardeningMaterial::sendSelf - failed to send data.\n";
    return res;
  }

int XC::HardeningMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "HardeningMaterial::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
           std::cerr << "HardeningMaterial::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::HardeningMaterial::Print(std::ostream &s, int flag) const
  {
    s << "HardeningMaterial, tag: " << this->getTag() << std::endl;
    s << "  E: " << E << std::endl;
    s << "  sigmaY: " << sigmaY << std::endl;
    s << "  Hiso: " << Hiso << std::endl;
    s << "  Hkin: " << Hkin << std::endl;
    s << "  eta: " << eta << std::endl;
  }


// AddingSensitivity:BEGIN ///////////////////////////////////
int XC::HardeningMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv.size() < 1)
      return -1;
    else if(argv[0] == "sigmaY" || argv[0] == "fy")
      return param.addObject(1, this);
    else if(argv[0] == "E")
      return param.addObject(2, this);
    else if(argv[0] == "H_kin")
      return param.addObject(3, this);
    else if(argv[0] == "H_iso")
      return param.addObject(4, this);
    else
      std::cerr << "WARNING: Could not set parameter in XC::HardeningMaterial. " << std::endl;
    return -1;
  }

int XC::HardeningMaterial::updateParameter(int parameterID, Information &info)
{
        switch (parameterID) {
        case -1:
                return -1;
        case 1:
                this->sigmaY = info.theDouble;
                break;
        case 2:
                this->E = info.theDouble;
                break;
        case 3:
                this->Hkin = info.theDouble;
                break;
        case 4:
                this->Hiso = info.theDouble;
                break;
        default:
                return -1;
        }

        return 0;
}



int XC::HardeningMaterial::activateParameter(int passedParameterID)
  {
        parameterID = passedParameterID;
        return 0;
  }




double XC::HardeningMaterial::getStressSensitivity(int gradNumber, bool conditional)
  {

        // First set values depending on what is random
        double SigmaYSensitivity = 0.0;
        double ESensitivity = 0.0;
        double HkinSensitivity = 0.0;
        double HisoSensitivity = 0.0;

        if (parameterID == 1) {  // sigmaY
                SigmaYSensitivity = 1.0;
        }
        else if (parameterID == 2) {  // E
                ESensitivity = 1.0;
        }
        else if (parameterID == 3) {  // Hkin
                HkinSensitivity = 1.0;
        }
        else if (parameterID == 4) {  // Hiso
                HisoSensitivity = 1.0;
        }
        else {
                // Nothing random here, but may have to return something in any case
        }

        // Then pick up history variables for this gradient number
        double CplasticStrainSensitivity = 0.0;
        double CbackStressSensitivity         = 0.0;
        double ChardeningSensitivity         = 0.0;
        if (SHVs != 0) {
                CplasticStrainSensitivity = (*SHVs)(0,(gradNumber-1));
                CbackStressSensitivity         = (*SHVs)(1,(gradNumber-1));
                ChardeningSensitivity         = (*SHVs)(2,(gradNumber-1));
        }

        double sensitivity;
        // Elastic step
        if ( fabs(TplasticStrain-CplasticStrain) < 1.0e-8) { 

                sensitivity = ESensitivity*(Tstrain-CplasticStrain)-E*CplasticStrainSensitivity;

        }

        // Plastic step
        else { 

                double myTstress = E * (Tstrain-CplasticStrain);

                double xsi = myTstress - CbackStress;
                
                double f = fabs(xsi) - (sigmaY + Hiso*Chardening);
                
                double TstressSensitivity = ESensitivity*(Tstrain-CplasticStrain)-E*CplasticStrainSensitivity;
                
                int sign = (xsi < 0) ? -1 : 1;
                
                double dGamma = f / (E+Hiso+Hkin);
                
                double fSensitivity = (TstressSensitivity-CbackStressSensitivity)*sign
                        - SigmaYSensitivity - HisoSensitivity*Chardening - Hiso*ChardeningSensitivity;
                
                double dGammaSensitivity = 
                        (fSensitivity*(E+Hkin+Hiso)-f*(ESensitivity+HkinSensitivity+HisoSensitivity))
                        /((E+Hkin+Hiso)*(E+Hkin+Hiso));
                
                sensitivity = (TstressSensitivity-dGammaSensitivity*E*sign-dGamma*ESensitivity*sign);
        }

        return sensitivity;
  }



double XC::HardeningMaterial::getInitialTangentSensitivity(int gradNumber)
  {
        // For now, assume that this is only called for initial stiffness 
        if (parameterID == 2) {
                return 1.0; 
        }
        else {
                return 0.0;
        }
  }


int XC::HardeningMaterial::commitSensitivity(double TstrainSensitivity, int gradNumber, int numGrads)
  {
        if (SHVs == 0) {
                SHVs = new Matrix(3,numGrads);
        }

        // First set values depending on what is random
        double SigmaYSensitivity = 0.0;
        double ESensitivity = 0.0;
        double HkinSensitivity = 0.0;
        double HisoSensitivity = 0.0;

        if (parameterID == 1) {  // sigmaY
                SigmaYSensitivity = 1.0;
        }
        else if (parameterID == 2) {  // E
                ESensitivity = 1.0;
        }
        else if (parameterID == 3) {  // Hkin
                HkinSensitivity = 1.0;
        }
        else if (parameterID == 4) {  // Hiso
                HisoSensitivity = 1.0;
        }
        else {
                // Nothing random here, but may have to save SHV's in any case
        }

        // Then pick up history variables for this gradient number
        double CplasticStrainSensitivity= (*SHVs)(0,(gradNumber-1));
        double CbackStressSensitivity        = (*SHVs)(1,(gradNumber-1));
        double ChardeningSensitivity        = (*SHVs)(2,(gradNumber-1));

        // Elastic step
        if ( fabs(TplasticStrain-CplasticStrain) < 1.0e-8) { 
                // No changes in the sensitivity history variables
        }

        // Plastic step
        else { 

                double myTstress = E * (Tstrain-CplasticStrain);

                double xsi = myTstress - CbackStress;
                
                double f = fabs(xsi) - (sigmaY + Hiso*Chardening);

                double TstressSensitivity = ESensitivity*(Tstrain-CplasticStrain)
                        + E*(TstrainSensitivity-CplasticStrainSensitivity);

                int sign = (xsi < 0) ? -1 : 1;

                double dGamma = f / (E+Hiso+Hkin);

                double fSensitivity = (TstressSensitivity-CbackStressSensitivity)*sign
                        - SigmaYSensitivity - HisoSensitivity*Chardening - Hiso*ChardeningSensitivity;

                double dGammaSensitivity = 
                        (fSensitivity*(E+Hkin+Hiso)-f*(ESensitivity+HkinSensitivity+HisoSensitivity))
                        /((E+Hkin+Hiso)*(E+Hkin+Hiso));

                (*SHVs)(0,(gradNumber-1)) += dGammaSensitivity*sign;
                (*SHVs)(1,(gradNumber-1)) += dGammaSensitivity*Hkin*sign + dGamma*HkinSensitivity*sign;
                (*SHVs)(2,(gradNumber-1)) += dGammaSensitivity;

        }

        return 0;
  }
// AddingSensitivity:END /////////////////////////////////////////////

