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
//MapUniformGrids.cc

#include "MapUniformGrids.h"
#include "modelador/Modelador.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "modelador/cad/entidades/Pnt.h"
#include "modelador/cad/entidades/UniformGrid.h"
#include "modelador/cad/entidades/SupCuadrilatera.h"
#include "modelador/set_mgmt/Set.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::MapUniformGrids::MapUniformGrids(Cad *cad)
  : MapEnt<UniformGrid>(cad) {}

//! @brief Lee un objeto UniformGrid desde el archivo de entrada.
bool XC::MapUniformGrids::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string str_err= "(MapUniformGrids) Procesando comando: " + cmd;
    if(verborrea>2)
      std::clog << str_err << std::endl;
    if(cmd == "tag_unif_grid")
      { //Nuevo identificador de la superficie.
        setTag(interpretaSize_t(status.GetString()));
        return true;
      }
    else if(cmd == "unif_grid") //Crea una nueva superficie cuadrilátera.
      {
        Nueva(status);
        return true;
      }
    else
      return MapEnt<UniformGrid>::procesa_comando(status);
  }

//! @brief Inserta la nueva linea en el conjunto total y los conjuntos abiertos.
void XC::MapUniformGrids::UpdateSets(UniformGrid *nuevo_unif_grid) const
  {
    Modelador *mdlr= const_cast<Modelador *>(getModelador());
    mdlr->get_sets().get_set_total()->GetUniformGrids().push_back(nuevo_unif_grid);
    mdlr->get_sets().inserta_ent_mdlr(nuevo_unif_grid);
    MapSet::map_sets &abiertos= mdlr->get_sets().get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->GetUniformGrids().push_back(nuevo_unif_grid);
      }
  }

//! @brief Crea una nueva malla uniforme.
XC::UniformGrid *XC::MapUniformGrids::Nueva(void)
  {
    UniformGrid *retval= busca(getTag());
    if(!retval) //La malla es nueva.
      {
        Modelador *mdlr= getModelador();
        retval= new UniformGrid(mdlr);
        (*this)[getTag()]= retval;
        UpdateSets(retval);
        tag++;
      }
    return retval;
  }

//! @brief Crea una nueva malla uniforme.
XC::UniformGrid *XC::MapUniformGrids::Nueva(CmdStatus &status)
  {
    std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
    bool nueva= true;
    size_t old_tag= getTag();
    if(fnc_indices.size()>0)
      {
        setTag(convert_to_size_t(fnc_indices[0])); //Identificador del punto.
        if(existe(getTag()))
          nueva= false;
      }
    UniformGrid *retval= Nueva();
    if(!nueva)
      setTag(old_tag);
    retval->LeeCmd(status);
    return retval;
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::MapUniformGrids::GetProp(const std::string &cod) const
  {
    if(cod == "face")
      {
        static const UniformGrid *cara;
        const size_t iCara= popSize_t(cod);
        cara= busca(iCara);
        if(cara)
          return any_const_ptr(cara);
        else
          {
            std::cerr << "MapUniformGrids::GetProp; no se encontró la superficie: '" 
                      << iCara << "'.\n";
            return any_const_ptr();
          }
      }
    else if(cod=="getTagNearestUniformGrid")
      {
        const Pos3d p= popPos3d(cod);
        const UniformGrid *tmp= getNearest(p);
        if(!tmp)
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
            std::cerr << "No se encontró una superficie cercana a la posición: "
                      << p << ". " << posLectura << std::endl;
          }
        else
          tmp_gp_int= tmp->GetTag();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="num_faces")
      {
        tmp_gp_szt= this->size();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return MapEnt<UniformGrid>::GetProp(cod);
  }
