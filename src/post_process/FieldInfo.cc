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
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/utils_any.h"

//! @brief Constructor.
XC::FieldInfo::FieldInfo(const std::string &nmb)
  : EntConNmb(nmb),nmb_set("total"),sobre_nodos(true), sobre_puntos_gauss(false), componentsType("double"),
    iterationNumber(0), orderNumber(0), time(0)  {}

void XC::FieldInfo::definedOnGaussPoints(void)
  {
    definedOnElements();
    sobre_puntos_gauss= true;
  }

//! @brief Lectura del objeto desde archivo.
bool XC::FieldInfo::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(FieldInfo) Procesando comando: " << cmd << std::endl;

    if(cmd == "setSet") //Conjunto sobre el que se define.
      {
        nmb_set= interpretaString(status.GetBloque());
        return true;
      }
    else if(cmd == "onNodes")
      {
	status.GetString();
        sobre_nodos= false;
        return true;
      }
    else if(cmd == "onElements")
      {
	status.GetString();
        sobre_nodos= false;
        return true;
      }
    else if(cmd == "onGaussPoints")
      {
	status.GetString();
        sobre_nodos= false;
        sobre_puntos_gauss= true;
        return true;
      }
    else if(cmd == "setComponentType")
      {
	componentsType= interpretaString(status.GetString());
        return true;
      }
    else if(cmd == "setComponentNames")
      {
	componentNames= convert_to_vector_string(interpretaVectorAny(status.GetString()));
        return true;
      }
    else if(cmd == "setComponentDescriptions")
      {
	componentDescriptions= convert_to_vector_string(interpretaVectorAny(status.GetString()));
        return true;
      }
    else if(cmd == "setComponentUnits")
      {
	componentUnits= convert_to_vector_string(interpretaVectorAny(status.GetString()));
        return true;
      }
    else if(cmd == "setComponentsProperty")
      {
	componentsProperty= interpretaString(status.GetString());
        return true;
      }
    else if(cmd == "setIterationNumber")
      {
	iterationNumber= interpretaInt(status.GetString());
        return true;
      }
    else if(cmd == "setOrderNumber")
      {
	orderNumber= interpretaInt(status.GetString());
        return true;
      }
    else if(cmd == "setTime")
      {
	time= interpretaDouble(status.GetString());
        return true;
      }
    else
      return EntConNmb::procesa_comando(status);
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

any_const_ptr XC::FieldInfo::GetProp(const std::string &cod) const
  {
    return EntConNmb::GetProp(cod);
  }
