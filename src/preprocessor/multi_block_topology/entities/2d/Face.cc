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
//Face.cc

#include "Face.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Polyline3d.h"
#include "xc_utils/src/geom/d2/Triangle3d.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"
#include "vtkCellType.h"

//! @brief Constructor.
XC::Face::Face(void)
  : CmbEdge(nullptr,0), ndivj(0) {}

//! @brief Constructor.
//!
//! @param ndivI: number of divisions for direction I.
//! @param ndivJ: number of divisions for direction J.
XC::Face::Face(Preprocessor *m,const size_t &ndivI, const size_t &ndivJ)
  : CmbEdge(m,ndivI), ndivj(ndivJ) {}

//! @brief Constructor.
//! @param name: Object identifier.
//! @param m: Pointer to preprocessor.
//! @param ndivI: number of divisions for direction I.
//! @param ndivJ: number of divisions for direction J.
XC::Face::Face(const std::string &name,Preprocessor *m,const size_t &ndivI, const size_t &ndivJ)
  : CmbEdge(name,m,ndivI), ndivj(ndivJ) {}

//! @brief Comparison operator.
bool XC::Face::operator==(const Face &other) const
  {
    std::cout << getClassName() << "::" << __FUNCTION__
              << other << std::endl;
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= CmbEdge::operator==(other);
	std::cout << "retval= " << retval << std::endl;
        if(retval)
          retval= (ndivj==other.ndivj);
       }
    std::cout << getClassName() << "::" << __FUNCTION__
              << "retval= " << retval << std::endl;
    return retval;
  }

//! @brief Sets the number of divisions for direction I.
void XC::Face::setNDivI(const size_t &ndi)
  { CmbEdge::ndiv= ndi; }

//! @brief Sets the number of divisions for direction J.
void XC::Face::setNDivJ(const size_t &ndj)
  { ndivj= ndj; }

//! @brief Inserts the body being passed as parameter neighbors
//! container of this surface.
void XC::Face::insert_body(Body *b)
  { bodies_surf.insert(b); }

//! @brief Updates topology.
void XC::Face::update_topology(void)
  {
    for(std::deque<Side>::iterator i=lines.begin();i!=lines.end();i++)
      (*i).getEdge()->insert_surf(this);
  }

//! @brief Returns the index of the edge in common with the surface
//! being passed as parameter (if it exists).
size_t XC::Face::CommonEdge(const Face &other) const
  {
    size_t retval= 0;
    size_t cont= 1;
    if(this == &other) return cont; //All border are common.
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++,cont++)
      {
        if((*i).getEdge()->isConnectedTo(other))
	  {
            retval= cont;
	    break;
	  }
      }
    return retval;
  }

//! Returns:
//! - 1 if the line belongs to both surfaces and the orientation is the same.
//! - -1 if the line belongs to both surfaces and the orientation is the opposite.
//! - 0 line doesn't belongs to both surfaces.
int XC::Face::SenseOfEdge(const Edge *l,const Face &other) const
  {
    //Searching for the line indices in this face 
    const size_t ind_l_esta= IndiceEdge(l);
    if(ind_l_esta == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; line :" << l->getName() 
                  << " is not an edge of the surface: "
		  << getName() << std::endl;
        return 0;
      }
    const size_t ind_l_other= other.IndiceEdge(l);
    if(ind_l_other == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; line :" << l->getName() 
                  << " is not an edge of the surface: "
		  << other.getName() << std::endl;
        return 0;
      }
    //Search the edges on each surface;
    const Side *l_esta= getSide(ind_l_esta);
    const Side *l_other= other.getSide(ind_l_other);
    if(l_esta->P2() == l_other->P2())
      return 1;
    else
      return -1;
  }

//! @brief Reverse the verrtex sequence.
//! 
//! Changes the orientation of the object (I->J->K->L => L->K->J->I)
void XC::Face::reverse(void)
  {
    CmbEdge::reverse();
    close();
  }

//! @brief Returns the i-th vertex.
const XC::Pnt *XC::Face::getVertex(const size_t &i) const
  { return getSide(i)->P1(); }

//! @brief Returns the contour of the face as a 3D polyline.
Polyline3d XC::Face::getContour(void) const
  { return getPolyline(); }

//! @brief Returns the face area.
double XC::Face::getArea(void) const
  {
    double retval= 0.0;
    //Simple trick for convex surfaces.
    Polyline3d contour= getContour();
    const Pos3d o= contour.getCenterOfMass();
    const size_t ns= contour.getNumSegments();
    for(size_t i=1;i<=ns;i++)
      {
	const Segment3d s= contour.getSegment(i);
	const Pos3d p1= s.getFromPoint();
	const Pos3d p2= s.getToPoint();
	const Triangle3d t(o,p1,p2);
	retval+= t.getArea();
      }
    return retval;
  }

//! @brief Return the surfaces that touch the line.
std::set<const XC::Face *> XC::getConnectedSurfaces(const Edge &p)
  { return p.getConnectedSurfaces(); }

//! @brief Returns true if the lines touches the body (neighbor).
bool XC::Face::isConnectedTo(const XC::Body &b) const
  {
    std::set<const Body *>::const_iterator i= bodies_surf.find(&b);
    return (i!=bodies_surf.end());
  }

//! @brief Returns the sets that contains this surface.
std::set<XC::SetBase *> XC::Face::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << getClassName() << __FUNCTION__
	        << "; preprocessor needed." << std::endl;
    return retval;
  }

//! @brief Appends the surface to each of the sets being passed as parameter.
void XC::Face::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        Set *s= dynamic_cast<Set *>(*i);
        if(s) s->getSurfaces().push_back(this);
      }
  }

//! @brief Returns a pointer to node which indices are being passed as parameters.
//!
//! @param i: index of the layer.
//! @param j: index of the row.
//! @param k: index of the column.
XC::Node *XC::Face::getNode(const size_t &i,const size_t &j,const size_t &k)
  { return CmbEdge::getNode(i,j,k); }

//! @brief Returns a pointer to node which indices are being passed as parameters.
//!
//! @param i: index of the layer.
//! @param j: index of the row.
//! @param k: index of the column.
const XC::Node *XC::Face::getNode(const size_t &i,const size_t &j,const size_t &k) const
  { return CmbEdge::getNode(i,j,k); }

//! @brief Returns a pointer to node which indices are is being passed as parameter.
XC::Node *XC::Face::getNode(const size_t &i,const size_t &j)
  { return const_cast<Node *>(static_cast<const Face &>(*this).getNode(i,j)); }

//! @brief Returns a pointer to node which indices are is being passed as parameter.
const XC::Node *XC::Face::getNode(const size_t &i,const size_t &j) const
  {
    const Node *retval= nullptr;
    if(ttzNodes.isIConstantLayer())
      retval= CmbEdge::getNode(1,i,j);
    else if(ttzNodes.isJConstantLayer())
      retval= CmbEdge::getNode(i,1,j);
    else if(ttzNodes.isKConstantLayer())
      retval= CmbEdge::getNode(i,j,1);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; the node set is not one-dimensional." << std::endl;
    return retval;
  }

//! @brief Returns a pointer to node which index is being passed as parameter.
XC::Node *XC::Face::getNode(const size_t &i)
  {
    std::cerr << getClassName() << __FUNCTION__
              << "; must not be called with only one index." << std::endl; 
    return nullptr;
  }

//! @brief Returns a pointer to node which index is being passed as parameter.
const XC::Node *XC::Face::getNode(const size_t &i) const
  { return const_cast<Node *>(static_cast<const Face &>(*this).getNode(i)); }

//! @brief Interfaz con VTK.
int XC::Face::getVtkCellType(void) const
  {
    int retval= VTK_EMPTY_CELL;
    const size_t nl= getNumberOfEdges();
    switch(nl)
      {
      case 1:
        retval= VTK_LINE;
      case 2:
        retval= VTK_EMPTY_CELL;
      case 3:
        retval= VTK_TRIANGLE;
      case 4:
        retval= VTK_QUAD;
      default:
        retval= VTK_POLYGON;
      }
    return retval;
  }

std::set<const XC::Pnt *> XC::getCommonVertex(const Face &f1,const Face &f2, const Face &f3)
  {
    std::set<const XC::Pnt *> f1v= f1.getVertices();
    std::set<const XC::Pnt *> f2v= f2.getVertices();
    std::set<const XC::Pnt *> f1_f2;
    std::set_intersection(f1v.begin(),f1v.end(),f2v.begin(),f2v.end(),
                  std::inserter(f1_f2,f1_f2.begin()));
    std::set<const XC::Pnt *> f3v= f3.getVertices();
    std::set<const XC::Pnt *> retval;
    std::set_intersection(f1_f2.begin(),f1_f2.end(),f3v.begin(),f3v.end(),
                  std::inserter(retval,retval.begin()));
    return retval;
  }
