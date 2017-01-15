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

#include "boost/lexical_cast.hpp"


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

