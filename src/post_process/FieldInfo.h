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

#include "xc_utils/src/nucleo/EntConNmb.h"
#include "xc_basic/src/matrices/m_string.h"
#include <boost/python/list.hpp>

namespace XC {
class Set;

//! @defgroup POST_PROCESS Post processing.
//
//! @ingroup POST_PROCESS
//
//! @brief Información sobre un campo definido sobre un
//! conjunto.
class FieldInfo: public EntConNmb
  {
  protected:
    std::string nmb_set; //!< nombre del conjunto sobre el que se define.
    bool sobre_nodos; //!< Verdadero si se define sobre los nodos del conjunto.
    bool sobre_puntos_gauss; //!< Verdadero si se define sobre puntos de Gauss de los elementos.
    std::vector<std::string> componentNames; //!< Nombre de las componentes del campo.
    std::vector<std::string> componentDescriptions; //!< Descripción de las componentes del campo.
    std::vector<std::string> componentUnits; //!< Unidades en las que se expresan las componentes del campo.
    std::string componentsProperty; //!< Propiedad mediante la cual se obtienen los valores de las componentes.
    std::string componentsType; //!< Tipo de las componentes ("int", "double", "string",...)
    int iterationNumber; //!< Número de iteración a la que corresponden los valores.
    int orderNumber; //!< Número de orden.
    double time; //!< Instante.

  public:
    FieldInfo(const std::string &);

    inline bool isDefinedOnNodes(void) const
      { return sobre_nodos; }
    inline void definedOnNodes(void)
      { sobre_nodos= true; }
    inline bool isDefinedOnElements(void) const
      { return !isDefinedOnNodes(); }
    inline void definedOnElements(void)
      { sobre_nodos= false; }
    inline bool isDefinedOnGaussPoints(void) const
      { return sobre_puntos_gauss; }
    void definedOnGaussPoints(void);
    void setSetName(const std::string &nmb)
      { nmb_set= nmb; }
    inline const std::string &getSetName(void) const
      { return nmb_set; }
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
