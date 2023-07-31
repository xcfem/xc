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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/04/02 22:02:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/MinMaxMaterial.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: This file contains the class definition for 
// MinMaxMaterial.  MinMaxMaterial wraps a UniaxialMaterial
// and imposes min and max strain limits.

#include <cstdlib>

#include <material/uniaxial/MinMaxMaterial.h>
#include <utility/matrix/ID.h>
#include "utility/matrix/Vector.h"

XC::MinMaxMaterial::MinMaxMaterial(int tag, UniaxialMaterial &material, double min, double max)
  :EncapsulatedMaterial(tag,MAT_TAG_MinMax,material),
   minStrain(min), maxStrain(max), Tfailed(false), Cfailed(false)
  {}

XC::MinMaxMaterial::MinMaxMaterial(int tag)
  :EncapsulatedMaterial(tag,MAT_TAG_MinMax),
   minStrain(0.0), maxStrain(0.0), Tfailed(false), Cfailed(false) {}

int XC::MinMaxMaterial::setTrialStrain(double strain, double strainRate)
  {
    if(Cfailed)
      return 0;
    if(strain >= maxStrain || strain <= minStrain)
      {
        Tfailed = true;
        return 0;
      }
    else
      {
        Tfailed = false;
        return this->getMaterial()->setTrialStrain(strain, strainRate);
      }
  }

double XC::MinMaxMaterial::getStress(void) const
  {
    if(Tfailed)
      return 0.0;
    else
      return this->getMaterial()->getStress();
  }

//! @brief Return the material tangent stiffness.
double XC::MinMaxMaterial::getTangent(void) const
  {
    const UniaxialMaterial *tmp= this->getMaterial();
    if(Tfailed)
      //return 0.0;
      return 1.0e-8*tmp->getInitialTangent();
    else
      return tmp->getTangent();
  }

double XC::MinMaxMaterial::getDampTangent(void) const
  {
    if(Tfailed)
      return 0.0;
    else
      return this->getMaterial()->getDampTangent();
  }

//! @brief Commit the state of the material.
int XC::MinMaxMaterial::commitState(void)
  {        
    Cfailed = Tfailed;

    // Check if failed at current step
    if(Tfailed)
      return 0;
    else
      return this->getMaterial()->commitState();
  }

int XC::MinMaxMaterial::revertToLastCommit(void)
  {
    // Check if failed at last step
    if(Cfailed)
      return 0;
    else
      return this->getMaterial()->revertToLastCommit();
  }

int XC::MinMaxMaterial::revertToStart(void)
  {
    int retval= EncapsulatedMaterial::revertToStart();
    Cfailed = false;
    Tfailed = false;
    retval+= this->getMaterial()->revertToStart();
    return retval;
  }

XC::UniaxialMaterial *XC::MinMaxMaterial::getCopy(void) const
  { return new MinMaxMaterial(*this); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::MinMaxMaterial::getDbTagData(void) const
  {
    static DbTagData retval(6);
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::MinMaxMaterial::sendData(Communicator &comm)
  {
    int res= EncapsulatedMaterial::sendData(comm);
    res+= comm.sendDoubles(minStrain,maxStrain,getDbTagData(),CommMetaData(4));
    res+= comm.sendBools(Tfailed, Cfailed, getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::MinMaxMaterial::recvData(const Communicator &comm)
  {
    int res= EncapsulatedMaterial::recvData(comm);
    res+= comm.receiveDoubles(minStrain,maxStrain, getDbTagData(),CommMetaData(4));
    res+= comm.receiveBools(Tfailed, Cfailed, getDbTagData(), CommMetaData(5));
    return res;
  }

int XC::MinMaxMaterial::sendSelf(Communicator &comm)
  {
    inicComm(6);
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data.\n";    
    return res;
  }

int XC::MinMaxMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ";  data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;    
  }

void XC::MinMaxMaterial::Print(std::ostream &s, int flag) const
  {
    s << "MinMaxMaterial tag: " << this->getTag() << std::endl;
    s << "\tMaterial: " << this->getMaterial()->getTag() << std::endl;
    s << "\tMin strain: " << minStrain << std::endl;
    s << "\tMax strain: " << maxStrain << std::endl;
  }
