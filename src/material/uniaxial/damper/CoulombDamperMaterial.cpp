//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
/* ******************************************************************
***
**    OpenSees - Open System for Earthquake Engineering Simulation **
**          Pacific Earthquake Engineering Research Center **
** **
** **
** (C) Copyright 1999, The Regents of the University of California **
** All Rights Reserved. **
** **
** Commercial use of this program without express permission of the **
** University of California, Berkeley, is strictly prohibited.  See **
** file 'COPYRIGHT'  in main directory for information on usage and **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES. **
** **
** Developed by: **
**   Frank McKenna (fmckenna@ce.berkeley.edu) **
**   Gregory L. Fenves (fenves@ce.berkeley.edu) **
**   Filip C. Filippou (filippou@ce.berkeley.edu) **
** **
** ******************************************************************
*/

#include "material/uniaxial/damper/CoulombDamperMaterial.h"
#include "utility/matrix/Vector.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/component/Parameter.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::CoulombDamperMaterial::CoulombDamperMaterial(int tag, double k,
                                             double fc, double t,
                                             double damp, int m,
                                             int n)
    : UniaxialMaterial(tag, MAT_TAG_CoulombDamperMaterial),
      trialStrain(0.0),
      trialStrainRate(0.0),
      tangent(k),
      friction(fc),
      commitTrialStrainRate(0),
      flipped(0),
      tol(t),
      dampOutTangent(damp),
      method(m),
      numFlipped(n),
      parameterID(0) {}

//! @brief Default constructor.
XC::CoulombDamperMaterial::CoulombDamperMaterial(int tag)
    : UniaxialMaterial(tag, MAT_TAG_CoulombDamperMaterial),
      trialStrain(0.0),
      trialStrainRate(0.0),
      tangent(0.0),
      friction(0.0),
      commitTrialStrainRate(0),
      flipped(0),
      tol(1e-6),
      dampOutTangent(1.0),
      method(1),
      numFlipped(2),
      parameterID(0) {}

int XC::CoulombDamperMaterial::setTrialStrain(double strain,
                                          double strainRate)
  {
    trialStrain = strain;
    trialStrainRate = strainRate;

    // flipped
    if ((commitTrialStrainRate > 0 && trialStrainRate < -tol) ||
        (commitTrialStrainRate < 0 && trialStrainRate > tol))
      { ++flipped; }

    return 0;
  }

int XC::CoulombDamperMaterial::setTrial(double strain, double &stress,
                                    double &tan, double strainRate)
  {
    trialStrain = strain;
    trialStrainRate = strainRate;

    stress = tangent * strain + sign();
    tangent = tan;

    // flipped
    if ((commitTrialStrainRate > 0 && trialStrainRate < -tol) ||
        (commitTrialStrainRate < 0 && trialStrainRate > tol))
      { ++flipped; }

    return 0;
  }

double XC::CoulombDamperMaterial::getStress(void) const
  {
    return tangent * trialStrain + sign();
  }

double XC::CoulombDamperMaterial::getTangent(void) const
  { return tangent; }

double XC::CoulombDamperMaterial::getDampTangent(void) const
  { return dsign(); }

double XC::CoulombDamperMaterial::getInitialTangent(void) const
  {
    return tangent;
  }

int XC::CoulombDamperMaterial::commitState(void)
  {
    commitTrialStrainRate = trialStrainRate;
    flipped = 0;
    return 0;
  }

int XC::CoulombDamperMaterial::revertToLastCommit(void)
  {
    trialStrainRate = commitTrialStrainRate;
    flipped = 0;
    return 0;
  }

int XC::CoulombDamperMaterial::revertToStart(void)
  {
    trialStrain = 0.0;
    trialStrainRate = 0.0;
    commitTrialStrainRate = 0.0;
    flipped = 0;
    return 0;
  } 

XC::UniaxialMaterial *XC::CoulombDamperMaterial::getCopy(void) const
  { return new CoulombDamperMaterial(*this); }

int XC::CoulombDamperMaterial::sendData(Communicator &comm)
  {
    int res = UniaxialMaterial::sendData(comm);
    res+= comm.sendDoubles(trialStrain, trialStrainRate, tangent, friction, getDbTagData(),CommMetaData(2));
    res+= comm.sendDoubles(commitTrialStrainRate, tol, dampOutTangent, getDbTagData(),CommMetaData(3));
    res+= comm.sendInts(flipped, method, numFlipped, getDbTagData(),CommMetaData(4));
    return res;
  }

int XC::CoulombDamperMaterial::recvData(const Communicator &comm)
  {
    int res = UniaxialMaterial::recvData(comm);
    res+= comm.receiveDoubles(trialStrain, trialStrainRate, tangent, friction, getDbTagData(),CommMetaData(2));
    res+= comm.receiveDoubles(commitTrialStrainRate, tol, dampOutTangent, getDbTagData(),CommMetaData(3));
    res+= comm.receiveInts(flipped, method, numFlipped, getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::CoulombDamperMaterial::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(5);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::CoulombDamperMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids."
	        << Color::def << std::endl;
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
           std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		     << "; failed to receive data."
	             << Color::def << std::endl;
      }
    return res;
  }

void XC::CoulombDamperMaterial::Print(std::ostream &s, int flag) const
  {
    s << "CoulombDamperMaterial tag: " << this->getTag() << std::endl
      << "  Tangent: " << tangent
      << ", Friciton force: " << friction
      << std::endl;
  }

int XC::CoulombDamperMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv[0]=="Tangent")
      {
        param.setValue(tangent);
        return param.addObject(1, this);
      }
    if(argv[0]=="Friction")
      {
        param.setValue(friction);
        return param.addObject(2, this);
      }
    return -1;
  }

int XC::CoulombDamperMaterial::updateParameter(int parameterID, Information &info)
  {
    switch (parameterID)
      {
      case 1:
	tangent = info.theDouble;
	return 0;
      case 2:
	friction = info.theDouble;
	return 0;
      default:
	return -1;
      }
  }

int XC::CoulombDamperMaterial::activateParameter(int paramID)
  {
    parameterID = paramID;

    return 0;
  }

double XC::CoulombDamperMaterial::getStressSensitivity(int gradIndex, bool conditional)
  {
    if (parameterID == 1) return trialStrain;
    if (parameterID == 2) return sign();
    return 0.0;
  }

double XC::CoulombDamperMaterial::getTangentSensitivity(int gradIndex)
  {
    if (parameterID == 1) return 1.0;
    return 0.0;
  }

double XC::CoulombDamperMaterial::getDampTangentSensitivity(int gradIndex)
  {
    if (parameterID == 2)
      {
        return dsign();
      }
    return 0.0;
  }

double XC::CoulombDamperMaterial::getInitialTangentSensitivity(int gradIndex)
  {
    if (parameterID == 1) return 1.0;
    return 0.0;
  }

int XC::CoulombDamperMaterial::commitSensitivity(double strainGradient, int gradIndex, int numGrads)
  {
    // Nothing to commit ... path independent
    return 0;
  }

double XC::CoulombDamperMaterial::sign() const
  {
    double res = 0.0;
    double dampTangent = friction / tol;

    if (flipped > numFlipped)
      {
        // rate flipped n times
        if (method == 1)
	  {
            res = factor() * dampTangent * trialStrainRate;
	  }
	else if (method == 2)
	  {
            if (trialStrainRate > tol)
	      {
                res = factor() * friction;
	      }
	    else if (trialStrainRate < -tol)
	      {
                res = -factor() * friction;
	      }
	  }
	else if (dampOutTangent == 3)
	  {
            res = dampOutTangent * trialStrainRate;
	  }

      }
    else if (trialStrainRate > -tol && trialStrainRate < tol)
      {
        // linear rate
        res = dampTangent * trialStrainRate;

      }
    else
      {
        // rate not flipped
        if (trialStrainRate > tol)
	  {
            // rate > 0
            res = friction;
	  }
	else if (trialStrainRate < -tol)
	  {
            // rate < 0
            res = -friction;
	  }
      }

    return res;
  }

double XC::CoulombDamperMaterial::dsign() const
  {
    double res= 0.0;
    double dampTangent = friction / tol;

    if (flipped > numFlipped)
      {
        // rate flipped n times
        if (method == 1)
	  {
            res = factor() * dampTangent;
	  }
	else if (method == 2)
	  { res = 0.0; }
	else if (method == 3)
	  { res = dampOutTangent; }
      }
    else if (trialStrainRate > -tol && trialStrainRate < tol)
      {
        // linear rate
        res = dampTangent;	
      }
    else
      { res = 0.0; }

    return res;
  }

double XC::CoulombDamperMaterial::factor() const
  {
    double res = 1.0;
    for (int i = 0; i < flipped; i += 2)
      { res *= 0.5; }
    return res;
  }
