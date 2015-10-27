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
//Cad.cc

#include "Cad.h"

#include "preprocessor/cad/entidades/Pnt.h"
#include "preprocessor/cad/entidades/Linea.h"
#include "preprocessor/cad/entidades/LineaTramos.h"
#include "preprocessor/cad/entidades/CmbEdge.h"
#include "preprocessor/cad/entidades/ArcoCircunf.h"
#include "preprocessor/cad/entidades/SupCuadrilatera.h"
#include "preprocessor/cad/entidades/Block.h"
#include "preprocessor/cad/entidades/UniformGrid.h"
#include "preprocessor/cad/matrices/MatrizPtrPnt.h"
#include "preprocessor/cad/matrices/TritrizPtrPnt.h"
#include "trf/Rotacion.h"
#include "trf/Reflexion.h"
#include "trf/Escalado.h"
#include "trf/Traslacion.h"

#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"

#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d1/Recta3d.h"

#include "xc_utils/src/nucleo/CmdParser.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/utils_any.h"
#include "boost/lexical_cast.hpp"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::Cad::Cad(Preprocessor *mod)
  : EntCmd(mod), preprocessor(mod),
    sistemas_referencia(this), transformaciones_geometricas(this),
    puntos(this), edges(this), faces(this), cuerpos(this), unif_grid(this),
    esquemas2d(this), esquemas3d(this) {}

//!  @brief Asigna índices a los objetos (nodos,elementos,puntos...) para poder emplearlos en VTK.
void XC::Cad::numera(void)
  {
    puntos.numera();
    edges.numera();
    faces.numera();
    cuerpos.numera();
  }

//! @brief Lee un objeto Pnt desde el archivo de entrada.
//!
//! Soporta los comandos:
//!
//! - pnt: Lee un nuevo punto.
//! - linea: Lee una nueva línea.
//! - arco: Lee un nuevo arco.
//! - sec_lineas: Lee una nueva línea compuesta.
//! - gm_superficie: Lee una nueva superficie.
//! - gm_cuerpo: Lee un nuevo sólido.
//! - gm_fila_pnt: Lee una fila de puntos.
//! - gm_escala: Lee un objeto escala.
//! - gm_traslacion: Lee un objeto traslación.
bool XC::Cad::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string str_err= "(Cad) Procesando comando: " + cmd;
    if(verborrea>2)
      std::clog << str_err << std::endl;
    if(!preprocessor)
      std::cerr << "Cad; ¡Ojo!, no se ha asignado un modelador." << std::endl;
    if(cmd == "tag_sis_ref")
      {
        sistemas_referencia.setTag(interpretaSize_t(status.GetString())); //Nuevo identificador del sistema de referencia.
        return true;
      }
    if(cmd == "tag_trf_geom")
      {
        transformaciones_geometricas.setTag(interpretaSize_t(status.GetString())); //Nuevo identificador de la transformación.
        return true;
      }
    if(cmd == "tag_punto")
      {
        puntos.setTag(interpretaSize_t(status.GetString())); //Nuevo identificador del punto.
        return true;
      }
    else if(cmd == "tag_edge")
      {
        edges.setTag(interpretaSize_t(status.GetString())); //Nuevo identificador del edge.
        return true;
      }
    else if(cmd == "tag_face")
      {
        faces.setTag(interpretaSize_t(status.GetString())); //Nuevo identificador.
        return true;
      }
    else if(cmd == "tag_body")
      {
        cuerpos.setTag(interpretaSize_t(status.GetString())); //Nuevo identificador.
        return true;
      }
    else if(cmd == "tag_unif_grid")
      {
        unif_grid.setTag(interpretaSize_t(status.GetString())); //Nuevo identificador.
        return true;
      }
    else if(cmd == "tag_esquema2d")
      {
        esquemas2d.setTag(interpretaSize_t(status.GetString())); //Nuevo identificador.
        return true;
      }
    else if(cmd == "tag_esquema3d")
      {
        esquemas3d.setTag(interpretaSize_t(status.GetString())); //Nuevo identificador.
        return true;
      }
    else if(cmd == "sistema_referencia") //Crea un nuevo sistema de referencia.
      {
        sistemas_referencia.Nuevo(status);
        return true;
      }
    else if(cmd == "rot") //Crea una nueva rotación.
      {
        transformaciones_geometricas.Nueva<Rotacion>(status);
        return true;
      }
    else if(cmd == "reflex") //Crea una nueva reflexión.
      {
        transformaciones_geometricas.Nueva<Reflexion>(status);
        return true;
      }
    else if(cmd == "escal") //Crea un nuevo escalado.
      {
        transformaciones_geometricas.Nueva<Escalado>(status);
        return true;
      }
    else if(cmd == "transl") //Crea una nueva traslacion.
      {
        transformaciones_geometricas.Nueva<Traslacion>(status);
        return true;
      }
    else if(cmd == "trf_pnts") //Aplica una transformación a los puntos que se pasan como parámetro.
      {
        std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
        size_t indice= transformaciones_geometricas.getTag();
        if(fnc_indices.size()>0)
          indice= convert_to_size_t(fnc_indices[0]); //Identificador de la transformacion.
        TrfGeom *trf= transformaciones_geometricas.busca(indice);
        if(trf)
          {
            const std::vector<size_t> tmp= crea_vector_size_t(status.GetString()); //Indices de los puntos.
            puntos.Transforma(*trf,tmp);
          }
        else
          {
            status.GetString(); //Ignoramos argumentos.
            std::cerr << "No se encontró la transformacion de índice: " << indice << ".\n";
          }
        return true;
      }
    else if(cmd == "pnt") //Crea un nuevo punto (o llama a uno existente).
      {
        puntos.Nuevo(status);
        return true;
      }
    else if(cmd == "linea")
      {
        edges.Nueva<Linea>(status);
        return true;
      }
    else if(cmd == "linea_tramos")
      {
        edges.Nueva<LineaTramos>(status);
        return true;
      }
    else if(cmd == "sec_lineas")
      {
        edges.Nueva<CmbEdge>(status);
        return true;
      }
    else if(cmd == "arco_circunf")
      {
        edges.Nueva<ArcoCircunf>(status);
        return true;
      }
    else if(cmd == "get_linea_extremos") //Crea una nueva línea (o llama a una existente)
      {
        std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
        if(fnc_indices.size()>1)
          {
            const size_t tag_p1= convert_to_size_t(fnc_indices[0]);
            const size_t tag_p2= convert_to_size_t(fnc_indices[1]);
            Edge *l= busca_edge_extremos(tag_p1,tag_p2);
            l->LeeCmd(status);
          }
        return true;
      }
    else if(cmd == "sup_cuadrilatera") //Crea una nueva superficie cuadrilátera.
      {
        faces.Nueva<SupCuadrilatera>(status);
        return true;
      }
    else if(cmd == "block") //Crea un nuevo "bloque".
      {
        cuerpos.Nuevo<Block>(status);
        return true;
      }
    else if(cmd == "copia_puntos")
      {
        const std::vector<size_t> tmp= crea_vector_size_t(status.GetString());
        puntos.Copia(tmp);
        return true;
      }
    else if(cmd == "unif_grid") //Crea una nueva malla uniforme.
      {
        unif_grid.Nueva(status);
        return true;
      }
    else if(cmd == "esquema2d") //Crea un esquema bidimensional.
      {
        esquemas2d.Nuevo(status);
        return true;
      }
    else if(cmd == "esquema3d") //Crea un esquema tridimensional.
      {
        esquemas3d.Nuevo(status);
        return true;
      }
    else if(cmd == "for_each_pnt")
      {
        const std::string bloque= status.GetBloque();
        puntos.EjecutaBloqueForEach(status,bloque);
        return true;
      }
    else if(cmd == "for_each_edge")
      {
        const std::string bloque= status.GetBloque();
        edges.EjecutaBloqueForEach(status,bloque);
        return true;
      }
    else if(cmd == "for_each_face")
      {
        const std::string bloque= status.GetBloque();
        faces.EjecutaBloqueForEach(status,bloque);
        return true;
      }
    else if(cmd == "for_each_body")
      {
        const std::string bloque= status.GetBloque();
        cuerpos.EjecutaBloqueForEach(status,bloque);
        return true;
      }
    else if(cmd == "numera")
      {
        status.GetBloque(); //Ignoramos entrada.
        numera();
        return true;
      }
    else if(cmd == "preprocessor")
      {
        if(preprocessor)
          preprocessor->LeeCmd(status);
        else
          {
            status.GetBloque(); //Ignoramos entrada.
	    std::cerr << "Cad::procesa_comando; no se ha asignado puntero al modelador." << std::endl;
          }
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

//! @brief Devuelve el «edge» que tiene por extremos los puntos
//! cuyos índices se pasan como parámetro.
XC::Edge *XC::Cad::busca_edge_extremos(const MapPuntos::Indice &pA,const MapPuntos::Indice &pB)
  {
    Edge *retval= nullptr;
    const Pnt *p1= puntos.busca(pA);
    const Pnt *p2= puntos.busca(pB);
    if(p1 && p2)
      { retval= busca_edge_ptr_extremos(*p1,*p2); }
    else
      {
        if(!p1)
          std::cerr << "Cad::busca_edge_extremos; no se encontró el punto de índice: "
                    << pA << std::endl;
        if(!p2)
          std::cerr << "Cad::busca_edge_extremos; no se encontró el punto de índice: "
                    << pB << std::endl;
      }
    return retval;
  }

//! @brief Devuelve el «edge» que tiene por extremos los puntos
//! cuyos índices se pasan como parámetro.
const XC::Edge *XC::Cad::busca_edge_extremos(const MapPuntos::Indice &pA,const MapPuntos::Indice &pB) const
  {
    const Edge *retval= nullptr;
    const Pnt *p1= puntos.busca(pA);
    const Pnt *p2= puntos.busca(pB);
    if(p1 && p2)
      { retval= busca_edge_ptr_extremos(*p1,*p2); }
    else
      {
        if(!p1)
          std::cerr << "Cad::busca_edge_extremos; no se encontró el punto de índice: "
                    << pA << std::endl;
        if(!p2)
          std::cerr << "Cad::busca_edge_extremos; no se encontró el punto de índice: "
                    << pB << std::endl;
      }
    return retval;
  }

//! @brief Crea concilia el número de divisiones de las líneas.
void XC::Cad::conciliaNDivs(void)
  {
    if(!faces.empty())
      {
        std::set<const Edge *> tmp_edges;
        for(MapLineas::iterator i=edges.begin();i!=edges.end();i++)
          tmp_edges.insert((*i).second);
        const size_t max_num_iter= 1000; 
        size_t conta= 0;
        while(!tmp_edges.empty())
          {
            const Edge *lado= *tmp_edges.begin();
	    std::set<const XC::Edge *> homologos;
            homologos= lado->GetLadosHomologos(homologos);
            homologos.insert(lado);
            const size_t nd= calcula_ndiv_lados(homologos);
            for(std::set<const Edge *>::const_iterator i= homologos.begin();i!=homologos.end();i++)
              {
                Edge *tmp= const_cast<Edge *>(*i);
                tmp->SetNDiv(nd);
                tmp_edges.erase(tmp);
              }
            conta++;
            if(conta>max_num_iter)
              {
		std::cerr << "Demasiadas iteraciones." << std::endl;
                break;
              }
          }
        for(MapSuperficies::iterator i=faces.begin();i!= faces.end();i++)
          (*i).second->ConciliaNDivIJ();        
      }
  }

//! @brief Busca la entidad del modelador cuyo nombre se pasa como parámetro.
XC::SetEstruct *XC::Cad::busca_set_estruct(const MapUniformGrids::Indice &id)
  {
    SetEstruct *retval= puntos.busca(id);
    if(!retval)
      retval= unif_grid.busca(id);
    return retval;
  }

//! @brief Borra todas las entidades geométricas.
void XC::Cad::clearAll(void)
  {
    sistemas_referencia.clearAll();

    esquemas2d.clearAll();
    esquemas3d.clearAll();

    unif_grid.clearAll();
    cuerpos.clearAll();
    faces.clearAll();
    edges.clearAll();
    puntos.clearAll();
  }

XC::Cad::~Cad(void)
  { clearAll(); }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::Cad::GetProp(const std::string &cod) const
  {
    if(cod == "pnt")
      {
        static const Pnt *punto;
        const size_t iPunto= popSize_t(cod);
        punto= puntos.busca(iPunto);
        if(punto)
          return any_const_ptr(punto);
        else
          {
            std::cerr << "Cad::GetProp; no se encontró el punto: '" 
                      << iPunto << "'.\n";
            return any_const_ptr();
          }
      }
    else if(cod=="getTagNearestPnt")
      {
        const Pos3d p= popPos3d(cod);
        
        const Pnt *tmp= puntos.getNearest(p);
        if(!tmp)
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
            std::cerr << "No se encontró un punto cercano a la posición: "
                      << p << ". " << posLectura << std::endl;
          }
        else
          tmp_gp_szt= tmp->GetTag();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod == "linea")
      {
        static const Edge *edge= nullptr;
        const size_t iEdge= popSize_t(cod);
        edge= edges.busca(iEdge);
        if(edge)
          return any_const_ptr(edge);
        else
          {
            std::cerr << "Cad::GetProp; no se encontró la línea: '" 
                      << iEdge << "'.\n";
            return any_const_ptr();
          }
      }
    else if(cod=="getTagNearestLine")
      {
        const Pos3d p= popPos3d(cod);
        const Edge *tmp= edges.getNearest(p);
        if(!tmp)
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
            std::cerr << "No se encontró una línea cercana a la posición: "
                      << p << ". " << posLectura << std::endl;
          }
        else
          tmp_gp_int= tmp->GetTag();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="getTagNearestFace")
      {
        const Pos3d p= popPos3d(cod);
        const Face *tmp= faces.getNearest(p);
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
    else if(cod=="tag_sistema_referencia")
      return any_const_ptr(sistemas_referencia.getTag());
    else if(cod=="tag_punto")
      return any_const_ptr(puntos.getTag());
    else if(cod=="last_tag_punto")
      {
        tmp_gp_szt= puntos.getTag()-1;
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="tag_edge")
      return any_const_ptr(edges.getTag());
    else if(cod=="last_tag_edge")
      {
        tmp_gp_szt= edges.getTag()-1;
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="tag_face")
      return any_const_ptr(faces.getTag());
    else if(cod=="last_tag_face")
      {
        tmp_gp_szt= faces.getTag()-1;
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="tag_unif_grid")
      return any_const_ptr(unif_grid.getTag());
    else if(cod=="tag_esquema2d")
      return any_const_ptr(esquemas2d.getTag());
    else if(cod=="tag_esquema3d")
      return any_const_ptr(esquemas3d.getTag());
    else if(cod=="num_puntos")
      {
        tmp_gp_szt= puntos.size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="num_edges")
      {
        tmp_gp_szt= edges.size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="dist_puntos")
      {
        size_t iPuntoA= 0, iPuntoB= 0;
        if(InterpreteRPN::Pila().size()>1)
          {
            iPuntoA= convert_to_size_t(InterpreteRPN::Pila().Pop());
            iPuntoB= convert_to_size_t(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,2,"GetProp",cod);
        tmp_gp_dbl= puntos.Dist(iPuntoA,iPuntoB);
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return EntCmd::GetProp(cod);
  }
