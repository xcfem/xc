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
//FrictionModelHandler.cc

#include "FrictionModelHandler.h"
#include "domain/domain/Domain.h"

// friction models
#include "domain/mesh/element/special/frictionBearing/frictionModel/Coulomb.h"
#include "domain/mesh/element/special/frictionBearing/frictionModel/VelDependent.h"
#include "domain/mesh/element/special/frictionBearing/frictionModel/VelPressureDep.h"
#include "domain/mesh/element/special/frictionBearing/frictionModel/VelNormalFrcDep.h"
#include "domain/mesh/element/special/frictionBearing/frictionModel/VelDepMultiLinear.h"

//! @brief Frees memory.
void XC::FrictionModelHandler::free_mem(void)
  {
    for(iterator i= begin();i!= end();i++)
      delete (*i).second;
    friction_models.erase(begin(),end());
    friction_model_tag= 0;
  }

//! @brief Default constructor.
XC::FrictionModelHandler::FrictionModelHandler(Preprocessor *owr)
  : PrepHandler(owr), friction_model_tag(0)
  { }

//! @brief Copy constructor.
XC::FrictionModelHandler::FrictionModelHandler(const FrictionModelHandler &other) : PrepHandler(other)
  { std::cerr << "This object must no be copied." << std::endl; }

//! @brief Assignment operator.
XC::FrictionModelHandler &XC::FrictionModelHandler::operator=(const FrictionModelHandler &other)
  {
    PrepHandler::operator=(other);
    std::cerr << "This object must not be assigned." << std::endl;
    return *this;
  }

XC::FrictionModelHandler::~FrictionModelHandler(void)
  { free_mem(); }

//! @brief Returns a reference to the friction model container.
const XC::FrictionModelHandler::map_friction_models &XC::FrictionModelHandler::Map(void) const
  { return friction_models; }

//! @brief Returns an iterator which points to principio de la lista.
XC::FrictionModelHandler::const_iterator XC::FrictionModelHandler::begin(void) const
  { return friction_models.begin(); }
//! @brief Returns an iterator apuntando después del final de la lista.
XC::FrictionModelHandler::const_iterator XC::FrictionModelHandler::end(void) const
  { return friction_models.end(); }
//! @brief Returns an iterator which points to principio de la lista.
XC::FrictionModelHandler::iterator XC::FrictionModelHandler::begin(void)
  { return friction_models.begin(); }
//! @brief Returns an iterator apuntando después del final de la lista.
XC::FrictionModelHandler::iterator XC::FrictionModelHandler::end(void)
  { return friction_models.end(); }

//! @brief Returns the name that corresponds to the given tag.
std::string XC::FrictionModelHandler::getName(const int &tag) const
  {
    std::string retval= "";
    const_iterator i= begin();
    for(;i!= end();i++)
      if((*i).second->getTag() == tag)
        {
          retval= (*i).first;
          break;
        }
    return retval;
  }

//! @brief Si encuentra el FrictionModel which name is being passed as parameter returns an iterator which points to mismo.
XC::FrictionModelHandler::const_iterator XC::FrictionModelHandler::find(const std::string &nmb) const
  { return friction_models.find(nmb); }

//! @brief Si encuentra el FrictionModel which name is being passed as parameter returns an iterator which points to mismo.
XC::FrictionModelHandler::iterator XC::FrictionModelHandler::find(const std::string &nmb)
  { return friction_models.find(nmb); }

//! @brief Si encuentra el FrictionModel which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
XC::FrictionModel *XC::FrictionModelHandler::find_ptr(const std::string &nmb)
  {
    XC::FrictionModelHandler::iterator i= find(nmb);
    if(i!= friction_models.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el FrictionModel which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
const XC::FrictionModel *XC::FrictionModelHandler::find_ptr(const std::string &nmb) const
  {
    XC::FrictionModelHandler::const_iterator i= find(nmb);
    if(i!= friction_models.end())
      return (*i).second;
    else
      return nullptr; 
  }

XC::FrictionModel *load_friction_model(int tag, const std::string &cmd)
  {
    XC::FrictionModel *retval= nullptr;
    if(cmd == "Coulomb")
      retval= new XC::Coulomb(tag);
    else if(cmd == "VelDependent")
      retval= new XC::VelDependent(tag);
    else if(cmd == "VelPressureDep")
      retval= new XC::VelPressureDep(tag);
    else if(cmd == "VelNormalFrcDep")
      retval= new XC::VelNormalFrcDep(tag);
    else if(cmd == "VelDepMultiLinear")
      retval= new XC::VelDepMultiLinear(tag);
    else
      std::cerr << "Unknown friction model type: " << cmd << std::endl;
    return retval;
  }

//! @brief Defines a new friction model framework.
XC::FrictionModel *XC::FrictionModelHandler::newFM(const std::string &cmd,const std::string &cod)
  {
    XC::FrictionModel *retval= load_friction_model(friction_model_tag, cmd);
    if(retval)
      {
        retval->set_owner(this);
        if(find(cod)!=end()) // Friction model exists.
          {
	    std::clog << "FrictionModelHandler::newFM; ¡warning! redefining: '"
                      << cod << "'." << std::endl;
            delete friction_models[cod];
          }
        friction_models[cod]= retval;
      }
    friction_model_tag++;
    return retval;

    if(!retval)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; friction model: " << cmd << " unknown." << std::endl;
    return retval;
  }

//! @brief True if friction model named nmb exists.
bool XC::FrictionModelHandler::exists(const std::string &nmb) const
  { return (find(nmb)!=end()); }

//! @brief Returns a reference to the material named as in the parameter.
XC::FrictionModel &XC::FrictionModelHandler::get(const std::string &nmb)
  {
    FrictionModel *retval= find_ptr(nmb);
    assert(retval);
    return *retval;
  }
