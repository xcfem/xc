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
//MapSoluMethod.cc

#include "MapSoluMethod.h"
#include "domain/domain/Domain.h"
#include "ProcSolu.h"

#include "solution/analysis/ModelWrapper.h"
#include "solution/SoluMethod.h"

#include "boost/any.hpp"


//! @brief Default constructor.
XC::MapSoluMethod::MapSoluMethod(ProcSoluControl *owr)
  : EntCmd(owr) {}

//! @brief Returns true ifexiste el método cuyo nombre
//! se pasa como parámetro.
bool XC::MapSoluMethod::existeSoluMethod(const std::string &cod) const
  { 
    map_solu_method::const_iterator i= solu_methods.find(cod);
    return (i != solu_methods.end());
  }

//! @brief Returns a const pointer to the solution method.
const XC::SoluMethod *XC::MapSoluMethod::getSoluMethod(const std::string &cod) const
  {
    const SoluMethod *retval= nullptr;
    map_solu_method::const_iterator i= solu_methods.find(cod);
    if(i != solu_methods.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Returns a pointer to the solution method.
XC::SoluMethod *XC::MapSoluMethod::getSoluMethod(const std::string &cod)
  {
    SoluMethod *retval= nullptr;
    map_solu_method::iterator i= solu_methods.find(cod);
    if(i != solu_methods.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Crea un nuevo solution method con el código being passed as parameter.
//! @brief Si ya existe returns a pointer al mismo.
XC::SoluMethod &XC::MapSoluMethod::creaSoluMethod(const std::string &cod,ModelWrapper *sm)
  {
    SoluMethod *retval= nullptr;
    if(existeSoluMethod(cod))
      retval= getSoluMethod(cod);
    else 
      retval= &(solu_methods[cod]= SoluMethod(nullptr,sm));
    return *retval;
  }

//! @brief Crea un nuevo solution method con el código being passed as parameter.
XC::SoluMethod &XC::MapSoluMethod::newSoluMethod(const std::string &cod_solu_method,const std::string &cod_solu_model)
  {
    ProcSoluControl *sc= dynamic_cast<ProcSoluControl *>(Owner());
    ModelWrapper *mdl= sc->getModelWrapper(cod_solu_model);
    SoluMethod &retval= creaSoluMethod(cod_solu_method,mdl);
    return retval;
  }

//! @brief Returns todo a su estado original.
void XC::MapSoluMethod::revertToStart(void)
  {
    for(iterator i= solu_methods.begin();i!=solu_methods.end();i++)
      (*i).second.revertToStart();
  }

//! @brief Borra todo.
void XC::MapSoluMethod::clearAll(void)
  { solu_methods.clear(); }


