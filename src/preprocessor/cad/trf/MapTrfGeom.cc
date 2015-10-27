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
//MapTrfGeom.cc

#include "MapTrfGeom.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/cad/trf/TrfGeom.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "preprocessor/set_mgmt/Set.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "Rotacion.h"
#include "Reflexion.h"
#include "Escalado.h"
#include "Traslacion.h"

//! @brief Constructor.
XC::MapTrfGeom::MapTrfGeom(Cad *cad)
  : MapCadMember<TrfGeom>(cad) {}

//! @brief Defines a new transformation.
XC::TrfGeom *XC::MapTrfGeom::newTransformation(const std::string &type)
  {
    TrfGeom *retval= nullptr;
    if(type == "rotation") //Crea una nueva rotación.
      retval= Nueva<Rotacion>();
    else if(type == "reflection") //Crea una nueva reflexión.
      retval= Nueva<Reflexion>();
    else if(type == "scaling") //Crea un nuevo escalado.
      retval= Nueva<Escalado>();
    else if(type == "translation") //Crea una nueva traslacion.
      retval= Nueva<Traslacion>();
    else
      std::cerr << "Transformation type: '" << type
                << "' unknown." << std::endl;
    return retval;
  }

//! @brief Lee un objeto MapTrfGeom desde el archivo de entrada.
bool XC::MapTrfGeom::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string str_err= "(MapTrfGeom) Procesando comando: " + cmd;
    if(verborrea>2)
      std::clog << str_err << std::endl;

    if(cmd == "tag_trf_geom")
      {
        setTag(interpretaSize_t(status.GetString()));
        return true;
      }
    else if(cmd == "rot") //Crea una nueva rotación.
      {
        Nueva<Rotacion>(status);
        return true;
      }
    else if(cmd == "reflex") //Crea una nueva reflexión.
      {
        Nueva<Reflexion>(status);
        return true;
      }
    else if(cmd == "escal") //Crea un nuevo escalado.
      {
        Nueva<Escalado>(status);
        return true;
      }
    else if(cmd == "transl") //Crea una nueva traslacion.
      {
        Nueva<Traslacion>(status);
        return true;
      }
    else
      return MapCadMember<TrfGeom>::procesa_comando(status);
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::MapTrfGeom::GetProp(const std::string &cod) const
  {
    if(cod=="num_trfs")
      {
        tmp_gp_szt= size();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return MapCadMember<TrfGeom>::GetProp(cod);
  }
