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
#include "xc_utils/src/base/CmdStatus.h"
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

//! @brief Lee un objeto ProcSoluControl desde archivo.
bool XC::ProcSoluControl::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ProcSoluControl) Procesando comando: " << cmd << std::endl;
    //Algoritmo de solución.
    if(cmd == "solu_model")
      {
        const CmdParser &parser= status.Parser();
        std::string cod_solu_model= "nil";
        if(parser.TieneIndices())
          {
            const std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
              cod_solu_model= convert_to_string(fnc_indices[0]);
          }
        ModelWrapper &tmp= solu_models.creaModelWrapper(cod_solu_model);
        tmp.LeeCmd(status);
        return true;
      }
    else if(cmd == "solu_method")
      {
        const CmdParser &parser= status.Parser();
        if(!solu_models.empty())
          {
            std::string cod_solu_model= solu_models.begin()->first;
            std::string cod_solu_method= "nil";
            std::deque<boost::any> fnc_indices;
            if(parser.TieneIndices())
              {
                fnc_indices= status.Parser().SeparaIndices(this);
                if(fnc_indices.size()>0)
                  cod_solu_method= convert_to_string(fnc_indices[0]);
                if(fnc_indices.size()>1)
                  cod_solu_model= convert_to_string(fnc_indices[1]);
              }
            ModelWrapper *mdl= getModelWrapper(cod_solu_model);
            SoluMethod &tmp= solu_methods.creaSoluMethod(cod_solu_method,mdl);
            tmp.LeeCmd(status);
          }
        else
          {
	    std::cerr << "Es necesario definir algún modelo de solución." << std::endl;
            status.GetString();
          }
        return true;
      }
    else if(cmd == "clearAll")
      {
        status.GetString();
        clearAll();
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

//! @brief Borra todo.
void XC::ProcSoluControl::clearAll(void)
  {
    solu_models.clearAll();
    solu_methods.clearAll();
  }

