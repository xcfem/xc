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
//MapCuerpos.cc

#include "MapCuerpos.h"
#include "modelador/Modelador.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "modelador/cad/entidades/Pnt.h"
#include "modelador/cad/entidades/Body.h"
#include "modelador/cad/entidades/Block.h"
#include "modelador/cad/entidades/SupCuadrilatera.h"
#include "modelador/set_mgmt/Set.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::MapCuerpos::MapCuerpos(Cad *cad)
  : MapEnt<Body>(cad) {}

//! @brief Lee un objeto Body desde el archivo de entrada.
bool XC::MapCuerpos::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string str_err= "(MapCuerpos) Procesando comando: " + cmd;
    if(verborrea>2)
      std::clog << str_err << std::endl;
    if(cmd == "tag_face")
      { //Nuevo identificador de la superficie.
        setTag(interpretaSize_t(status.GetString()));
        return true;
      }
    else if(cmd == "block") //Crea un nuevo cuerpo.
      {
        Nuevo<Block>(status);
        return true;
      }
    else
      return MapEnt<Body>::procesa_comando(status);
  }

//! @brief Inserta el nuevo cuerpo en el conjunto total y los conjuntos abiertos.
void XC::MapCuerpos::UpdateSets(Body *nuevo) const
  {
    Modelador *mdlr= const_cast<Modelador *>(getModelador());
    mdlr->get_sets().get_set_total()->GetCuerpos().push_back(nuevo);
    mdlr->get_sets().inserta_ent_mdlr(nuevo);
    MapSet::map_sets &abiertos= mdlr->get_sets().get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->GetCuerpos().push_back(nuevo);
      }
  }

//! @brief Crea concilia el número de divisiones de las líneas.
bool XC::MapCuerpos::conciliaNDivs(void)
  {
    getCad()->conciliaNDivs();
    return checkNDivs();
  }

//! @brief Verifica que los números de divisiones de los lados
//! son compatibles.
bool XC::MapCuerpos::checkNDivs(void) const
  {
    size_t conta= 0;
    if(!empty())
      for(const_iterator i= begin();i!=end();i++)
        if(!(*i).second->checkNDivs()) conta++;
    return (conta==0);
  }


//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::MapCuerpos::GetProp(const std::string &cod) const
  {
    if(cod=="num_bodies")
      {
        tmp_gp_szt= size();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return MapEnt<Body>::GetProp(cod);
  }
