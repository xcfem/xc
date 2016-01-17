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

//! @brief Busca la entidad cuyo nombre se pasa como parámetro.
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

