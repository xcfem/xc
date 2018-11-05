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
//Body.cc

#include "Body.h"
#include "Pnt.h"
#include "xc_utils/src/matrices/m_int.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "Face.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"
#include "xc_utils/src/geom/pos_vec/Pos3dArray.h"

//! @brief Constructor
XC::SideSequence::SideSequence(const size_t first,const bool &_forward)
  : forward(_forward)
  {
    if(forward)
      {
        l1= (first-1)%4+1;
        l2= (first)%4+1;
        l3= (first+1)%4+1;
        l4= (first+2)%4+1;
      }
    else
      {
        l1= (first+3)%4+1;
        l2= (first+2)%4+1;
        l3= (first+1)%4+1;
        l4= (first)%4+1;
      }
  }

//! @brief Constructor.
XC::Body::BodyFace::BodyFace(Body *owr, Face *face_ptr,const size_t &p,const bool &d)
  : CommandEntity(owr), surface(face_ptr), sec_lados(p,d) {}

//! @brief Return a pointer to the surface that limits the solid.
XC::Face *XC::Body::BodyFace::Surface(void)
  { return surface; }
//! @brief Return a pointer to the surface that limits the solid.
const XC::Face *XC::Body::BodyFace::Surface(void) const
  { return surface; }
//! @brief Set the surface that limites the solid.
void XC::Body::BodyFace::SetSurf(XC::Face *s)
  { surface= s; }
//! @brief Return the name of the surface that limits the solid.
const std::string &XC::Body::BodyFace::getName(void) const
  { return surface->getName(); }
//! @brief Return true if the surface doesn't exists.
bool XC::Body::BodyFace::Vacia(void) const
  { return (surface==nullptr); }
//! @brief Return the number of edges on the surface.
size_t XC::Body::BodyFace::getNumberOfLines(void) const
  { return surface->getNumberOfEdges(); }
//! @brief Return the number of vertices on the surface.
size_t XC::Body::BodyFace::getNumberOfVertices(void) const
  { return surface->getNumberOfVertices(); }

//! @brief Return the pointer to the side face whose index is passed as parameter.
const XC::CmbEdge::Side *XC::Body::BodyFace::getSide(const size_t &i) const
  {
    if(!surface) return nullptr;
    const CmbEdge::Side *retval(nullptr);
    const size_t idx= (i-1)%4+1;
    switch(idx)
      {
        case 1:
          retval= surface->getSide(sec_lados.l1);
          break;
        case 2:
          retval= surface->getSide(sec_lados.l2);
          break;
        case 3:
          retval= surface->getSide(sec_lados.l3);
          break;
        case 4:
          retval= surface->getSide(sec_lados.l4);
          break;
        default:
          retval= nullptr;
          break;
      }
    return retval;
  }

//! @brief Return a pointer to the i-th edge of the face.
XC::CmbEdge::Side *XC::Body::BodyFace::getSide(const size_t &i)
  { return const_cast<CmbEdge::Side *>(static_cast<const BodyFace &>(*this).getSide(i)); }

//! @brief Return a pointer to the i-th vertex of the face.
const XC::Pnt *XC::Body::BodyFace::getVertex(const size_t &i) const
  {
    const CmbEdge::Side *l= getSide(i);
    if(l)
      {
        if(sec_lados.isDirect())
          return l->P1();
        else
          return l->P2();
      }
    return nullptr;
  }

//! @brief Return a pointer to the i-th vertex of the face.
XC::Pnt *XC::Body::BodyFace::getVertex(const size_t &i)
  { return const_cast<Pnt *>(static_cast<const BodyFace &>(*this).getVertex(i)); }

//! @brief Return a pointer to the (i,j) node of the face.
XC::Node *XC::Body::BodyFace::getNode(const size_t &i,const size_t &j)
  {
    assert(surface);
    return surface->getNode(i,j);
  }

//! @brief Return the positions for the nodes on a face.
Pos3dArray XC::Body::BodyFace::get_positions(void) const
  {
    if(!surface)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; pointer to surface is null." << std::endl;
        return Pos3dArray(); 
      }
    const int numEdges= surface->getNumberOfEdges();
    if(numEdges!=4)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; can't mesh " << numEdges
	          << " edges surfaces." << std::endl;
        return Pos3dArray();
      }
    const CmbEdge::Side *l1= getSide(1);
    const CmbEdge::Side *l2= getSide(2);
    const CmbEdge::Side *l3= getSide(3);
    const CmbEdge::Side *l4= getSide(4);
    //Rows of points quasi-parllels to l2 and l4 and increasing index from l4 to l2.
    //Columns of point quasi.parallels to l1 and l3 and increasing index from l1 to l3.
    if(l1->NDiv()!=l3->NDiv())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; lines 1 and 3 have different number of divisions. "
	          << "Can't generate mesh." << std::endl;
        return Pos3dArray();
      }
    if(l2->NDiv()!=l4->NDiv())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; lines 2 and 4 have different number of divisions. "
	          << "Can't generate mesh." << std::endl;
        return Pos3dArray();
      }
    Pos3dArray ptos_l1= l1->getNodePosForward();
    Pos3dArray ptos_l2= l2->getNodePosForward();
    Pos3dArray ptos_l3= l3->getNodePosReverse(); //Reverse order.
    Pos3dArray ptos_l4= l4->getNodePosReverse(); //Reverse order.
    return Pos3dArray(ptos_l1,ptos_l2,ptos_l3,ptos_l4);
  }

//! @brief Check the number of divisions.
bool XC::Body::BodyFace::checkNDivs(void) const
  { return surface->checkNDivs(); }

//! @brief Trigger the creation of nodes on faces.
void XC::Body::BodyFace::create_nodes(void)
  {
    if(surface)
      surface->create_nodes();
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; pointer to surface is null." << std::endl;
  }

//! @brief Constructor.
XC::Body::Body(Preprocessor *m,const std::string &nombre)
  : EntMdlr(nombre,0,m) {}

//! @brief Returns object BND.
BND3d XC::Body::Bnd(void) const
  { 
    BND3d retval;
    const size_t nv= getNumberOfVertices();
    if(nv<1) //the set is empty.
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; body is empty." << std::endl;
      }
    else
      {
        retval= BND3d(getVertex(1)->GetPos(),getVertex(1)->GetPos());
	if(nv<2)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; body has only one point." << std::endl;
	  }
	else
	  {
	    for(size_t i=2;i<=nv;i++)
	      retval+= getVertex(i)->GetPos();
	  }
      }
    return retval;
  }

//! @brief Returns true if this object lies inside the
//! geometric object.
bool XC::Body::In(const GeomObj3d &geomObj, const double &tol) const
  {
    bool retval= false;
    const size_t nv= getNumberOfVertices();
    if(nv<1) //the set is empty.
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; body is empty." << std::endl;
      }
    else if(nv<2)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; body has only one point." << std::endl;
	retval= geomObj.In(getVertex(1)->GetPos(),tol);
      }
    else
      for(size_t i=1;i<=nv;i++)
	{
	  Pos3d pos(getVertex(i)->GetPos());
          if(!geomObj.In(pos,tol))
            { retval= false; break; }
	}
    return retval;
  }

//! @brief Returns true if this object lies outside the
//! geometric object.
bool XC::Body::Out(const GeomObj3d &geomObj, const double &tol) const
  { return !In(geomObj,tol); }

//! @brief Return the bodies that touch the surface
//! passed as parameter (neighbors).
std::set<const XC::Body *> XC::getConnectedBodies(const Face &s)
  { return s.getConnectedBodies(); }

//! @brief Return the sets to wich the body belongs.
std::set<XC::SetBase *> XC::Body::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; preprocessor needed." << std::endl;
    return retval;
  }

//! @brief Add the body to the set passed as parameter.
void XC::Body::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        Set *s= dynamic_cast<Set *>(*i);
        if(s) s->getBodies().push_back(this);
      }
  }

//! @brief Unpdate topology of the surface passed as parameter (inserts neighbor).
void XC::Body::set_surf(Face *s)
  {
    if(s)
     { s->insert_body(this); }
  }

//! Return indices of the vertices.
std::vector<int> XC::Body::getIndicesVertices(void) const
  {
    const size_t nv= getNumberOfVertices();
    std::vector<int> retval(nv);
    if(nv>=1)
      {
        for(size_t i=0;i<nv;i++)
          retval[i]= getVertex(i+1)->getIdx();
      }
    return retval;
  }
