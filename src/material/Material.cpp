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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/25 23:33:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/Material.cpp,v $
                                                                        
                                                                        
// File: ~/material/Material.C
//
// Written: fmk 
// Created: 05/98
// Revision: A
//
// Description: This file contains the class implementation for MaterialModel.
//
// What: "@(#) MaterialModel.C, revA"

#include "Material.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/ID.h"
#include "domain/domain/Domain.h"
#include "utility/utils/misc_utils/colormod.h"

//! @param matName: name of the material.
const XC::Material *XC::Material::getMaterialByName(const std::string &matName) const
  {
    const Material *retval= nullptr; 
    const MaterialHandler *material_handler= getMaterialHandler();
    if(material_handler)
      retval= material_handler->find_ptr(matName);
    else
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to preprocessor."
		  << Color::def << std::endl;
      }
    if(!retval)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__ << "; "
		<< "material identified by: '" << matName
		<< "' not found.\n";      
    return retval;
  }

//! @brief Constructor.
//!
//! To construct a material whose unique identifier among materials in the
//! domain is given by \p tag, and whose class identifier is given
//! by \p classTag. These integers are passed to the TaggedObject and
//! MovableObject class constructors.
XC::Material::Material(int tag, int classTag)
  :TaggedObject(tag), MovableObject(classTag) {}

//! @brief Return true if both objects are equal.
bool XC::Material::isEqual(const Material &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
	retval= (typeid(*this) == typeid(other));
	if(retval)
	  {
	    retval= TaggedObject::isEqual(other);
	  }
      }
    return retval;
  }

//! @brief Returns (if possible) a pointer to the material handler (owner).
const XC::MaterialHandler *XC::Material::getMaterialHandler(void) const
  {
    const MaterialHandler *retval= dynamic_cast<const MaterialHandler *>(Owner());
    if(!retval)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; material handler not defined."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Returns a pointer to the material handler (if possible).
XC::MaterialHandler *XC::Material::getMaterialHandler(void)
  {
    MaterialHandler *retval= dynamic_cast<MaterialHandler *>(Owner());
    if(!retval)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; material handler not defined."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Returns (if possible) a pointer to the material handler (owner).
const XC::Domain *XC::Material::getDomain(void) const
  {
    const Domain *retval= nullptr;
    const MaterialHandler *mHandler= this->getMaterialHandler();
    if(mHandler)
      retval= mHandler->getDomain();
    if(!retval)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; no connection with problem domain."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Returns a pointer to the material handler (if possible).
XC::Domain *XC::Material::getDomain(void)
  {
    Domain *retval= nullptr;
    MaterialHandler *mHandler= this->getMaterialHandler();
    if(mHandler)
      retval= mHandler->getDomain();
    if(!retval)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; no connection with problem domain."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Returns the name of the material.
std::string XC::Material::getName(void) const
  {
    std::string retval;
    const MaterialHandler *mhandler= getMaterialHandler();
    if(mhandler)
      retval= mhandler->getName(getTag());
    return retval;
  }

int XC::Material::setVariable(const std::string &argv)
  { return -1; }

int XC::Material::getVariable(int variableID, double &info)
  { return -1; }

int XC::Material::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }

int XC::Material::updateParameter(int responseID, Information &eleInformation)
  { return -1; }

XC::Response* XC::Material::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  { return nullptr; }

//! @brief Returns material response.
int XC::Material::getResponse(int responseID, Information &info)
  { return -1; }

//! @brief Returns material response.
//! @param cod: name of the requested value.
//! @param silent: if true don't complaint about non-existen property.
XC::Matrix XC::Material::getValues(const std::string &cod, bool silent) const
  {
    Matrix retval;
    if(cod == "stress" || cod == "stresses")
      {
	const Vector stress(getGeneralizedStress());
	const size_t sz= stress.Size();
	retval.resize(1,sz);
	retval.putRow(0,stress);
      }
    else if(cod == "strain" || cod == "strains")
      {
	const Vector strain(getGeneralizedStrain());
	const size_t sz= strain.Size();
	retval.resize(1,sz);
	retval.putRow(0,strain);
      }
    else
      if(!silent)
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	 	  << "; property: " << cod
	          << " not found."
		  << Color::def << std::endl;
    return retval;
  }

//! @brief Update state variables for the material
//! according to the new values of the parameters.
void XC::Material::update(void)
   {return;}

//! @brief Increments generalized strain
//! @param incS: strain increment.
void XC::Material::incrementInitialGeneralizedStrain(const Vector &incS)
  {
    Vector initStrain= getInitialGeneralizedStrain();
    initStrain+= incS;
    setInitialGeneralizedStrain(initStrain);
  }

//! @brief zeroes initial generalized strain
void XC::Material::zeroInitialGeneralizedStrain(void)
  {
    Vector tmp= getInitialGeneralizedStrain();
    tmp.Zero();
    setInitialGeneralizedStrain(tmp);
  }


//! @brief Sends a pointer to material through the communicator being passed as parameter.
//! @param posClassTag: Index of the material class identifier.
//! @param posDbTag: Index of the dbTag.
int XC::sendMaterialPtr(Material *ptr,DbTagData &dt,Communicator &comm,const BrokedPtrCommMetaData &md)
  {
    int res= 0;
    if(ptr)
      {
        dt.setDbTagDataPos(md.getPosClassTag(),ptr->getClassTag());
        res= comm.sendMovable(*ptr,dt,md);
      }
    if(res < 0)
      std::cerr << Color::red << __FUNCTION__
                << "; failed to send material"
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives a pointer to material through the communicator being passed as parameter.
//! @param posClassTag: Index of the material class identifier.
//! @param posDbTag: Index of the dbTag.
XC::Material *XC::receiveMaterialPtr(Material* ptr,DbTagData &dt,const Communicator &comm,const BrokedPtrCommMetaData &md)
  {
    Material *retval= nullptr;
    const int matClass= dt.getDbTagDataPos(md.getPosClassTag());
    if(ptr && (ptr->getClassTag() == matClass))
      retval= ptr;
    else 
      {

        // check if we have a material object already &
        // if we do if of right type
        if(ptr) // if old one .. delete it
          delete ptr;
        ptr= nullptr;
        retval= comm.getBrokedMaterial(ptr,dt,md);
      }
    return retval;
  }


//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::Material::getPyDict(void) const
  {
    boost::python::dict retval= TaggedObject::getPyDict();
    retval["name"]= this->getName();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::Material::setPyDict(const boost::python::dict &d)
  {
    TaggedObject::setPyDict(d);
    // Check that it has been created with the same name.
    const std::string tmp= boost::python::extract<std::string>(d["name"]);
    if(tmp!=this->getName())
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; name of the material has changed from: '"
		  << tmp << "' to: '" << this->getName()
		  << "'"
		  << Color::def << std::endl;
      }
  }
