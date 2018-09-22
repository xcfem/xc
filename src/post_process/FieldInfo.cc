//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//FieldInfo

#include "FieldInfo.h"



//! @brief Constructor.
XC::FieldInfo::FieldInfo(const std::string &nmb)
  : NamedEntity(nmb),nmb_set("total"),on_nodes(true), on_gauss_points(false), componentsType("double"),
    iterationNumber(0), orderNumber(0), time(0)  {}

void XC::FieldInfo::definedOnGaussPoints(void)
  {
    definedOnElements();
    on_gauss_points= true;
  }

//! @brief Assigns field component names.
void XC::FieldInfo::setComponentNames(const boost::python::list &l)
  {
    const size_t sz= len(l);
    for(size_t i=0; i<sz; i++)
      componentNames.push_back(boost::python::extract<std::string>(l[i]));
  }

//! @brief Assigns field component descriptions.
void XC::FieldInfo::setComponentDescriptions(const boost::python::list &l)
  {
    const size_t sz= len(l);
    for(size_t i=0; i<sz; i++)
      componentDescriptions.push_back(boost::python::extract<std::string>(l[i]));
  }

//! @brief Assigns field component units.
void XC::FieldInfo::setComponentUnits(const boost::python::list &l)
  {
    const size_t sz= len(l);
    for(size_t i=0; i<sz; i++)
      componentUnits.push_back(boost::python::extract<std::string>(l[i]));
  }

const std::string &XC::FieldInfo::getComponentsProperty(void) const
  { return componentsProperty; }
