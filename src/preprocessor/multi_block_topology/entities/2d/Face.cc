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
#include "xc_utils/src/geom/sis_ref/Ref2d3d.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Polyline3d.h"
#include "xc_utils/src/geom/d2/Triangle3d.h"
#include "xc_utils/src/geom/d2/Polygon3d.h"

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
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= CmbEdge::operator==(other);
        if(retval)
          retval= (ndivj==other.ndivj);
       }
    return retval;
  }

//! @brief Return the index of th opposite side with respect to the one
//! being passed as parameter.
int XC::Face::get_index_opposite_side(const int &indice) const
  {
    int retval= -1;
    const size_t numSides= getNumberOfEdges();
    if((numSides % 2) == 0) // even number of sides
      {
        assert(indice<numSides);
	const size_t n_2= numSides/2;
	retval= (indice+n_2) % numSides;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; number of sides is odd. There is no"
	        << " opposite side." << std::endl;
    return retval;
  }

//! @brief Return the opposite side with respect to the one
//! being passed as parameter.
const XC::Edge *XC::Face::get_opposite_side(const Edge *l) const
  {
    Face *this_no_const= const_cast<Face *>(this);
    return this_no_const->get_opposite_side(l);
  }

//! @brief Return the opposite side with respect to the one
//! being passed as parameter.
XC::Edge *XC::Face::get_opposite_side(const Edge *l)
  {
    Edge *retval= nullptr;
    const size_t numSides= getNumberOfEdges();
    if((numSides % 2) == 0) // even number of sides
      {
	const size_t indice= IndiceEdge(l);
	if(indice!=0)
	  { 
	    const size_t indOpp= get_index_opposite_side(indice-1);
	    assert(indOpp<numSides);
            retval= lines[indOpp].getEdge();
	  }
	else //No la encuentra.
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; line :" << l->getName() 
		    << " is not an edge of the surface: "
		    << getName() << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; number of sides is odd. There is no"
	        << " opposite side." << std::endl;
    return retval;    
  }

//! @brief Computes a suitable number of divisions for the edge argument
//! and its opposite side.
size_t XC::Face::calc_ndiv_opposite_sides(const Edge *edgeA,const size_t &nd) const
  {
    const Edge *edgeB= get_opposite_side(edgeA);
    size_t retval= nd;
    const size_t ndA= edgeA->NDiv();
    const size_t ndB= edgeB->NDiv();
    if(edgeA->hasNodes() && edgeB->hasNodes())
      {
        if(ndA==ndB)
          retval= ndA;
        else
	  std::cerr << __FUNCTION__ << "; sides: "
                    << edgeA->getName() << " y " << edgeB->getName() 
                    << " are already meshed, and they have different number of divisions ("
                    << ndA << " y " << ndB << std::endl;
      }
    else if(edgeA->hasNodes()) //A edge already meshed.
      {
        if(ndA!=nd)
          {
	    std::clog << __FUNCTION__ << "; edge: "
                      << edgeA->getName()
                      << " is already meshed, division number can't be changed."
                      << " to " << nd << " keeping NDiv= " << ndA << std::endl;
            retval= ndA;
          }
      }
    else if(edgeB->hasNodes()) //B edge already meshed.
      {
        if(ndB!=nd)
          {
	    std::clog << __FUNCTION__ << "; edge: "
                      << edgeB->getName()
                      << " is already meshed, division number can't be changed."
                      << " to " << nd << " keeping NDiv= " << ndB << std::endl;
            retval= ndB;
          }
      }
    return retval;
  }

//! @brief Sets the number of divisions for direction I.
void XC::Face::set_ndiv_i(const size_t &ndi)
  { CmbEdge::ndiv= ndi; }

//! @brief Sets the number of divisions for direction J.
void XC::Face::set_ndiv_j(const size_t &ndj)
  { ndivj= ndj; }

//! @brief Sets the number of divisions for all its sides.
//!
//! @param nd: number of divisions.
void XC::Face::setNDiv(const size_t &nd)
  {
    const size_t nl= getNumberOfEdges();
    if(nl>0)
      {
         for(std::deque<Side>::iterator i=lines.begin();i!=lines.end();i++)
          (*i).setNDiv(nd);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no sides defined." << std::endl;
  }


//! @brief Set the number of divisions on the i axis.
void XC::Face::setNDivI(const size_t &ndi)
  {
    const size_t numSides= getNumberOfEdges();
    if((numSides % 2) == 0) // even number of sides
      {
	const size_t n_2= numSides/2;
	for(size_t i= 0;i<n_2;i+= 2)
	  { set_ndiv_opposite_sides(i,ndi); }
      }
    else
      {
	std::clog << getClassName() << "::" << __FUNCTION__
	          << "; surface: " << getName()
	          << " has an odd number of sides: "
	          << numSides << " cannot set sizes on I sides."
	          << " Using setNDiv instead."
	          << std::endl;
	setNDiv(ndi);
      }
  }

//! @brief Set the number of divisions on the j axis.
void XC::Face::setNDivJ(const size_t &ndj)
  {
    const size_t numSides= getNumberOfEdges();
    if((numSides % 2) == 0) // even number of sides
      {
	const size_t n_2= numSides/2;
	for(size_t i= 0;i<n_2;i+= 2)
	  { set_ndiv_opposite_sides(i+1,ndj); }
      }
    else
      {
	std::clog << getClassName() << "::" << __FUNCTION__
	          << "; surface: " << getName()
	          << " has an odd number of sides: "
	          << numSides << " cannot set sizes on J sides."
	          << " Using setNDiv instead."
	          << std::endl;
	setNDiv(ndj);
      }
  }

//! @brief Set the number of divisions for the edge argument
//! and its opposite side.
void XC::Face::set_ndiv_opposite_sides(const size_t &A,const size_t &nd)
  {
    const size_t numSides= getNumberOfEdges();
    if((numSides % 2) == 0) // even number of sides
      {
        Edge *edgeA= lines[A].getEdge();
        const size_t ndc= calc_ndiv_opposite_sides(edgeA,nd);
	if(ndc!=nd)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; cannot set the number of divisions"
		    << " to " << nd << " using " << ndc
	            << " instead." << std::endl;
        if(ndc>0)
          {
	    const size_t B= get_index_opposite_side(A);
	    if((A%2)==0) // even side
	      { set_ndiv_i(ndc); }
	    else // odd side
	      { set_ndiv_j(ndc); }
            edgeA->setNDiv(ndc);
            Edge *edgeB= lines[B].getEdge();
            edgeB->setNDiv(ndc);
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; number of sides is odd: " 
                << lines.size() << " there is no opposite side." << std::endl;
  }

//! @brief Verifies that the number of divisions of the lines are
//! compatible.
bool XC::Face::checkNDivs(const size_t &i) const
  {
    const size_t ndivA= lines[i].getEdge()->NDiv();
    const size_t j= get_index_opposite_side(i);
    const size_t ndivB= lines[j].getEdge()->NDiv();
    if(ndivA!=ndivB)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; lines: "
                  << lines[i].getEdge()->getName() << " and "
                  << lines[j].getEdge()->getName() 
                  << " of surface: " << getName()
                  << " have different number of divisions ("
                  << ndivA << " y " << ndivB << ')' << std::endl;
        return false;
      }
    else
      return true;
  }

//! @brief Verifies that the number of divisions of the lines are
//! compatible.
bool XC::Face::checkNDivs(void) const
  {
    bool retval= true;
    const size_t numSides= getNumberOfEdges();
    if((numSides % 2) == 0) // even number of sides
      {
	const size_t n_2= getNumberOfEdges()/2;
	for(size_t i= 0;i<n_2;i++)
	  {
	    bool tmp= checkNDivs(i);
	    if(!tmp)
	      {
		retval= false;
		break;
	      }
	  }
      }
    return retval;
  }

//! @brief Conciliate lines division numbers with
//! those of the surface.
void XC::Face::ConciliaNDivIJ(void)
  {
    if(checkNDivs())
      {
	set_ndiv_i(lines[0].getEdge()->NDiv());
	set_ndiv_j(lines[1].getEdge()->NDiv());
      }
  }

//! @brief Computes the number of divisions on the i axis
//! for an element edge size equal or lesser than the
//! size being passed as parameter.
//!
//! @param sz: length of the element side.
//! @param mustBeEver: true if the number of divisions of each
//!                    line must be even.
void XC::Face::SetElemSize(const double &sz, bool mustBeEven)
  {
    const size_t numSides= getNumberOfEdges();
    for(size_t i= 0;i<numSides; i++)
      {
        Edge *edge= lines[i].getEdge();
        const double l= edge->getLength();
        size_t n= ceil(l/sz);
	if(mustBeEven && (n % 2 != 0))
	  n++;
        edge->setNDiv(n);
      }
  }

//! @brief Computes the number of divisions on the i axis
//! for an element edge size equal or lesser than the
//! size being passed as parameter.
void XC::Face::SetElemSizeI(const double &sz)
  {
    const size_t numSides= getNumberOfEdges();
    if((numSides % 2) == 0) // even number of sides
      {
	const size_t n_2= numSides/2;
	double lmax= 0.0;
	for(size_t i= 0;i<n_2;i+= 2)
	  {
	    const double l1= lines[i].getLength();
	    lmax= std::max(lmax, l1);
	    const size_t j= get_index_opposite_side(i);
	    const double l2= lines[j].getLength();
	    lmax= std::max(lmax, l2);
	  }	
	const size_t n= ceil(lmax/sz);
	setNDivI(n);
      }	
    else
      {
	std::clog << getClassName() << "::" << __FUNCTION__
	          << "; surface: " << getName()
	          << " has an odd number of sides: "
	          << numSides << " cannot set sizes on I and J sides."
	          << " Using setElemSize instead."
	          << std::endl;
	SetElemSize(sz);
      }
  }

//! @brief Computes the number of divisions on the j axis
//! for an element edge size equal or lesser than the
//! size being passed as parameter.
void XC::Face::SetElemSizeJ(const double &sz)
  {
    const size_t numSides= getNumberOfEdges();
    if((numSides % 2) == 0) // even number of sides
      {
	const size_t n_2= numSides/2;
	double lmax= 0.0;
	for(size_t i= 0;i<n_2;i+= 2)
	  {
	    const double l1= lines[i+1].getLength();
	    lmax= std::max(lmax, l1);
	    const size_t j= get_index_opposite_side(i+1);
	    const double l2= lines[j].getLength();
	    lmax= std::max(lmax, l2);
	  }	
	const size_t n= ceil(lmax/sz);
	setNDivJ(n);
      }	
    else
      {
	std::clog << getClassName() << "::" << __FUNCTION__
	          << "; surface: " << getName()
	          << " has an odd number of sides: "
	          << numSides << " cannot set sizes on I and J sides."
	          << " Using setElemSize instead."
	          << std::endl;
	SetElemSize(sz);
      }
  }


//! @brief Computes the number of divisions on the i and j axis
//! for an element edges sizes equal or lesser than the
//! sizes being passed as parameter.
void XC::Face::SetElemSizeIJ(const double &szI,const double &szJ)
  {
    const size_t numSides= getNumberOfEdges();
    if((numSides % 2) == 0) // even number of sides
      {
	SetElemSizeI(szI);
	SetElemSizeJ(szJ);
      }
    else
      {
	std::clog << getClassName() << "::" << __FUNCTION__
	          << "; surface: " << getName()
	          << " has an odd number of sides: "
	          << numSides << " cannot set sizes on I and J sides."
	          << " Using setElemSize instead."
	          << std::endl;
	SetElemSize(szI);
      }
  }

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

//! @brief Return a deque of opposite edge pairs.
std::deque<std::pair<const XC::Edge *, const XC::Edge *> > XC::Face::getOppositeEdges(void) const
  {
    std::deque<std::pair<const Edge *, const Edge *> > retval;
    const size_t numSides= getNumberOfEdges();
    if((numSides % 2) == 0) // even number of sides
      {
	const size_t n_2= getNumberOfEdges()/2;
	for(size_t i= 0;i<n_2;i++)
	  {
             const Edge *e0= lines[i].getEdge();
             const size_t j= get_index_opposite_side(i);
             const Edge *e1= lines[j].getEdge();
	     retval.push_back({e0, e1});
	  }
      }
    return retval;
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

//! @brief Return the surface contour as a polygon.
Polygon3d XC::Face::getPolygon(void) const
  { return Polygon3d(getContour()); }

//! @brief Returns a vector in the direction of the local
//! Z axis.
Vector3d XC::Face::getKVector(void) const
  {
    const Vector3d vI= getIVector();
    const Vector3d vJ= getJVector();
    return vI.getCross(vJ);
  }

//! @brief Returns a matrix with the axes of the surface as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
XC::Matrix XC::Face::getLocalAxes(void) const
  {
    Matrix retval(3,3);
    const Vector3d vectorI= getIVector();
    retval(0,0)= vectorI(1); retval(0,1)= vectorI(2); retval(0,2)= vectorI(3);
    const Vector3d vectorJ= getJVector();
    retval(1,0)= vectorJ(1); retval(1,1)= vectorJ(2); retval(1,2)= vectorJ(3);
    const Vector3d vectorK= vectorI.getCross(vectorJ);    
    retval(2,0)= vectorK(1); retval(2,1)= vectorK(2); retval(2,2)= vectorK(3);
    return retval;
  }

//! @brief Returns the centroid of the quad surface.
Pos3d XC::Face::getCentroid(void) const
  { return getPolygon().getCenterOfMass(); }

Ref2d3d XC::Face::getRef(void) const
  {
    const Pos3d org= getCentroid();
    const Vector3d vI= getIVector();
    const Vector3d vJ= getJVector();
    return Ref2d3d(org, vI, vJ);
  }

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
    std::set<const XC::Pnt *> f1v= f1.getVertexSet();
    std::set<const XC::Pnt *> f2v= f2.getVertexSet();
    std::set<const XC::Pnt *> f1_f2;
    std::set_intersection(f1v.begin(),f1v.end(),f2v.begin(),f2v.end(),
                  std::inserter(f1_f2,f1_f2.begin()));
    std::set<const XC::Pnt *> f3v= f3.getVertexSet();
    std::set<const XC::Pnt *> retval;
    std::set_intersection(f1_f2.begin(),f1_f2.end(),f3v.begin(),f3v.end(),
                  std::inserter(retval,retval.begin()));
    return retval;
  }
