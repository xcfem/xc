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
                                                                        
// $Revision: 1.10 $
// $Date: 2004/11/24 00:48:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ConnectedMaterial.cpp,v $
                                                                        
                                                                        
// File: ~/material/ConnectedModel.C
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for 
// ConnectedModel. ConnectedModel is an aggregation
// of XC::UniaxialMaterial objects all considered acting in Connected.
//
// What: "@(#) ConnectedModel.C, revA"

#include "ConnectedMaterial.h"
#include "utility/matrix/ID.h"

XC::ConnectedMaterial::ConnectedMaterial(int tag, int classTag,const DqUniaxialMaterial &theMaterialModels)
  :UniaxialMaterial(tag,classTag), theModels(theMaterialModels)
  { theModels.set_owner(this); }


//! @brief Default constructor.
XC::ConnectedMaterial::ConnectedMaterial(int tag, int classTag)
  :UniaxialMaterial(tag,classTag), theModels()
  { theModels.set_owner(this); }

XC::ConnectedMaterial::ConnectedMaterial(const ConnectedMaterial &other)
  :UniaxialMaterial(other), theModels(other.theModels)
  { theModels.set_owner(this); }

XC::ConnectedMaterial &XC::ConnectedMaterial::operator=(const ConnectedMaterial &other)
  {
    UniaxialMaterial::operator=(other);
    theModels= other.theModels;
    theModels.set_owner(this);
    return *this;
  }

//! @brief Append a connected material.
//!
//! @param dir: direction 
void XC::ConnectedMaterial::appendMaterial(const std::string &matName)
  {
    const Material *ptr_mat= getMaterialByName(matName);
    if(ptr_mat)
      {
	const UniaxialMaterial *tmp= dynamic_cast<const UniaxialMaterial *>(ptr_mat);
	if(tmp)
	  theModels.push_back(tmp);
	else
	  std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
		    << "material identified by: '" << matName
		    << "' is not an uniaxial material.\n";
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
		<< "material identified by: '" << matName
		<< "' not found." << std::endl;
  }

//! @brief Set the connected materials.
void XC::ConnectedMaterial::setMaterials(const boost::python::list &materialsToConnect)
  {
    const size_t sz= len(materialsToConnect);
    for(size_t i=0; i<sz; i++)
      {
	const std::string matName= boost::python::extract<std::string>(materialsToConnect[i]);
	appendMaterial(matName);
      }
  }

//! @brief Return the number of connected materials.
size_t XC::ConnectedMaterial::getNumMaterials(void) const
  { return theModels.size(); }

//! @brief Return a reference to the material container.
const XC::DqUniaxialMaterial &XC::ConnectedMaterial::getMaterials(void) const
  { return theModels; }

//! @brief Return a reference to the material container.
XC::DqUniaxialMaterial &XC::ConnectedMaterial::getMaterials(void)
  { return theModels; }

//! @brief Send its members through the communicator argument.
int XC::ConnectedMaterial::sendData(Communicator &comm)
  {
    int res= UniaxialMaterial::sendData(comm);
    res+= comm.sendMovable(theModels,getDbTagData(),CommMetaData(2));
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send.\n";
    return res;
  }

//! @brief Receives its members through the communicator argument.
int XC::ConnectedMaterial::recvData(const Communicator &comm)
  {
    int res= UniaxialMaterial::recvData(comm);
    res+= comm.receiveMovable(theModels,getDbTagData(),CommMetaData(2));
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive.\n";
    return res;
  }

int XC::ConnectedMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return theModels.setParameter(argv, param); }

double XC::ConnectedMaterial::getStressSensitivity(int gradIndex, bool conditional)
  { return theModels.getStressSensitivity(gradIndex, conditional); }

double XC::ConnectedMaterial::getTangentSensitivity(int gradIndex)
  { return theModels.getTangentSensitivity(gradIndex); }

double XC::ConnectedMaterial::getInitialTangentSensitivity(int gradIndex)
  { return theModels.getInitialTangentSensitivity(gradIndex); }

double XC::ConnectedMaterial::getDampTangentSensitivity(int gradIndex)
  { return theModels.getDampTangentSensitivity(gradIndex); }

double XC::ConnectedMaterial::getRhoSensitivity(int gradIndex)
  { return theModels.getRhoSensitivity(gradIndex); }

int XC::ConnectedMaterial::commitSensitivity(double dedh, int gradIndex, int numGrads)
  { return theModels.commitSensitivity(dedh, gradIndex, numGrads); }





