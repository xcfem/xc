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
//ProcSoluControl.cc

#include "ProcSoluControl.h"
#include "domain/domain/Domain.h"
#include "ProcSolu.h"

#include "solution/analysis/ModelWrapper.h"
#include "solution/SoluMethod.h"

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

