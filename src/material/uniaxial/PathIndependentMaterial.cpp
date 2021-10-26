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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/04/02 22:02:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/PathIndependentMaterial.cpp,v $

// Written: MHS
// Created: Aug 2000
//
// Description: This file contains the class definition for 
// PathIndependentMaterial.  PathIndependentMaterial uses a XC::UniaxialMaterial
// object to represent a path-independent uniaxial material.  Since
// it is path-independent, no state information is stored by
// PathIndependentMaterial.

#include <cstdlib>
#include <material/uniaxial/PathIndependentMaterial.h>
#include <utility/matrix/ID.h>

//! @brief Constructor.
XC::PathIndependentMaterial::PathIndependentMaterial(int tag, UniaxialMaterial &material)
:EncapsulatedMaterial(tag,MAT_TAG_PathIndependent)
  {
    theMaterial = material.getCopy();
    if(theMaterial == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; failed to get copy of material\n";
        //exit(-1);
      }
  }

XC::PathIndependentMaterial::PathIndependentMaterial(int tag)
  : EncapsulatedMaterial(tag,MAT_TAG_PathIndependent) {}

XC::PathIndependentMaterial::PathIndependentMaterial(void)
  :EncapsulatedMaterial(0,MAT_TAG_PathIndependent) {}

int XC::PathIndependentMaterial::setTrialStrain(double strain, double strainRate)
  {
    if(theMaterial)
      return theMaterial->setTrialStrain(strain, strainRate);
    else
      return -1;
  }

//! @brief Return the material strain.
double XC::PathIndependentMaterial::getStrain(void)
  {
    if (theMaterial)
      return theMaterial->getStrain();
    else
      return 0.0;
  }

double XC::PathIndependentMaterial::getStrainRate(void)
  {
    if (theMaterial)
      return theMaterial->getStrainRate();
    else
      return 0.0;
  }

//! @brief Return the material stress.
double XC::PathIndependentMaterial::getStress(void) const
  {
    if(theMaterial)
      return theMaterial->getStress();
    else
      return 0.0;
  }

//! @brief Return the material tangent stiffness.
double XC::PathIndependentMaterial::getTangent(void) const
  {
    if(theMaterial)
      return theMaterial->getTangent();
    else
      return 0.0;
  }

double XC::PathIndependentMaterial::getDampTangent(void) const
  {
    if(theMaterial)
      return theMaterial->getDampTangent();
    else
      return 0.0;
  }

//! @brief Return the material initial stiffness.
double XC::PathIndependentMaterial::getInitialTangent(void) const
  {
    if(theMaterial)
      return theMaterial->getInitialTangent();
    else
      return 0.0;
  }

//! @brief Commit the state of the material.
int XC::PathIndependentMaterial::commitState(void)
  { return 0; }

int XC::PathIndependentMaterial::revertToLastCommit(void)
  { return 0; }

//! @brief Revert the material to its initial state.
int XC::PathIndependentMaterial::revertToStart(void)
  {
    int retval= EncapsulatedMaterial::revertToStart();
    retval+= theMaterial->revertToStart();
    return retval;
  }

XC::UniaxialMaterial *XC::PathIndependentMaterial::getCopy(void) const
  { return new PathIndependentMaterial(*this); }


int XC::PathIndependentMaterial::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(4); 
    int res= EncapsulatedMaterial::sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send the ID.\n";
    return res;

  }

int XC::PathIndependentMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to get the ID\n";
    else
      res+= EncapsulatedMaterial::recvData(comm);
    return res;
  }

void XC::PathIndependentMaterial::Print(std::ostream &s, int flag) const
  {
    s << "PathIndependentMaterial tag: " << this->getTag() << std::endl;
    s << "\tmaterial: " << theMaterial->getTag() << std::endl;
  }
