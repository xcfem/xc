//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//FieldInfo

#include "FieldInfo.h"



//! @brief Constructor.
XC::FieldInfo::FieldInfo(const std::string &nmb)
  : EntConNmb(nmb),nmb_set("total"),sobre_nodos(true), sobre_puntos_gauss(false), componentsType("double"),
    iterationNumber(0), orderNumber(0), time(0)  {}

void XC::FieldInfo::definedOnGaussPoints(void)
  {
    definedOnElements();
    sobre_puntos_gauss= true;
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
