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
//AnalysisAggregationMap.cc

#include "AnalysisAggregationMap.h"
#include "domain/domain/Domain.h"
#include "ProcSolu.h"

#include "solution/analysis/ModelWrapper.h"
#include "solution/AnalysisAggregation.h"

#include "boost/any.hpp"


//! @brief Default constructor.
XC::AnalysisAggregationMap::AnalysisAggregationMap(ProcSoluControl *owr)
  : EntCmd(owr) {}

//! @brief Return true if the solution method exists
bool XC::AnalysisAggregationMap::AnalysisAggregationExists(const std::string &cod) const
  { 
    map_solu_method::const_iterator i= solu_methods.find(cod);
    return (i != solu_methods.end());
  }

//! @brief Returns a const pointer to the solution method.
const XC::AnalysisAggregation *XC::AnalysisAggregationMap::getAnalysisAggregation(const std::string &cod) const
  {
    const AnalysisAggregation *retval= nullptr;
    map_solu_method::const_iterator i= solu_methods.find(cod);
    if(i != solu_methods.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Returns a pointer to the solution method.
XC::AnalysisAggregation *XC::AnalysisAggregationMap::getAnalysisAggregation(const std::string &cod)
  {
    AnalysisAggregation *retval= nullptr;
    map_solu_method::iterator i= solu_methods.find(cod);
    if(i != solu_methods.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Creates a new solution method with the code being passed as parameter.
//! @brief Si ya existe returns a pointer al mismo.
XC::AnalysisAggregation &XC::AnalysisAggregationMap::createAnalysisAggregation(const std::string &cod,ModelWrapper *sm)
  {
    AnalysisAggregation *retval= nullptr;
    if(AnalysisAggregationExists(cod))
      retval= getAnalysisAggregation(cod);
    else 
      retval= &(solu_methods[cod]= AnalysisAggregation(nullptr,sm));
    return *retval;
  }

//! @brief Creates a new solution method with the code being passed as parameter.
XC::AnalysisAggregation &XC::AnalysisAggregationMap::newAnalysisAggregation(const std::string &cod_solu_method,const std::string &cod_solu_model)
  {
    ProcSoluControl *sc= dynamic_cast<ProcSoluControl *>(Owner());
    ModelWrapper *mdl= sc->getModelWrapper(cod_solu_model);
    AnalysisAggregation &retval= createAnalysisAggregation(cod_solu_method,mdl);
    return retval;
  }

//! @brief Returns todo a su estado original.
void XC::AnalysisAggregationMap::revertToStart(void)
  {
    for(iterator i= solu_methods.begin();i!=solu_methods.end();i++)
      (*i).second.revertToStart();
  }

//! @brief Clears all.
void XC::AnalysisAggregationMap::clearAll(void)
  { solu_methods.clear(); }


