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
//MapModelWrapper.cc

#include "MapModelWrapper.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "domain/domain/Domain.h"
#include "solution/ProcSolu.h"

#include "solution/analysis/ModelWrapper.h"
#include "xc_utils/src/base/utils_any.h"
#include "boost/any.hpp"


//! @brief Constructor por defecto.
XC::MapModelWrapper::MapModelWrapper(ProcSoluControl *owr)
  : EntCmd(owr) {}

//! @brief Lee un objeto MapModelWrapper desde archivo.
bool XC::MapModelWrapper::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(MapModelWrapper) Procesando comando: " << cmd << std::endl;

    if(cmd == "clearAll")
      {
        status.GetString();
        clearAll();
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

//! @brief Devuelve verdadero si existe el método cuyo nombre
//! se pasa como parámetro.
bool XC::MapModelWrapper::existeModelWrapper(const std::string &cod) const
  { 
    map_solu_model::const_iterator i= solu_models.find(cod);
    return (i != solu_models.end());
  }

//! @brief Devuelve un apuntador al método
//! de solución.
XC::ModelWrapper *XC::MapModelWrapper::getModelWrapper(const std::string &cod)
  {
    ModelWrapper *retval= nullptr;
    iterator i= solu_models.find(cod);
    if(i != solu_models.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Devuelve un apuntador al método
//! de solución.
const XC::ModelWrapper *XC::MapModelWrapper::getModelWrapper(const std::string &cod) const
  {
    const ModelWrapper *retval= nullptr;
    const_iterator i= solu_models.find(cod);
    if(i != solu_models.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Crea un nuevo método de solución con el código que se pasa como parámetro.
//! @brief Si ya existe devuelve un puntero al mismo.
XC::ModelWrapper &XC::MapModelWrapper::creaModelWrapper(const std::string &cod)
  {
    ModelWrapper *retval= nullptr;
    if(existeModelWrapper(cod))
      retval= getModelWrapper(cod);
    else
      {
        ModelWrapper nuevo(nullptr);
        retval= &(solu_models[cod]= nuevo);
      }
    return *retval;
  }

//! @brief Borra todo.
void XC::MapModelWrapper::clearAll(void)
  { solu_models.clear(); }


