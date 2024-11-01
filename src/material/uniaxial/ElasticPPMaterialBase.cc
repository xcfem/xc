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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ElasticPPMaterialBase.cpp,v $
                                                                        
                                                                        
// File: ~/material/ElasticPPMaterialBase.C
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class implementation for 
// ElasticMaterial. 
//
// What: "@(#) ElasticPPMaterialBase.C, revA"


#include <material/uniaxial/ElasticPPMaterialBase.h>
#include <utility/matrix/Vector.h>
#include <cmath>
#include <cfloat>
#include "utility/utils/misc_utils/colormod.h"


//! @brief Sets the positive yield stress value (tension).
void XC::ElasticPPMaterialBase::set_fyp(const double &f)
  {
    fyp= f;
    if(fyp < 0)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; fyp < 0, setting > 0"
	          << Color::def << std::endl;
        fyp*= -1.;
      }
  }

//! @brief Sets the positive yield strain value (tension).
void XC::ElasticPPMaterialBase::set_eyp(const double &eyp)
  { set_fyp(E*eyp); }

//! @brief Set the yield stress a compression value.
void XC::ElasticPPMaterialBase::set_fyn(const double &f)
  {
    fyn= f;
    if(fyn > 0)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; fyn > 0, setting < 0"
	          << Color::def << std::endl;
        fyn*= -1.;
      }  
  }

//! @brief Asigna el yield stress a compression value.
void XC::ElasticPPMaterialBase::set_eyn(const double &eyn)
  { set_fyn(E*eyn); }

//! @brief Constructor.
//! @param[in] tag material identifier.
//! @param[in] e material elastic modulus.
//! @param[in] eyp positive yield strain value (tension).
XC::ElasticPPMaterialBase::ElasticPPMaterialBase(int tag, int classTag, double e, double eyp)
  :EPPBaseMaterial(tag, classTag, e, 0.0), fyp(0.0), fyn(0.0),
   ep(0.0), commitStress(0.0), EnergyP(0.0)
  {
    set_eyp(eyp);
    fyn = -fyp;
  }

//! @brief Constructor.
//! @param[in] tag material identifier.
//! @param[in] e material elastic modulus.
//! @param[in] eyp positive yield strain value (tension).
//! @param[in] eyn negative yield strain value (compression).
//! @param[in] ez initial strain.
XC::ElasticPPMaterialBase::ElasticPPMaterialBase(int tag, int classTag, double e, double eyp,double eyn, double ez )
  :EPPBaseMaterial(tag, classTag, e, ez), fyp(0.0), fyn(0.0),
   ep(0.0), commitStress(0.0), EnergyP(0.0)
  {
    set_eyp(eyp);
    set_eyn(eyn);
  }

//! @brief Constructor.
XC::ElasticPPMaterialBase::ElasticPPMaterialBase(int tag, int classTag)
  :EPPBaseMaterial(tag, classTag, 0.0, 0.0), fyp(0.0), fyn(0.0),
   ep(0.0), commitStress(0.0), EnergyP(0.0) {}

//! @brief Sets trial strain.
int XC::ElasticPPMaterialBase::setTrialStrain(double strain, double strainRate)
  {
  /*
    if (fabs(trialStrain - strain) < DBL_EPSILON)
      return 0;
  */
    trialStrain = strain;

    // compute trial stress
    const double sigtrial= E * get_total_strain(); // trial stress

    const double f= yield_function(sigtrial); //yield function

    const double fYieldSurface= - E * DBL_EPSILON;
    if(f<=fYieldSurface )
      {
        // elastic
        trialStress= sigtrial;
        trialTangent= E;
      }
    else
      {
        // plastic
        if(sigtrial>0.0)
          { trialStress = fyp; }
        else
          { trialStress = fyn; }
        trialTangent = 0.0;
      }
    return 0;
  }

//! @brief Commit material state.
int XC::ElasticPPMaterialBase::commitState(void)
  {

    // compute trial stress
    const double sigtrial= E * get_total_strain(); // trial stress

    const double f= yield_function(sigtrial); //yield function

    const double fYieldSurface= - E * DBL_EPSILON;
    if(f>fYieldSurface)
      {
        // plastic
        if(sigtrial>0.0)
          { ep+= f / E; }
        else
          { ep-= f / E; }
      }
    //added by SAJalali for energy recorder
    EnergyP+= 0.5*(commitStress + trialStress)*(trialStrain - commitStrain);
    
    EPPBaseMaterial::commitState();
    commitStress= trialStress;
    return 0;
  }        

//! @brief Returns the material state to its last commit.
int XC::ElasticPPMaterialBase::revertToLastCommit(void)
  {
    EPPBaseMaterial::revertToLastCommit();
    trialStress = commitStress;
    return 0;
  }


//! @brief Returns the material to its initial state.
int XC::ElasticPPMaterialBase::revertToStart(void)
  {
    int retval= EPPBaseMaterial::revertToStart();
    ep = 0.0; // plastic strain at last commit
    commitStress= 0.0;
    EnergyP = 0.0; //by SAJalali
    return retval;
  }

//! @brief Virtual constructor.
XC::UniaxialMaterial *XC::ElasticPPMaterialBase::getCopy(void) const
  { return new ElasticPPMaterialBase(*this); }

//! @brief Send object members through the communicator argument.
int XC::ElasticPPMaterialBase::sendData(Communicator &comm)
  {
    int res= EPPBaseMaterial::sendData(comm);
    res+= comm.sendDoubles(fyp, fyn, ep, commitStress, EnergyP, getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ElasticPPMaterialBase::recvData(const Communicator &comm)
  {
    int res= EPPBaseMaterial::recvData(comm);
    res+= comm.receiveDoubles(fyp, fyn, ep, commitStress, EnergyP, getDbTagData(),CommMetaData(4));
    return res;
  }

int XC::ElasticPPMaterialBase::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(5);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; WARNING: " 
                << dataTag
		<< " failed to send."
	        << Color::def << std::endl;
    return res;
  }

int XC::ElasticPPMaterialBase::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; WARNING: "
                << dataTag
		<< " failed to receive ID"
	        << Color::def << std::endl;
    else
      res+= recvData(comm);
    return res;
  }

void XC::ElasticPPMaterialBase::Print(std::ostream &s, int flag) const
  {
    s << "ElasticPP tag: " << this->getTag()
      << std::endl
      << "  E: " << E
      << std::endl
      << "  ep: " << ep
      << std::endl
      << "  Otress: " << trialStress
      << " tangent: " << trialTangent
      << std::endl;
  }

double XC::ElasticPPMaterialBase::get_fyp(void) const
  { return fyp; }

double XC::ElasticPPMaterialBase::get_eyp(void) const
  { return fyp/E; }

double XC::ElasticPPMaterialBase::get_fyn(void) const
  { return fyn; }

double XC::ElasticPPMaterialBase::get_eyn(void) const
  { return fyn/E; }
