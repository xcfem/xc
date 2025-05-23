//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
#include "PolygonalFace.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/coo_sys/ref_sys/Ref2d3d.h"
#include "utility/geom/d3/BND3d.h"
#include "utility/geom/d1/Polyline3d.h"
#include "utility/geom/d2/Triangle3d.h"
#include "utility/geom/d2/Polygon3d.h"
#include "utility/geom/pos_vec/Pos3dArray.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"
#include "vtkCellType.h"

#include <gmsh.h>
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::Face::Face(void)
  : CmbEdge(nullptr,0), hole(false), ndivj(0) {}

//! @brief Constructor.
//!
//! @param ndivI: number of divisions for direction I.
//! @param ndivJ: number of divisions for direction J.
XC::Face::Face(Preprocessor *m,const size_t &ndivI, const size_t &ndivJ)
  : CmbEdge(m,ndivI), hole(false), ndivj(ndivJ) {}

//! @brief Constructor.
//! @param name: Object identifier.
//! @param m: Pointer to preprocessor.
//! @param ndivI: number of divisions for direction I.
//! @param ndivJ: number of divisions for direction J.
XC::Face::Face(const std::string &name,Preprocessor *m,const size_t &ndivI, const size_t &ndivJ)
  : CmbEdge(name,m,ndivI), hole(false), ndivj(ndivJ) {}

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
	  retval= (hole==other.hole);
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
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; number of sides is odd. There is no"
	        << " opposite side."
		<< Color::def << std::endl;
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
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; line :" << l->getName() 
		    << " is not an edge of the surface: "
		    << getName()
		    << Color::def << std::endl;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; number of sides is odd. There is no"
	        << " opposite side."
		<< Color::def << std::endl;
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
	  std::cerr << Color::red << __FUNCTION__ << "; sides: "
                    << edgeA->getName() << " y " << edgeB->getName() 
                    << " are already meshed, and they have different number of divisions ("
                    << ndA << " y " << ndB
		    << Color::def << std::endl;
      }
    else if(edgeA->hasNodes()) //A edge already meshed.
      {
        if(ndA!=nd)
          {
	    std::clog << Color::red << __FUNCTION__ << "; edge: "
                      << edgeA->getName()
                      << " is already meshed, division number can't be changed."
                      << " to " << nd << " keeping NDiv= " << ndA
		      << Color::def << std::endl;
            retval= ndA;
          }
      }
    else if(edgeB->hasNodes()) //B edge already meshed.
      {
        if(ndB!=nd)
          {
	    std::clog << Color::red << __FUNCTION__ << "; edge: "
                      << edgeB->getName()
                      << " is already meshed, division number can't be changed."
                      << " to " << nd << " keeping NDiv= " << ndB
		      << Color::def << std::endl;
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
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; no sides defined."
		<< Color::def << std::endl;
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
	std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	          << "; surface: " << getName()
	          << " has an odd number of sides: "
	          << numSides << " cannot set sizes on I sides."
	          << " Using setNDiv instead."
	          << Color::def << std::endl;
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
	std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	          << "; surface: " << getName()
	          << " has an odd number of sides: "
	          << numSides << " cannot set sizes on J sides."
	          << " Using setNDiv instead."
	          << Color::def << std::endl;
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
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; cannot set the number of divisions"
		    << " to " << nd << " using " << ndc
	            << " instead."
		    << Color::def << std::endl;
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
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; number of sides is odd: " 
                << lines.size() << " there is no opposite side."
		<< Color::def << std::endl;
  }

//! @brief If the i-th edge and its opposite edges are incompatibles return a
//! list containing a pointer to each of them. Otherwise returns an empty list.
std::deque<const XC::Edge *> XC::Face::getNDivErrors(const size_t &i) const
  {
    std::deque<const Edge *> retval;
    const Edge *edgeI= lines[i].getEdge();
    const size_t ndivA= edgeI->NDiv();
    const size_t j= get_index_opposite_side(i);
    const Edge *edgeJ= lines[j].getEdge();
    const size_t ndivB= edgeJ->NDiv();
    if(ndivA!=ndivB)
      {
	retval.push_back(edgeI);
	retval.push_back(edgeJ);
      }
    return retval;
  }

//! @brief Verifies that the number of divisions of the lines are
//! compatible.
bool XC::Face::checkNDivs(const size_t &i) const
  {
    std::deque<const Edge *> tmp= this->getNDivErrors(i);
    if(!tmp.empty())
      {
        const size_t ndivA= tmp[0]->NDiv();
        const size_t ndivB= tmp[1]->NDiv();
	if(verbosity>0)
	  std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
		    << "; lines: "
		    << tmp[0]->getName() << " and "
		    << tmp[1]->getName() 
		    << " of surface: " << getName()
		    << " have different number of divisions ("
		    << ndivA << " y " << ndivB << ')'
		    << Color::def << std::endl;
        return false;
      }
    else
      return true;
  }

//! @brief Returns a list with the edges that have an incompatible number of divisions.
std::deque<const XC::Edge *> XC::Face::getNDivErrors(void) const
  {
    std::deque<const Edge *> retval;
    const size_t numSides= getNumberOfEdges();
    if((numSides % 2) == 0) // even number of sides
      {
	const size_t n_2= getNumberOfEdges()/2;
	for(size_t i= 0;i<n_2;i++)
	  {
	    std::deque<const Edge *> tmp= getNDivErrors(i);
            for(std::deque<const Edge *>::const_iterator j= tmp.begin(); j!= tmp.end(); j++)
	      retval.push_back(*j);
	  }
      }
    return retval;
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
	std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	          << "; surface: " << getName()
	          << " has an odd number of sides: "
	          << numSides << " cannot set sizes on I and J sides."
	          << " Using setElemSize instead."
	          << Color::def << std::endl;
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
	std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	          << "; surface: " << getName()
	          << " has an odd number of sides: "
	          << numSides << " cannot set sizes on I and J sides."
	          << " Using setElemSize instead."
	          << Color::def << std::endl;
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
	std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	          << "; surface: " << getName()
	          << " has an odd number of sides: "
	          << numSides << " cannot set sizes on I and J sides."
	          << " Using setElemSize instead."
	          << Color::def << std::endl;
	SetElemSize(szI);
      }
  }

//! @brief Return an iterator to the hole corresponding to the argument
//! return holes.end() if not found.
XC::Face::hole_iterator XC::Face::findHole(PolygonalFace *pFace)
  {
    hole_iterator retval= holes.end();
    for(hole_iterator i= holes.begin(); i!= holes.end(); i++)
      if(*i==pFace)
	{
	  retval= i;
	  break;
	}
    return retval;
  }

//! @brief Return an iterator to the hole corresponding to the argument
//! return holes.end() if not found.
XC::Face::hole_const_iterator XC::Face::findHole(PolygonalFace *pFace) const
  {
    Face *this_no_const= const_cast<Face *>(this);
    return this_no_const->findHole(pFace);
  }

//! @brief Return a pointer to the hole corresponding to the argument
//! return nullptr if not found.
XC::PolygonalFace *XC::Face::findHolePtr(PolygonalFace *pFace)
  {
    PolygonalFace *retval= nullptr;
    hole_iterator i= findHole(pFace);    
    if(i!=holes.end())
      retval= *i;
    return retval;
  }

//! @brief Return a pointer to the hole corresponding to the argument
//! return nullptr if not found.
const XC::PolygonalFace *XC::Face::findHolePtr(PolygonalFace *pFace) const
  {
    Face *this_no_const= const_cast<Face *>(this);
    return this_no_const->findHolePtr(pFace);
  }

//! @brief Return true if the face corresponds to a hole.
bool XC::Face::isHole(void) const
  { return hole; }

//! @brief Return true if the face corresponds to a hole.
void XC::Face::setHole(const bool &b)
  { hole= b; }

//! @brief Add a hole to the face.
//!
//! @param pFace: hole to add.
void XC::Face::addHole(PolygonalFace *pFace)
  {
    // Check if hole is already added
    if(findHolePtr(pFace))
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; hole: " << pFace->getName()
		<< " is already added. Doing nothing."
	        << Color::def << std::endl;
    else
      {
	pFace->setHole(true);
        holes.push_back(pFace);
      }
  }

//! @brief Return a list of the face holes.
const std::deque<XC::PolygonalFace *> &XC::Face::getHoles(void) const
  { return holes; }

//! @brief Return a list of the face holes.
boost::python::list XC::Face::getPyHoles(void) const
  {
    boost::python::list retval;
    for(hole_const_iterator i= holes.begin(); i!= holes.end(); i++)
      {
	PolygonalFace *pFace= *i;	
        boost::python::object pyObj(boost::ref(*pFace));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Create a Gmsh curve loop from the face sides.
int XC::Face::create_gmsh_loop(void) const
  {
    const size_t numSides= getNumberOfEdges();
    std::vector<int> gmshContourTags(numSides);
    for(size_t i= 0;i<numSides; i++)
      {
	const Side &side= lines[i];
	int gmshLineTag= side.getTag()+1; // Gmsh tags must be strictly positive.
	if(not side.isDirect()) //Change sign when needed.
	  gmshLineTag= -gmshLineTag;
	gmshContourTags[i]= gmshLineTag;
      }
    const int gmshLoopTag= getTag()+1; // Gmsh tags must be strictly positive.
    int retval= gmsh::model::geo::addCurveLoop(gmshContourTags,gmshLoopTag);
    return retval;
  }

//! @brief Create the curve loops of the face holes.
std::vector<int> XC::Face::create_gmsh_loops_for_holes(void) const
  {
    const size_t sz= holes.size();
    std::vector<int> retval(sz);
    size_t count= 0;
    for(std::deque<PolygonalFace *>::const_iterator i= holes.begin(); i!= holes.end(); i++, count++)
      { retval[count]= (*i)->create_gmsh_loop(); }
    return retval;
  }

//! @brief Create the curve loops for the outer face and the holes.
std::vector<int> XC::Face::create_gmsh_loops(void) const
  {
    const size_t num_loops= holes.size()+1;
    std::vector<int> retval(num_loops);
    size_t count= 0;
    //// Contour loop.
    retval[count]= create_gmsh_loop();
    count++;
    //// Hole loops.
    std::vector<int> holeTags= create_gmsh_loops_for_holes();
    for(std::vector<int>::const_iterator i= holeTags.begin(); i!= holeTags.end(); i++, count++)
      {	retval[count]= *i; }
    return retval;
  }

//! @brief Ask Gmsh to create the surface corresponding to this
//! face.
int XC::Face::create_gmsh_surface(void) const
  {
    int retval= -1;
    if(!isHole())
      {
	// Create gmsh loops.
	std::vector<int> loopTags= create_gmsh_loops();
	// Create gmsh surface.
	const int gmshTag= getTag()+1;
	retval= gmsh::model::geo::addPlaneSurface(loopTags, gmshTag);
      }
    return retval;
  }

//! @brief Return a pointer to the side at the position
//! argument. If not found returns nullptr.
//! @param pos: 3D position argument.
std::deque<XC::Face::Side *> XC::Face::findSides(const Pos3d &pos)
  {
    std::deque<Side *> retval= CmbEdge::findSides(pos);
    // Search on holes.
    for(hole_const_iterator i= holes.begin(); i!= holes.end(); i++)
      {
	std::deque<Side *> tmp= (*i)->findSides(pos);
	retval.insert(retval.end(), tmp.begin(), tmp.end());
      }
    return retval;
  }

//! @brief Return a pointer to the vertex at the position
//! argument. If not found returns nullptr.
//! @param pos: 3D position argument.
XC::Pnt *XC::Face::findVertex(const Pos3d &pos)
  {
    XC::Pnt *retval= CmbEdge::findVertex(pos);
    if(!retval)
      {
	 // Search on holes.
	 for(hole_const_iterator i= holes.begin(); i!= holes.end(); i++)
	   {
	     retval= (*i)->findVertex(pos);
	     if(retval)
	       break;
	   }
      }
    return retval;
  }

//! @brief Creates interior nodes at the positions being passed as parameters.
std::vector<XC::Node *> XC::Face::create_interior_nodes(const std::vector<Pos3d> &positions)
  {
    const size_t sz= positions.size();
    std::vector<XC::Node *> retval(sz,nullptr);
    if(ttzNodes.Null())
      {
	ttzNodes= NodePtrArray3d(1,1,sz);
	if(getPreprocessor())
	  {
	    for(size_t i=0;i<sz;i++)
	      retval[i]= create_node(positions[i],1,1,i+1);
	    if(verbosity>5)
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; created " << ttzNodes.NumPtrs() << " node(s)."
			<< Color::def << std::endl;
	  }
      }
    else
      if(verbosity>2)
	std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
		  << "; nodes from entity: '" << getName()
		  << "' already exist."
		  << Color::def << std::endl;
    return retval;
  }

//! @brief Create nodes from quad tags (i.e. [tagI, tagJ, tagK, tagL].
int XC::Face::create_elements_from_quads(const std::deque<std::vector<int> > &quads)
  {
    int retval= 0;
    const Element *seed= getPreprocessor()->getElementHandler().get_seed_element();
    if(seed)
      {
	const size_t numElements= quads.size();
	ttzElements= ElemPtrArray3d(1,1,numElements);
	for(size_t i= 0;i<numElements;i++)
	  {
	    std::vector<int> quad= quads[i];
	    const size_t nNodes= quad.size();
	    if(nNodes>0)
	      {
		ID nTags(nNodes);
		for(size_t j= 0; j<nNodes; j++)
		  { nTags[j]= quad[j]; }
		Element *tmp= seed->getCopy();
		tmp->setIdNodes(nTags);
		ttzElements(1,1,i+1)= tmp;
	      }
	    else
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; empty quad at position: " << i
			<< Color::def << std::endl;
	  }
	add_elements_to_handler(ttzElements);
	retval= numElements;
      }
    else if(verbosity>0)
      std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
		<< "; seed element not set."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Triggers node creation on the edges.
void XC::Face::create_line_nodes(void)
  {
    // Create nodes on face contour.
    CmbEdge::create_line_nodes();
    
    // Create nodes on holes.
    for(hole_const_iterator i= holes.begin(); i!= holes.end(); i++)
      (*i)->create_line_nodes();
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

//! @brief Return the opposite side with respect to the one
//! being passed as parameter.
const XC::Edge *XC::Face::getOppositeEdge(const Edge &e) const
  { return get_opposite_side(&e); }

//! @brief Return the opposite side with respect to the one
//! being passed as parameter.
XC::Edge *XC::Face::getOppositeEdge(const Edge &e)
  { return get_opposite_side(&e); }

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

//! @brief Returns the index of the edge in common with the given surface
//! (if it exists).
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

//! @brief Returns the edges in common with the given surface.
std::set<const XC::Edge *> XC::Face::getCommonEdges(const Face &other)
  {
    std::set<const Edge *> retval;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      {
	const Edge *candidate= (*i).getEdge();
        if(candidate->isConnectedTo(other))
	  {
            retval.insert(candidate);
	    break;
	  }
      }
    return retval;
  }

//! @brief Returns the edges in common with the given surface.
boost::python::list XC::Face::getCommonEdgesPy(const Face &other)
  {
    const std::set<const Edge *> tmp= this->getCommonEdges(other);
    boost::python::list retval;
    for(std::set<const Edge *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	const Edge *pEdge= *i;	
        boost::python::object pyObj(boost::ref(*pEdge));
	retval.append(pyObj);
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
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; line :" << l->getName() 
                  << " is not an edge of the surface: "
		  << getName()
		  << Color::def << std::endl;
        return 0;
      }
    const size_t ind_l_other= other.IndiceEdge(l);
    if(ind_l_other == 0)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; line :" << l->getName() 
                  << " is not an edge of the surface: "
		  << other.getName()
		  << Color::def << std::endl;
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

//! @brief Move forward or backwards the sequence of vertexes.
//! 
//! Move forward or backwards the sequence of vertexes. For example if
//! the given advance is 2. Changes the vertex sequence as follows
//! newI= K, newJ= L, newK= I, newL= J.
void XC::Face::revolve(const int &advance)
  {
    CmbEdge::revolve(advance);
    if(advance!=0)
      close();
  }

//! @brief Set the orientation of the face normal according to the vector
//! argument. If the dot product of the vector and the surface normal vector is
//! negative the method flips the surface.
//! @param v: orientation vector.
void XC::Face::setKOrientation(const Vector3d &v)
  {
    const double d= dot(this->getKVector(), v);    
    if(d<0.0)
      this->reverse();
  }


//! @brief Set the orientation of the face i unit vector as close as possible
//! to the given vector.
//! @param v: orientation vector.
int XC::Face::setIOrientation(const Vector3d &v)
  {
    const Vector3d iVector= this->getIVector();
    const double angle= v.getAngle(iVector);
    const int advance= int(round(2*angle/M_PI));
    if(advance!=0)
      {
	this->revolve(advance);
	const double newAngle= v.getAngle(this->getIVector());
	const int newAdvance= int(round(2*newAngle/M_PI));
	if(newAdvance!=0)
	  this->revolve(newAdvance);
      }
    return advance;
  }


//! @brief Returns the i-th vertex.
const XC::Pnt *XC::Face::getVertex(const size_t &i) const
  { return getSide(i)->P1(); }

//! @brief Returns the i-th vertex.
XC::Pnt *XC::Face::getVertex(const size_t &i)
  { return getSide(i)->P1(); }

//! @brief Returns the contour of the face as a 3D polyline.
Polyline3d XC::Face::getContour(void) const
  { return getPolyline(); }

//! @brief Return the plane containing the face.
Plane XC::Face::getPlane(void) const
  {
    Polyline3d contour= getContour();
    contour.pop_back(); // remove repeated vertex.
    Plane retval= Plane(contour.begin(), contour.end());
    const Vector3d planeNormal= retval.Normal();
    const Vector3d thisNormal= getKVector();
    const double tmp= (planeNormal-thisNormal).GetModulus2(); //must be zero.
    if(tmp>1e-2) // plane is inverted.
      retval.swap();
    return retval;
  }

//! @brief return true if the surface is flat.
//! @param tol: tolerance for the maximum distance from the vertexes to the theoretical plane.
bool XC::Face::isFlat(const double &tol) const
  {
    const Plane plane= this->getPlane();
    // Compute the maximum distance from the plane to the vertices.
    Polyline3d polyline= getPolyline();
    double maxDist2= 0.0;
    const GeomObj::list_Pos3d &vertexes= polyline.getVertexList();
    const size_t sz= vertexes.size();
    if(sz>3)
	for(GeomObj::list_Pos3d::const_iterator i= vertexes.begin();i!=vertexes.end();i++)
	  {
	    const Pos3d &pos= *i;
	    const double d2= plane.dist2(pos);
	    if(d2>maxDist2)
	      { maxDist2= d2; }
	  }
    bool retval= (sqrt(maxDist2)<=tol);
    return retval;
  }

//! @brief return true if the surface is warped.
//! @param tol: tolerance for the maximum distance from the vertexes to the theoretical plane.
bool XC::Face::isWarped(const double &tol) const
  { return !isFlat(tol); }

//! @brief Return the surface contour as a polygon.
Polygon3d XC::Face::getPolygon(void) const
  {
    Polyline3d contour= getContour();
    contour.pop_back(); // remove repeated vertex.
    return Polygon3d(contour);
  }

//! @brief Return the squared distance to the point.
double XC::Face::getSquaredDistanceTo(const Pos3d &pt) const    
  { return getPolygon().dist2(pt); }


//! @brief Return true if the point list is oriented clockwise.
//! @param vPoint: position of the point of view.
bool XC::Face::clockwise(const Pos3d &vPoint) const
  {
    const Polygon3d plg= getPolygon();
    return plg.clockwise(vPoint);
  }

//! @brief Return the orientation of the face
//! @param vPoint: position of the point of view.
std::string XC::Face::orientation(const Pos3d &vPoint) const
  {
    std::string retval= "counterclockwise";
    if(clockwise(vPoint))
      { retval= "clockwise"; }
    return retval;
  }

//! @brief Return true if the points are counter-clockwise ordered
//! with respect to the element.
bool XC::Face::counterclockwise(const Pos3d &vPoint) const
  { return !clockwise(vPoint); }

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

//! @brief Return the surfaces that touch the given line.
std::set<const XC::Face *> XC::get_connected_surfaces(const Edge &p)
  { return p.getConnectedSurfaces(); }

//! @brief Return the bodies that touch this surface (neighbors).
const std::set<const XC::Body *> &XC::Face::getConnectedBodies(void) const
  { return this->bodies_surf; }

//! @brief Return the bodies of the given set that touch this surface.
std::set<const XC::Body *> XC::Face::getConnectedBodies(const SetBase *s) const
  {
    std::set<const Body *> retval;
    if(s)
      {
	if(!this->bodies_surf.empty())
	  {
	    std::set<const Body *>::const_iterator i= this->bodies_surf.begin();
	    for(;i!=this->bodies_surf.end();i++)
	      {
		const Body *b= *i;
		if(s->In(b))
		  retval.insert(b);
	      }
	  }
      }
    else
      retval= this->getConnectedBodies();
    return retval;
  }

//! @brief Return the bodies that touch this surface (neighbors).
boost::python::list XC::Face::getConnectedBodiesPy(void) const
  {
    const std::set<const Body *> tmp= this->getConnectedBodies();
    boost::python::list retval;
    for(std::set<const Body *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	const Body *pBody= *i;	
        boost::python::object pyObj(boost::ref(*pBody));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Return the bodies of the given set that touch this surface.
boost::python::list XC::Face::getConnectedBodiesPy(const SetBase *s) const
  {
    const std::set<const Body *> tmp= this->getConnectedBodies(s);
    boost::python::list retval;
    for(std::set<const Body *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	const Body *pBody= *i;	
        boost::python::object pyObj(boost::ref(*pBody));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Return the surfaces that touch this surface (neighbors).
std::set<const XC::Face *> XC::Face::getConnectedSurfaces(void) const
  {
    std::set<const Face *> retval;
    const std::deque<const Pnt *> vertices= this->getVertices();
    for(std::deque<const Pnt *>::const_iterator i= vertices.begin(); i!=vertices.end(); i++)
      {
	const Pnt *pt= *i;
	const std::set<const Face *> tmp= pt->getConnectedSurfaces();
	for(std::set<const Face *>::const_iterator j= tmp.begin(); j!=tmp.end(); j++)
	  {
	    const Face *face= *j;
	    if(face!=this)
	      retval.insert(face);
	  }
      }
    return retval;
  }

//! @brief Return the surfaces of the given set that touch this one.
std::set<const XC::Face *> XC::Face::getConnectedSurfaces(const SetBase *s) const
  {
    std::set<const Face *> retval;
    const std::set<const Face *> tmp= this->getConnectedSurfaces();
    if(s)
      {
	if(!tmp.empty())
	  {
	    for(std::set<const Face *>::const_iterator i= tmp.begin(); i!=tmp.end(); i++)
	      {
		const Face *f= *i;
		if(s->In(f))
		  retval.insert(f);
	      }
	  }
      }
    else
      retval= tmp;
    return retval;
  }

//! @brief Return the surfaces that touch this surface (neighbors).
boost::python::list XC::Face::getConnectedSurfacesPy(void) const
  {
    const std::set<const Face *> tmp= this->getConnectedSurfaces();
    boost::python::list retval;
    for(std::set<const Face *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	const Face *pFace= *i;	
        boost::python::object pyObj(boost::ref(*pFace));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Return the surfaces of the given set that touch this one.
boost::python::list XC::Face::getConnectedSurfacesPy(const SetBase *s) const
  {
    const std::set<const Face *> tmp= this->getConnectedSurfaces(s);
    boost::python::list retval;
    for(std::set<const Face *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	const Face *pFace= *i;	
        boost::python::object pyObj(boost::ref(*pFace));
	retval.append(pyObj);
      }
    return retval;
  }

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
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; preprocessor needed."
		<< Color::def << std::endl;
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
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; the node set is not one-dimensional."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Returns a pointer to node which index is being passed as parameter.
XC::Node *XC::Face::getNode(const size_t &i)
  {
    std::cerr << Color::red << getClassName() << __FUNCTION__
              << "; must not be called with only one index."
	      << Color::def << std::endl; 
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

std::set<const XC::Pnt *> XC::get_common_vertex(const Face &f1,const Face &f2, const Face &f3)
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

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::Face::getPyDict(void) const
  {
    boost::python::dict retval= CmbEdge::getPyDict();
    boost::python::list bodyTags;
    for(std::set<const Body *>::const_iterator i= bodies_surf.begin(); i!=bodies_surf.end(); i++)
      {
	const Body *b= *i;
        bodyTags.append(b->getTag());
      }
    retval["bodyTags"]= bodyTags;
    retval["ndivj"]= this->ndivj;
    boost::python::list holeTags;
    for(dq_holes::const_iterator i= holes.begin(); i!= holes.end(); i++)
      {
	const PolygonalFace *hole= *i;
	holeTags.append(hole->getTag());
      }
    retval["holeTags"]= holeTags;
    retval["hole"]= this->hole;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::Face::setPyDict(const boost::python::dict &d)
  {
    CmbEdge::setPyDict(d);
    // The topology can't be updated here because the bodies doesn't exist
    // yet (they are readed after the points). The faceTags field is ignored.
    //boost::python::list bodyTags= boost::python::extract<boost::python::list>(d["bodyTags"]);
    Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
	MultiBlockTopology &mbt= preprocessor->getMultiBlockTopology();

	this->ndivj=  boost::python::extract<size_t>(d["ndivj"]);
	boost::python::list holeTags= boost::python::extract<boost::python::list>(d["holeTags"]);
	const size_t numHoles= boost::python::len(holeTags);

	SurfaceMap &surfaces= mbt.getSurfaces();
	for(size_t i= 0; i<numHoles; i++)
	  {
	    const size_t holeTag= boost::python::extract<size_t>(holeTags[i]);
	    Face *f= surfaces.busca(holeTag);
	    PolygonalFace *pf= dynamic_cast<PolygonalFace *>(f);
	    this->holes.push_back(pf);
	  }
	this->update_topology();
      }
    else
      std::cerr << Color::red << getClassName() << __FUNCTION__
	        << "; preprocessor needed."
		<< Color::def << std::endl;
    this->hole= boost::python::extract<bool>(d["hole"]);
  }

