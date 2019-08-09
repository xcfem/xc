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
//FieldInfo.h

#ifndef FIELDINFO_H
#define FIELDINFO_H

#include "xc_utils/src/kernel/NamedEntity.h"
#include "xc_utils/src/matrices/m_string.h"
#include <boost/python/list.hpp>

namespace XC {
class Set;

//! @defgroup POST_PROCESS Post processing.
//
//! @ingroup POST_PROCESS
//
//! @brief Information about a field defined over a set.
class FieldInfo: public NamedEntity
  {
  protected:
    std::string set_name; //!< name of the set.
    bool on_nodes; //!< True if defined over the nodes of the set.
    bool on_gauss_points; //!< True if defined over Gauss points of the elements.
    std::vector<std::string> componentNames; //!< Name of the field components.
    std::vector<std::string> componentDescriptions; //!< Field components description.
    std::vector<std::string> componentUnits; //!< Units for each component.
    std::string componentsProperty; //!< Property that gets the values of the components.
    std::string componentsType; //!< Component data type ("int", "double", "string",...)
    int iterationNumber; //!< Number of the iteration in which this values were obtained.
    int orderNumber; //!< Order number.
    double time; //!< Time.

  public:
    FieldInfo(const std::string &);

    inline bool isDefinedOnNodes(void) const
      { return on_nodes; }
    inline void definedOnNodes(void)
      { on_nodes= true; }
    inline bool isDefinedOnElements(void) const
      { return !isDefinedOnNodes(); }
    inline void definedOnElements(void)
      { on_nodes= false; }
    inline bool isDefinedOnGaussPoints(void) const
      { return on_gauss_points; }
    void definedOnGaussPoints(void);
    void setSetName(const std::string &nmb)
      { set_name= nmb; }
    inline const std::string &getSetName(void) const
      { return set_name; }
    inline size_t getNumberOfComponents(void) const
      { return componentNames.size(); }
    inline void setComponentsType(const std::string &s)
      { componentsType= s; }
    inline const std::string &getComponentsType(void) const
      { return componentsType; }
    void setComponentNames(const boost::python::list &);
    inline const std::vector<std::string> &getComponentNames(void) const
      { return componentNames; }
    void setComponentDescriptions(const boost::python::list &);
    inline const std::vector<std::string> &getComponentDescriptions(void) const
      { return componentDescriptions; }
    void setComponentUnits(const boost::python::list &);
    inline const std::vector<std::string> &getComponentUnits(void) const
      { return componentUnits; }
    inline void setComponentsProperty(const std::string &s)
      { componentsProperty= s; }
    const std::string &getComponentsProperty(void) const;
    inline void setIterationNumber(const int &in)
      { iterationNumber= in; }
    inline const int &getIterationNumber(void) const
      { return iterationNumber; }
    inline void setOrderNumber(const int &on)
      { orderNumber= on; }
    inline const int &getOrderNumber(void) const
      { return orderNumber; }
    inline void setTime(const double &t)
      { time= t; }
    inline const double &getTime(void) const
      { return time; }
  };
} // end of XC namespace
#endif
