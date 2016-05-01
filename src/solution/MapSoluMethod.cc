//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
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
//MapSoluMethod.cc

#include "MapSoluMethod.h"
#include "domain/domain/Domain.h"
#include "ProcSolu.h"

#include "solution/analysis/ModelWrapper.h"
#include "solution/SoluMethod.h"

#include "boost/any.hpp"


//! @brief Constructor por defecto.
XC::MapSoluMethod::MapSoluMethod(ProcSoluControl *owr)
  : EntCmd(owr) {}

//! @brief Devuelve verdadero si existe el método cuyo nombre
//! se pasa como parámetro.
bool XC::MapSoluMethod::existeSoluMethod(const std::string &cod) const
  { 
    map_solu_method::const_iterator i= solu_methods.find(cod);
    return (i != solu_methods.end());
  }

//! @brief Devuelve un apuntador al método
//! de solución.
const XC::SoluMethod *XC::MapSoluMethod::getSoluMethod(const std::string &cod) const
  {
    const SoluMethod *retval= nullptr;
    map_solu_method::const_iterator i= solu_methods.find(cod);
    if(i != solu_methods.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Devuelve un apuntador al método
//! de solución.
XC::SoluMethod *XC::MapSoluMethod::getSoluMethod(const std::string &cod)
  {
    SoluMethod *retval= nullptr;
    map_solu_method::iterator i= solu_methods.find(cod);
    if(i != solu_methods.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Crea un nuevo método de solución con el código que se pasa como parámetro.
//! @brief Si ya existe devuelve un puntero al mismo.
XC::SoluMethod &XC::MapSoluMethod::creaSoluMethod(const std::string &cod,ModelWrapper *sm)
  {
    SoluMethod *retval= nullptr;
    if(existeSoluMethod(cod))
      retval= getSoluMethod(cod);
    else 
      retval= &(solu_methods[cod]= SoluMethod(nullptr,sm));
    return *retval;
  }

//! @brief Crea un nuevo método de solución con el código que se pasa como parámetro.
XC::SoluMethod &XC::MapSoluMethod::newSoluMethod(const std::string &cod_solu_method,const std::string &cod_solu_model)
  {
    ProcSoluControl *sc= dynamic_cast<ProcSoluControl *>(Owner());
    ModelWrapper *mdl= sc->getModelWrapper(cod_solu_model);
    SoluMethod &retval= creaSoluMethod(cod_solu_method,mdl);
    return retval;
  }

//! @brief Devuelve todo a su estado original.
void XC::MapSoluMethod::revertToStart(void)
  {
    for(iterator i= solu_methods.begin();i!=solu_methods.end();i++)
      (*i).second.revertToStart();
  }

//! @brief Borra todo.
void XC::MapSoluMethod::clearAll(void)
  { solu_methods.clear(); }


