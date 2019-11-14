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
//SurfaceMap.cc

#include "SurfaceMap.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/multi_block_topology/entities/Pnt.h"
#include "preprocessor/multi_block_topology/entities/Face.h"
#include "preprocessor/multi_block_topology/entities/QuadSurface.h"
#include "preprocessor/set_mgmt/Set.h"



//! @brief Constructor.
XC::SurfaceMap::SurfaceMap(MultiBlockTopology *mbt)
  : EntityMap<Face>(mbt) {}

//! @brief Insert the new line in the total and the sets that are open.
void XC::SurfaceMap::UpdateSets(Face *nueva_face) const
  {
    Preprocessor *preprocessor= const_cast<Preprocessor *>(getPreprocessor());
    preprocessor->get_sets().get_set_total()->getSurfaces().push_back(nueva_face);
    preprocessor->get_sets().insert_ent_mdlr(nueva_face);
    MapSet::map_sets &open_sets= preprocessor->get_sets().get_open_sets();
    for(MapSet::map_sets::iterator i= open_sets.begin();i!= open_sets.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->getSurfaces().push_back(nueva_face);
      }
  }

//! @brief Conciliate the number of divisions of the lines.
bool XC::SurfaceMap::conciliaNDivs(void)
  { 
    getMultiBlockTopology()->conciliaNDivs();
    return checkNDivs();
  }

//! @brief Verifies that number of divisions of the edges
//! are compatible.
bool XC::SurfaceMap::checkNDivs(void) const
  {
    size_t conta= 0;
    if(!empty())
      for(const_iterator i= begin();i!=end();i++)
        if(!(*i).second->checkNDivs()) conta++;
    return (conta==0);
  }

//! @brief Find a face between the points or creates a new one.
//! and inserts it on the container
XC::Face *XC::SurfaceMap::createFace(Pnt *pA,Pnt *pB,Pnt *pC,Pnt *pD)
  {
    Face *retval= nullptr;
    if(pA && pB)
      {
	if((pA==pB) or (pA==pC) or (pA==pD) or
	   (pB==pC) or (pB==pD) or
	   (pC==pD))
	      std::cerr << getClassName() << __FUNCTION__
			<< "; vertices of the face: ("
			<< pA->getName() << ","
			<< pB->getName() 
			<< "), are the same." << std::endl;
	retval= find_face_ptr_by_vertices(*pA,*pB,*pC,*pD);
        if(!retval) //Face doesn't exists.
          {
            assert(getPreprocessor());
            retval= New<QuadSurface>();
	    QuadSurface *tmp= dynamic_cast<QuadSurface *>(retval);
            assert(tmp);
	    PntPtrArray points(2,2);
	    points(0,0)= pA; points(0,1)= pB;
	    points(1,0)= pC; points(1,1)= pD;
	    tmp->setPoints(points);
          }
        if(!retval)
	  std::cerr << getClassName() << __FUNCTION__
		    << "; can't get a line"
                    << " between points: " << pA->getName()
                    << " and " << pB->getName() << std::endl;
      }
    else
      std::cerr << getClassName() << __FUNCTION__
		<< "; error, null pointer to point (A, B or both)."
		<< std::endl;
    return retval;    
  }

//! @brief New quadrilateral surface.
XC::QuadSurface *XC::SurfaceMap::newQuadSurfacePts(const size_t &id_p1, const size_t &id_p2, const size_t &id_p3, const size_t &id_p4)
  {
    QuadSurface *retval= dynamic_cast<QuadSurface *>(this->New<QuadSurface>());
    assert(retval);
    ID tmp(4);
    tmp[0]= id_p1; tmp[1]= id_p2; tmp[2]= id_p3; tmp[3]= id_p4;
    retval->setPoints(tmp);
    return retval;
  }

//! @brief New quadrilateral surface.
XC::QuadSurface *XC::SurfaceMap::newQuadSurfaceLines(const size_t &id_p1, const size_t &id_p2, const size_t &id_p3, const size_t &id_p4)
  {
    QuadSurface *retval= dynamic_cast<QuadSurface *>(this->New<QuadSurface>());
    assert(retval);
    ID tmp(4);
    tmp[0]= id_p1; tmp[1]= id_p2; tmp[2]= id_p3; tmp[3]= id_p4;
    retval->addLines(tmp);
    return retval;
  }

//! @brief New quadrilateral surface.
XC::QuadSurface *XC::SurfaceMap::newQuadSurfaceGridPoints(const boost::python::list &l)
  {
    QuadSurface *retval= dynamic_cast<QuadSurface *>(this->New<QuadSurface>());
    assert(retval);
    retval->defGridPoints(l);
    return retval;
  }
