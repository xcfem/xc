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
//ProcSoluControl.cc

#include "ProcSoluControl.h"
#include "domain/domain/Domain.h"
#include "ProcSolu.h"

#include "solution/analysis/ModelWrapper.h"
#include "solution/SoluMethod.h"
#include "xc_utils/src/base/utils_any.h"
#include "boost/any.hpp"


//! @brief Constructor por defecto.
XC::ProcSoluControl::ProcSoluControl(ProcSolu *owr)
  : EntCmd(owr), solu_models(this), solu_methods(this) {}


XC::ProcSolu *XC::ProcSoluControl::getProcSolu(void)
  { return dynamic_cast<ProcSolu *>(Owner()); }

const XC::ProcSolu *XC::ProcSoluControl::getProcSolu(void) const
  { return dynamic_cast<const ProcSolu *>(Owner()); }

//! @brief Devuelve un apuntador al dominio sobre el que operan
//! los agloritmos de solución.
XC::Domain *XC::ProcSoluControl::getDomain(void)
  {
    ProcSolu *ps= getProcSolu();
    assert(ps); 
    return ps->getDomainPtr();
  }

//! @brief Devuelve un apuntador al dominio sobre el que operan
//! los agloritmos de solución.
const XC::Domain *XC::ProcSoluControl::getDomain(void) const
  {
    const ProcSolu *ps= getProcSolu();
    assert(ps); 
    return ps->getDomainPtr();
  }

//! @brief Devuelve un puntero al integrador.
XC::Integrator *XC::ProcSoluControl::getIntegratorPtr(void)
  {
    ProcSolu *ps= getProcSolu();
    assert(ps);
    return ps->getIntegratorPtr();
  }

//! @brief Devuelve un puntero al integrador.
const XC::Integrator *XC::ProcSoluControl::getIntegratorPtr(void) const
  {
    const ProcSolu *ps= getProcSolu();
    assert(ps);
    return ps->getIntegratorPtr();
  }

//! @bried Devuelve una referencia al contenedor de «ModelWrapper».
XC::MapModelWrapper &XC::ProcSoluControl::getModelWrapperContainer(void)
  { return solu_models; }

//! @bried Devuelve una referencia al contenedor de procedimientos de solución.
XC::MapSoluMethod &XC::ProcSoluControl::getSoluMethodContainer(void)
  { return solu_methods; }

//! @brief Devuelve un apuntador al procedimiento
//! de solución cuyo nombre se pasa como parámetro.
const XC::ModelWrapper *XC::ProcSoluControl::getModelWrapper(const std::string &cod) const
  { return solu_models.getModelWrapper(cod); }

//! @brief Devuelve un apuntador al procedimiento
//! de solución.
XC::ModelWrapper *XC::ProcSoluControl::getModelWrapper(const std::string &cod)
  { return solu_models.getModelWrapper(cod); }

//! @brief Devuelve un apuntador al método
//! de solución.
const XC::SoluMethod *XC::ProcSoluControl::getSoluMethod(const std::string &cod) const
  { return solu_methods.getSoluMethod(cod); }

//! @brief Devuelve un apuntador al método
//! de solución.
XC::SoluMethod *XC::ProcSoluControl::getSoluMethod(const std::string &cod)
  { return solu_methods.getSoluMethod(cod); }

//! @brief Devuelve todo a su estado original.
void XC::ProcSoluControl::revertToStart(void)
  {
    getDomain()->revertToStart();
    solu_methods.revertToStart();
  }

//! @brief Borra todo.
void XC::ProcSoluControl::clearAll(void)
  {
    solu_models.clearAll();
    solu_methods.clearAll();
  }

