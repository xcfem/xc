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
//MapModelWrapper.cc

#include "MapModelWrapper.h"
#include "domain/domain/Domain.h"
#include "solution/SolutionProcedure.h"

#include "solution/analysis/ModelWrapper.h"

#include "boost/any.hpp"


//! @brief Default constructor.
XC::MapModelWrapper::MapModelWrapper(SolutionProcedureControl *owr)
  : CommandEntity(owr) {}

//! @brief Return true if the method whose name is
//! passed as parameter exists.
bool XC::MapModelWrapper::existeModelWrapper(const std::string &cod) const
  { 
    map_model_wrapper::const_iterator i= solu_models.find(cod);
    return (i != solu_models.end());
  }

//! @brief Return a pointer to the solution method.
XC::ModelWrapper *XC::MapModelWrapper::getModelWrapper(const std::string &cod)
  {
    ModelWrapper *retval= nullptr;
    iterator i= solu_models.find(cod);
    if(i != solu_models.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Return a const pointer to the solution method.
const XC::ModelWrapper *XC::MapModelWrapper::getModelWrapper(const std::string &cod) const
  {
    const ModelWrapper *retval= nullptr;
    const_iterator i= solu_models.find(cod);
    if(i != solu_models.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Return the name of a model wrapper.
const std::string &XC::MapModelWrapper::getModelWrapperName(const ModelWrapper *mw) const
  {
    static std::string retval;
    retval= "";
    const_iterator i= begin();
    for(; i!= end(); i++)
      {
	const ModelWrapper *tmp= &((*i).second);
	if(tmp==mw) // found it.
	  {
	    retval= (*i).first;
	    break;
	  }
      }
    if(i==end())
      {
	retval= "not_found";
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; model wrapper not found."
	          << std::endl;
      }
    return retval;
  }

//! @brief Creates a new solution method with the identifier being passed as parameter (if already exists, it returns a pointer to it). 
XC::ModelWrapper &XC::MapModelWrapper::creaModelWrapper(const std::string &cod)
  {
    ModelWrapper *retval= nullptr;
    if(existeModelWrapper(cod))
      retval= getModelWrapper(cod);
    else
      {
        ModelWrapper newWrapper(nullptr);
        retval= &(solu_models[cod]= newWrapper);
      }
    return *retval;
  }

//! @brief Clears all.
void XC::MapModelWrapper::clearAll(void)
  { solu_models.clear(); }


