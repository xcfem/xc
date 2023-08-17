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
// $Date: 2010-09-11 00:50:53 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/HalfDiagramMaterial.cpp,v $

// Created: Aug 2022
//
// Description: This file contains the class definition for 
// HalfDiagramMaterial.  HalfDiagramMaterial wraps a UniaxialMaterial
// and removes the positive or negative part of its diagram
// (see derived classes).

#include "HalfDiagramMaterial.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::HalfDiagramMaterial::HalfDiagramMaterial(int tag, int classTag, UniaxialMaterial &material)
  :EncapsulatedUniaxialMaterial(tag, classTag, material)
  {}

XC::HalfDiagramMaterial::HalfDiagramMaterial(int tag, int classTag)
  :EncapsulatedUniaxialMaterial(tag, classTag)
  {}

int XC::HalfDiagramMaterial::setTrialStrain(double strain, double strainRate)
  {
    return this->getMaterial()->setTrialStrain(strain, strainRate);
  }


// int XC::HalfDiagramMaterial::setTrialStrain(double strain, double temp, double strainRate)
//   {
//     return this->getMaterial()->setTrialStrain(strain, temp, strainRate);
//   }

int XC::HalfDiagramMaterial::revertToLastCommit(void)
  { return this->getMaterial()->revertToLastCommit(); }

int XC::HalfDiagramMaterial::revertToStart(void)
  { return this->getMaterial()->revertToStart(); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::HalfDiagramMaterial::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }


int XC::HalfDiagramMaterial::sendSelf(Communicator &comm)
  {
    inicComm(4);
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data.\n";    
    return res;
  }

int XC::HalfDiagramMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ";  data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;    
  }

void XC::HalfDiagramMaterial::Print(std::ostream &s, int flag) const
  {
    s << getClassName() << ", tag: " << this->getTag() << std::endl
      << "  material: " << this->getMaterial()->getTag() << std::endl;
  }

int XC::HalfDiagramMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      retval= tmp->setParameter(argv, param);
    return retval;
  }

int XC::HalfDiagramMaterial::updateParameter(int parameterID, Information &info)
  { return 0; }

double XC::HalfDiagramMaterial::getStrainSensitivity(int gradIndex)
  { return this->getMaterial()->getStrainSensitivity(gradIndex); }

double XC::HalfDiagramMaterial::getInitialTangentSensitivity(int gradIndex)
  { return this->getMaterial()->getInitialTangentSensitivity(gradIndex); }

double XC::HalfDiagramMaterial::getRhoSensitivity(int gradIndex)
  { return this->getMaterial()->getRhoSensitivity(gradIndex); }


