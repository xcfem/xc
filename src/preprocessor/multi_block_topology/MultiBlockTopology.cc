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
//MultiBlockTopology.cc

#include "MultiBlockTopology.h"

#include "preprocessor/multi_block_topology/entities/Pnt.h"
#include "preprocessor/multi_block_topology/entities/Line.h"
#include "preprocessor/multi_block_topology/entities/DividedLine.h"
#include "preprocessor/multi_block_topology/entities/CmbEdge.h"
#include "preprocessor/multi_block_topology/entities/CircularArc.h"
#include "preprocessor/multi_block_topology/entities/QuadSurface.h"
#include "preprocessor/multi_block_topology/entities/Block.h"
#include "preprocessor/multi_block_topology/entities/UniformGrid.h"
#include "preprocessor/multi_block_topology/matrices/PntPtrArray.h"
#include "preprocessor/multi_block_topology/matrices/PntPtrArray3d.h"
#include "trf/Rotation.h"
#include "trf/Reflection.h"
#include "trf/Scaling.h"
#include "trf/Translation.h"

#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"

#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d2/Plane.h"
#include "xc_utils/src/geom/d1/Line3d.h"

#include "boost/lexical_cast.hpp"


#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::MultiBlockTopology::MultiBlockTopology(Preprocessor *prep)
  : PreprocessorContainer(prep), reference_systems(this),
    transformaciones_geometricas(this),
    points(this), edges(this), faces(this), cuerpos(this), unif_grid(this),
    framework2d(this), framework3d(this) {}

//! @brief Assign indexes to the objects (nodes,elements,points,...)
//! to be used in VTK arrays.
void XC::MultiBlockTopology::numera(void)
  {
    points.numera();
    edges.numera();
    faces.numera();
    cuerpos.numera();
  }

//! @brief Return the «edge» that has as end points those
//! whose indices are passed as parameters.
XC::Edge *XC::MultiBlockTopology::busca_edge_extremos(const PntMap::Indice &pA,const PntMap::Indice &pB)
  {
    Edge *retval= nullptr;
    const Pnt *p1= points.busca(pA);
    const Pnt *p2= points.busca(pB);
    if(p1 && p2)
      { retval= busca_edge_ptr_extremos(*p1,*p2); }
    else
      {
        if(!p1)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; point identified by : "
                    << pA << " not found." << std::endl;
        if(!p2)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; point identified by : "
                    << pB << " not found." << std::endl;
      }
    return retval;
  }

//! @brief Returs the «edge» that has as end points those
//! whose indices are passed as parameter.
const XC::Edge *XC::MultiBlockTopology::busca_edge_extremos(const PntMap::Indice &pA,const PntMap::Indice &pB) const
  {
    const Edge *retval= nullptr;
    const Pnt *p1= points.busca(pA);
    const Pnt *p2= points.busca(pB);
    if(p1 && p2)
      { retval= busca_edge_ptr_extremos(*p1,*p2); }
    else
      {
        if(!p1)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; point identified by : "
                    << pA << " not found." << std::endl;
        if(!p2)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; point identified by : "
                    << pB << " not found." << std::endl;
      }
    return retval;
  }

//! @brief Conciliate number of divisions of the lines.
void XC::MultiBlockTopology::conciliaNDivs(void)
  {
    if(!faces.empty())
      {
        std::set<const Edge *> tmp_edges;
        for(LineMap::iterator i=edges.begin();i!=edges.end();i++)
          tmp_edges.insert((*i).second);
        const size_t max_num_iter= 1000; 
        size_t conta= 0;
        while(!tmp_edges.empty())
          {
            const Edge *lado= *tmp_edges.begin();
	    std::set<const XC::Edge *> homologos;
            homologos= lado->getHomologousSides(homologos);
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
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; too much iterations." << std::endl;
                break;
              }
          }
        for(SurfaceMap::iterator i=faces.begin();i!= faces.end();i++)
          (*i).second->ConciliaNDivIJ();        
      }
  }

//! @brief Search for the entity whose name is passed as a parameter.
XC::SetEstruct *XC::MultiBlockTopology::busca_set_estruct(const UniformGridMap::Indice &id)
  {
    SetEstruct *retval= points.busca(id);
    if(!retval)
      retval= unif_grid.busca(id);
    return retval;
  }

//! @brief Erase all the geometry entities.
void XC::MultiBlockTopology::clearAll(void)
  {
    reference_systems.clearAll();

    framework2d.clearAll();
    framework3d.clearAll();

    unif_grid.clearAll();
    cuerpos.clearAll();
    faces.clearAll();
    edges.clearAll();
    points.clearAll();
  }

XC::MultiBlockTopology::~MultiBlockTopology(void)
  { clearAll(); }

