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
                                                                        
// $Revision: 1.10 $
// $Date: 2005/06/16 21:41:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/EPPGapMaterial.cpp,v $

// File: ~/material/EPPGapMaterial.C
//
// Written: krm 
// Created: 07/2000
// Revision: A
//
// Description: This file contains the class implementation for 
// ElasticMaterial. 
//
// What: "@(#) EPPGapMaterial.C, revA"

#include <material/uniaxial/EPPGapMaterial.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/element/utils/Information.h>
#include "domain/component/Parameter.h"

//! @brief Constructor.
XC::EPPGapMaterial::EPPGapMaterial(int tag, double e, double fyl, double gap0, double eta0, int accum)
  : EPPBaseMaterial(tag,MAT_TAG_EPPGap,e), fy(fyl),
    gap(gap0), eta(eta0),minElasticYieldStrain(gap0),damage(accum),
    parameterID(0), SHVs()
  {
    if(E == 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; E is zero, continuing with E = fy/0.002\n";
        if(fy != 0.0)
          E = fabs(fy)/0.002;
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; E and fy are zero\n";
            exit(-1);
          }
      }
    else
      maxElasticYieldStrain = fy/E + gap;

    if(fy*gap<0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; alternate signs on fy and gap encountered"
	          << ", continuing anyway\n";
      }
    if((eta >= 1) || (eta <= -1))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; value of eta must be -1 <= eta <= 1, setting eta to 0\n";
        eta = 0;
      }
    if((damage < 0) || (damage > 1))
      { std::cerr << getClassName() << "::" << __FUNCTION__
		  << "%s -- damage switch must be 0 or 1\n"; }
  }

//! @brief Constructor.
XC::EPPGapMaterial::EPPGapMaterial(int tag)
  :EPPBaseMaterial(tag,MAT_TAG_EPPGap), fy(0.0),
   gap(0.0), eta(0.0), maxElasticYieldStrain(0.0),
   minElasticYieldStrain(0.0), damage(0),
   parameterID(0), SHVs() {}

int XC::EPPGapMaterial::setTrialStrain(double strain, double strainRate)
  {
    // set the trial strain
    trialStrain = strain;

    // determine trial stress and tangent
    if(fy >= 0)
      {
        if(trialStrain > maxElasticYieldStrain)
          {
            trialStress = fy+(trialStrain-gap-fy/E)*eta*E;
            trialTangent = eta*E;
          }
        else if(trialStrain < minElasticYieldStrain)
          {
            trialStress = 0;
            trialTangent = 0;
          }
        else
          {
            trialStress =  E*(trialStrain-minElasticYieldStrain);
            trialTangent = E;
          }
      }
    else
      {
        if(trialStrain < maxElasticYieldStrain)
          {
            trialStress =  fy+(trialStrain-gap-fy/E)*eta*E;
            trialTangent = eta*E;
          }
        else if(trialStrain > minElasticYieldStrain)
          {
            trialStress =  0;
            trialTangent = 0;
          }
        else
          {
            trialStress =  E*(trialStrain-minElasticYieldStrain);
            trialTangent = E;
          }
      }
    return 0;
  }

//! @brief Return the initial stiffness.
double XC::EPPGapMaterial::getInitialTangent(void) const
  {
    if ((fy >= 0.0 && gap > 0.0) || (fy < 0.0 && gap < 0.0)) 
      return 0.0; 
    else 
      return E;
  }

int XC::EPPGapMaterial::commitState(void)
  {
    if(fy >= 0)
      {
        if(trialStrain > maxElasticYieldStrain)
          {
            maxElasticYieldStrain = trialStrain;
            minElasticYieldStrain = trialStrain-trialStress/E;
          }
        else if(trialStrain < minElasticYieldStrain && trialStrain > gap && damage == 0 )
          {
            maxElasticYieldStrain = (trialStrain-eta*gap)/(1-eta)+fy/E;
            minElasticYieldStrain = trialStrain;
          }
      }
    else
      {
        if(trialStrain < maxElasticYieldStrain)
          {
            maxElasticYieldStrain = trialStrain;
            minElasticYieldStrain = trialStrain-trialStress/E;
          }
       else if(trialStrain > minElasticYieldStrain && trialStrain < gap && damage == 0 )
          {
            maxElasticYieldStrain = (trialStrain-eta*gap)/(1-eta)+fy/E;
            minElasticYieldStrain = trialStrain;
          }
      }
    //added by SAJalali for energy recorder
    EnergyP+= 0.5*(commitStress + trialStress)*(trialStrain - commitStrain);

    commitStrain= trialStrain;
    commitStress = trialStress;	//added by SAJalali
    return 0;
  }


int XC::EPPGapMaterial::revertToLastCommit(void)
  {
    trialStrain= commitStrain;
    trialStress= commitStress;	//added by SAJalali
    return 0;
  }

//! @brief Revert the material to its initial state.
int XC::EPPGapMaterial::revertToStart(void)
  {
    int retval= EPPBaseMaterial::revertToStart();
    commitStrain= 0.0;
    trialStrain= 0.0;
    maxElasticYieldStrain = fy/E+gap;
    minElasticYieldStrain = gap;
    //added by SAJalali
    commitStress = 0;

// AddingSensitivity:BEGIN /////////////////////////////////
    if(!SHVs.isEmpty()) 
      SHVs.Zero();
// AddingSensitivity:END //////////////////////////////////

    return retval;
  }


XC::UniaxialMaterial *XC::EPPGapMaterial::getCopy(void) const
  { return new EPPGapMaterial(*this); }

//! @brief Send object members through the communicator argument.
int XC::EPPGapMaterial::sendData(Communicator &comm)
  {
    int res= EPPBaseMaterial::sendData(comm);
    res+= comm.sendDoubles(commitStrain,E,fy,gap,getDbTagData(),CommMetaData(4));
    res+= comm.sendDoubles(eta,maxElasticYieldStrain,minElasticYieldStrain,getDbTagData(),CommMetaData(5));
    res+= comm.sendInt(damage,getDbTagData(), CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::EPPGapMaterial::recvData(const Communicator &comm)
  {
    int res= EPPBaseMaterial::recvData(comm);
    res+= comm.receiveDoubles(commitStrain,E,fy,gap,getDbTagData(),CommMetaData(4));
    res+= comm.receiveDoubles(eta,maxElasticYieldStrain,minElasticYieldStrain,getDbTagData(),CommMetaData(5));
    res+= comm.receiveInt(damage,getDbTagData(), CommMetaData(6));
    return res;
  }

int XC::EPPGapMaterial::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data\n";
    return res;
  }

int XC::EPPGapMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        res= recvData(comm);
        if(res < 0)
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; - failed to receive data\n";
      }
    return res;
  }

void XC::EPPGapMaterial::Print(std::ostream &s, int flag) const
  {
    s << "EPPGap tag: " << this->getTag() << std::endl;
    s << "  E: " << E << ", kinematic hardening ratio: " << eta << std::endl;
    s << "  fy: " << fy << std::endl;
    s << "  initial gap: " << gap << std::endl;
    if (damage == 1)
        s << "  damage accumulation specified" << std::endl;
  }

int XC::EPPGapMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv[0]=="E")
      {
        param.setValue(E);
        return param.addObject(1, this);
      }
    if((argv[0]=="Fy") || (argv[0]=="fy"))
      {
        param.setValue(fy);
        return param.addObject(2, this);
      }
    if(argv[0]=="gap")
      {
        param.setValue(gap);
        return param.addObject(3, this);
      }
    return 0;
  }

int XC::EPPGapMaterial::updateParameter(int parameterID, Information &info)
  {
    switch (parameterID)
      {
      case 1:
	E = info.theDouble;
	break;
      case 2:
	fy = info.theDouble;
	break;
      case 3:
	gap = info.theDouble;
	break;
      default:
	return -1;
      }

    return 0;
  }

int XC::EPPGapMaterial::activateParameter(int passedParameterID)
  {
    parameterID = passedParameterID;
    return 0;
  }

double XC::EPPGapMaterial::getStressSensitivity(int gradIndex, bool conditional)
  {
    double dsdh = 0.0;

    double dEdh = 0.0;
    double dfydh = 0.0;
    double dgapdh = 0.0;

    if (parameterID == 1)
      dEdh = 1.0;
    if (parameterID == 2)
      dfydh = 1.0;
    if (parameterID == 3)
      dgapdh = 1.0;

    double depsyMindh = 0.0;
    if (!SHVs.isEmpty())
      { depsyMindh = SHVs(0,gradIndex); }

    if (fy >= 0)
      {
      if (trialStrain > maxElasticYieldStrain) {
	dsdh = dfydh + (-dgapdh-dfydh/E+fy/(E*E)*dEdh)*eta*E + (trialStrain-gap-fy/E)*eta*dEdh;
      } else if (trialStrain < minElasticYieldStrain) {
	dsdh = 0.0;
      } else {
	dsdh = dEdh*(trialStrain-minElasticYieldStrain) - E*depsyMindh;
      }
    } else {
      if (trialStrain < maxElasticYieldStrain) {
	dsdh = dfydh + (-dgapdh-dfydh/E+fy/(E*E)*dEdh)*eta*E + (trialStrain-gap-fy/E)*eta*dEdh;
      } else if (trialStrain > minElasticYieldStrain) {
	dsdh = 0.0;
      } else {
	dsdh = dEdh*(trialStrain-minElasticYieldStrain) - E*depsyMindh;
      }
    }

    return dsdh;
  }

double XC::EPPGapMaterial::getTangentSensitivity(int gradIndex)
  { return 0.0; }

double XC::EPPGapMaterial::getInitialTangentSensitivity(int gradIndex)
  {
    if(parameterID == 1)
      return 1.0;
    else
      return 0.0;
  }

int XC::EPPGapMaterial::commitSensitivity(double dedh, int gradIndex, int numGrads)
  {
    if(SHVs.isEmpty())
      { SHVs= Matrix(1,numGrads); }

    //return 0;

    if (gradIndex >= SHVs.noCols())
      {
        //std::cerr << gradIndex << ' ' << SHVs.noCols() << endln;
        return 0;
      }

    double dEdh = 0.0;
    double dfydh = 0.0;
    double dgapdh = 0.0;

    if(parameterID == 1)
      dEdh = 1.0;
    if(parameterID == 2)
      dfydh = 1.0;
    if(parameterID == 3)
      dgapdh = 1.0;

   double depsyMindh = SHVs(0,gradIndex);

    if (fy >= 0)
      {
        if (trialStrain > maxElasticYieldStrain)
	  {
	    double dsdh = this->getStressSensitivity(gradIndex, true);
	    dsdh += eta*E*dedh; // unconditional
	    depsyMindh = dedh - (-trialStress)/(E*E)*dEdh - dsdh/E;
          }
        else if(trialStrain < minElasticYieldStrain && trialStrain > gap && damage == 0)
	  depsyMindh = dedh;
      }
    else
      {
        if(trialStrain < maxElasticYieldStrain)
	  {
	    double dsdh = this->getStressSensitivity(gradIndex, true);
	    dsdh += eta*E*dedh; // unconditional
	    depsyMindh = dedh - (-trialStress)/(E*E)*dEdh - dsdh/E;
	  }
      else if(trialStrain > minElasticYieldStrain && trialStrain < gap && damage == 0)
	depsyMindh = dedh;
      }
    SHVs(0,gradIndex) = depsyMindh;
    return 0;
  }
