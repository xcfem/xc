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
//Edge.cc

#include "Edge.h"
#include "preprocessor/multi_block_topology/entities/2d/Face.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "utility/matrix/util_matrix.h"
#include "utility/matrices/m_int.h"
#include "utility/geom/d3/BND3d.h"
#include "utility/geom/pos_vec/Pos3dArray.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"

#include <limits>
#include "boost/any.hpp"

//! @brief Constructor.
//! 
//! @param nd: Number of divisions.
XC::Edge::Edge(Preprocessor *m,const size_t &nd)
  : EntMdlr(m), ndiv(nd) {}

//! @brief Constructor.
//! 
//! @param name: object identifier.
//! @param m: pointer to preprocessor.
//! @param nd: number of divisions.
XC::Edge::Edge(const std::string &name,Preprocessor *m,const size_t &nd)
  : EntMdlr(name,0,m), ndiv(nd) {}

//! @brief Comparison operator.
bool XC::Edge::operator==(const Edge &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= EntMdlr::operator==(other);
        if(retval)
          retval= (ndiv==other.ndiv);
       } 
    return retval;
  }

//! @brief Insert a surface in contact with the line (neighbour).
//! @param s: surface to insert.
void XC::Edge::insert_surf(Face *s)
  { surfaces_line.insert(s); }

//! @brief Update topology.
void XC::Edge::update_topology(void)
  {
    if(P1()) P1()->insert_line(this);
    if(P2()) P2()->insert_line(this);
  }

//! @brief Return a pointer to the edge's start point.
XC::Pnt *XC::Edge::P1(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; this function must be redefined on"
              << " derived classes, nullptr returned;" << std::endl;
    return nullptr;
  }

//! @brief Return a constant pointer to the edge's start point.
const XC::Pnt *XC::Edge::P1(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; this function must be redefined on"
              << " derived classes, nullptr returned;" << std::endl;
    return nullptr;
  }

//! @brief Return a pointer to the edge's start point.
XC::Pnt *XC::Edge::P2(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; this function must be redefined on"
              << " derived classes, nullptr returned;" << std::endl;
    return nullptr;
  }

//! @brief Return a constant pointer to the edge's end point.
const XC::Pnt *XC::Edge::P2(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; this function must be redefined on"
              << " derived classes, nullptr returned;" << std::endl;
    return nullptr;
  }

//! @brief Return the centroid of the edge.
Pos3d XC::Edge::getCentroid(void) const
  {
    Pos3d retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; this function must be redefined on"
              << " derived classes, meaningless value returned;"
	      << std::endl;
    return retval;
  }

//! @brief Returns a vector in the direction of the local
//! X axis.
Vector3d XC::Edge::getIVector(void) const
  {
    Vector3d retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; this function must be redefined on"
              << " derived classes, meaningless value returned;"
	      << std::endl;
    return retval;
  }

//! @brief Returns a vector in the direction of the local
//! Y axis.
Vector3d XC::Edge::getJVector(void) const
  {
    Vector3d retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; this function must be redefined on"
              << " derived classes, meaningless value returned;"
	      << std::endl;
    return retval;
  }

//! @brief Returns a vector in the direction of the local
//! Z axis
Vector3d XC::Edge::getKVector(void) const
  {
    const Vector3d vI= getIVector();
    const Vector3d vJ= getJVector();
    return vI.getCross(vJ);
  }

//! @brief Returns a matrix with the axes of the surface as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
XC::Matrix XC::Edge::getLocalAxes(void) const
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

//! @brief Returns true if the points passed as parameters
//! are the ends of the edge.
//!
//! @param p1: first point.
//! @param p2: second point.
bool XC::Edge::areEndPoints(const Pnt *p1,const Pnt *p2) const
  {
    bool retval= false;
    const Pnt *e1= P1();
    const Pnt *e2= P2();
    if(p1==e1)
      retval= (p2==e2);
    else if(p1==e2)
      retval= (p2==e1);
    return retval;
  }

//! @brief Returns true if the line lies inside the
//! geometric object.
//!
//! @param geomObj: object to be contained in.
//! @param tol: tolerance.
bool XC::Edge::In(const GeomObj3d &geomObj, const double &tol) const
  {
    bool retval= false;
    const Pnt *p1= P1();
    const Pnt *p2= P2();
    if(p1)
      retval= p1->In(geomObj,tol);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; p1 not defined." << std::endl;
    if(p2)
      retval= retval && p2->In(geomObj,tol);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; p2 not defined." << std::endl;
    return retval;
  }

//! @brief Returns true if the line lies outside the
//! geometric object.
//!
//! @param geomObj: object to be contained in.
//! @param tol: tolerance.
bool XC::Edge::Out(const GeomObj3d &geomObj, const double &tol) const
  {
    bool retval= false;
    const Pnt *p1= P1();
    const Pnt *p2= P2();
    if(p1)
      retval= p1->Out(geomObj,tol);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; p1 not defined." << std::endl;
    if(p2)
      retval= retval && p2->Out(geomObj,tol);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; p2 not defined." << std::endl;
    return retval;
  }


//! @brief Assigns the number of divisions of this
//! edge and all its homologous.
//!
//! @param nd: number of divisions.
void XC::Edge::setNDivHomologousEdges(const size_t &nd)
  {
    std::set<const XC::Edge *> homologous= getHomologousSides();
    for(std::set<const Edge *>::const_iterator i= homologous.begin();i!=homologous.end();i++)
      {
        Edge *tmp= const_cast<Edge *>(*i);
        tmp->ndiv= nd;
      }
    ndiv= nd;
  }

//! @brief Assigns the number of of divisions if it is
//! compatible with the homologous edges.
//!
//! @param nd: number of divisions.
void XC::Edge::setNDiv(const size_t &nd)
  {
    if(ndiv!=nd) //If number of divisions changes
      {
        if(ttzNodes.empty()) //Not meshed.
          {
            // const size_t ns= surfaces_line.size();
            // if(ns>1)
            //   {
	    //     // std::clog << getClassName() << "::" << __FUNCTION__
	    // 	//           << "; " << getName()
            //     //           << " is an edge of the surfaces: "
            //     //           << getConnectedSurfacesNames()
            //     //           << ". Number of divisions"
            //     //           << " was " << ndiv
            //     //           << " we adopt: " << nd << std::endl;
            //     ndiv= std::max(ndiv,nd);
            //   }
            // else
            ndiv= nd;
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; " << getName()
                    << " edge is already meshed"
	            << " and keeps its number of divisions"
                    << " ndiv= " << ndiv << ".\n";
      }
  }

//! @brief Compute the number of divisions necessary to 
//! get the element size passed as parameter.
//!
//! @param sz: size of the element.
void XC::Edge::SetElemSize(const double &sz)
  {
    const double l= getLength();
    const size_t n= ceil(l/sz);
    setNDiv(n);
  }

//! @brief Return the element size (aka L/ndiv).
double XC::Edge::getElemSize(void) const
  { return getLength()/ndiv; }


//! @brief Create points along the line.
void XC::Edge::divide(void)
  {
    const Pos3dArray positions= get_positions();
    create_points(positions);
  }

//! @brief Divides the line by the point being passed as parameter.
XC::Edge *XC::Edge::splitAtPoint(Pnt *p)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << std::endl;
    return nullptr;
  }

//! @brief Divides the line by the point obtained by: p1+lambda*VDir().
XC::Edge *XC::Edge::splitAtLambda(const double &)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << std::endl;
    return nullptr;
  }

//! @brief Divides the line by the point obtained by: p1+lambda*VDir().
XC::Edge *XC::Edge::splitAtNaturalCoord(const double &)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << std::endl;
    return nullptr;
  }


//! @brief Divides the edge at the point argument. Returns the new
//! Edge.
XC::Edge *XC::Edge::splitAtPos3d(const Pos3d &p, const double &tol)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << std::endl;
    return nullptr;
  }


//! @brief Return a pointer to node whose indices are passed as parameters.
//!
//! @param i: index of the layer.
//! @param j: index of the row.
//! @param k: index of the column.
XC::Node *XC::Edge::getNode(const size_t &i,const size_t &j,const size_t &k)
  { return EntMdlr::getNode(i,j,k); }

//! @brief Return a pointer to node whose indices are passed as parameters.
//!
//! @param i: index of the layer.
//! @param j: index of the row.
//! @param k: index of the column.
const XC::Node *XC::Edge::getNode(const size_t &i,const size_t &j,const size_t &k) const
  { return EntMdlr::getNode(i,j,k); }

//! @brief Return a pointer to node whose index is passed as parameter.
//!
//! @param i: index of the layer.
XC::Node *XC::Edge::getNode(const size_t &i)
  {  return const_cast<Node *>(static_cast<const Edge &>(*this).getNode(i)); }

//! @brief Return a pointer to node whose index is passed as parameter.
//!
//! @param i: index of the layer.
const XC::Node *XC::Edge::getNode(const size_t &i) const
  {
    Node *retval= nullptr;
    if(!ttzNodes.Null())
      retval= ttzNodes.getAtI(i);
    return retval;
  }

//! @brief Returns the node whose ordinal index is passed as
//! parameter, starting from the beginning.
//!
//! @param i: index of the layer.
XC::Node *XC::Edge::getNodeForward(const size_t &i)
  { return getNode(i); }

//! @brief Return the node whose ordinal index is passed as parameter,
//! starting from the beginning.
//!
//! @param i: index of the layer.
const XC::Node *XC::Edge::getNodeForward(const size_t &i) const
  { return getNode(i); }

//! @brief Return the node whose ordinal index is passed as parameter,
//! starting from the end.
//!
//! @param i: index of the layer.
XC::Node *XC::Edge::getNodeReverse(const size_t &i)
  { return getNode(getNumberOfNodes()-i+1); }

//! @brief Return the node whose ordinal index is passed as parameter,
//! starting from the end.
//!
//! @param i: index of the layer.
const XC::Node *XC::Edge::getNodeReverse(const size_t &i) const
  { return getNode(getNumberOfNodes()-i+1); }

//! @brief Return the first node of the line.
const XC::Node *XC::Edge::getFirstNode(void) const
  { 
    Edge *this_no_const= const_cast<Edge *>(this);
    const Node *retval= this_no_const->getFirstNode();
    return retval;
  }

//! @brief Return the first node of the line.
XC::Node *XC::Edge::getFirstNode(void)
  {
    Node *retval= getNode(1);
    if(!retval)
      {
        Pnt *p1= P1();
        if(p1)
          retval= p1->getNode();
      }
    return retval;
  }

//! @brief Return the last node of the line.
const XC::Node *XC::Edge::getLastNode(void) const
  { 
    Edge *this_no_const= const_cast<Edge *>(this);
    const Node *retval= this_no_const->getLastNode();
    return retval;
  }

//! @brief Return the last node of the line.
XC::Node *XC::Edge::getLastNode(void)
  { 
    Node *retval= getNodeReverse(1);
    if(!retval)
      {
        Pnt *p2= P2();
        if(p2)
          retval= p2->getNode();
      }
    return retval;
  }

//! @brief Return the node pointers in forward order.
std::vector<XC::Node *> XC::Edge::getNodesForward(void)
  {
    const size_t nn= getNumberOfNodes();
    std::vector<Node *> retval(nn, nullptr);
    for(size_t i=1;i<=nn;i++)
      retval[i-1]= this->getNode(i);
    return retval;
  }

//! @brief Return the node pointersin reverse order.
std::vector<XC::Node *> XC::Edge::getNodesReverse(void)
  {
    const size_t nn= getNumberOfNodes();
    std::vector<Node *> retval(nn, nullptr);
    for(size_t i=1;i<=nn;i++)
      retval[i-1]= this->getNode(nn-i+1);
    return retval;
  }

//! @brief Return the node pointers in forward order.
std::vector<const XC::Node *> XC::Edge::getNodesForward(void) const
  {
    const size_t nn= getNumberOfNodes();
    std::vector<const Node *> retval(nn, nullptr);
    for(size_t i=1;i<=nn;i++)
      retval[i-1]= this->getNode(i);
    return retval;
  }

//! @brief Return the node pointersin reverse order.
std::vector<const XC::Node *> XC::Edge::getNodesReverse(void) const
  {
    const size_t nn= getNumberOfNodes();
    std::vector<const Node *> retval(nn, nullptr);
    for(size_t i=1;i<=nn;i++)
      retval[i-1]= this->getNode(nn-i+1);
    return retval;
  }

//! @brief Return the IDs of the nodes in forward order.
std::vector<int> XC::Edge::getTagsNodesForward(void) const
  {
    const std::vector<const Node *> tmp= this->getNodesForward();
    std::vector<int> retval(tmp.size());
    size_t j= 0;
    for(std::vector<const Node *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++, j++)
      retval[j]= (*i)->getTag();
    return retval;
  }

//! @brief Return the IDs of the nodes in reverse order.
std::vector<int> XC::Edge::getTagsNodesReverse(void) const
  {
    const std::vector<const Node *> tmp= this->getNodesReverse();
    std::vector<int> retval(tmp.size());
    size_t j= 0;
    for(std::vector<const Node *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++, j++)
      retval[j]= (*i)->getTag();
    return retval;
  }

//! @brief Return the positions of the nodes in forward order.
Pos3dArray XC::Edge::getNodePosForward(void) const
  {
    const std::vector<const Node *> tmp= this->getNodesForward();
    Pos3dArray retval(tmp.size());
    size_t j= 1;
    for(std::vector<const Node *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++, j++)
      retval(j,1)= pos_node(**i);
    return retval;
  }

//! @brief Return the positions of the nodes in reverse order.
Pos3dArray XC::Edge::getNodePosReverse(void) const
  {
    const std::vector<const Node *> tmp= this->getNodesReverse();
    Pos3dArray retval(tmp.size());
    size_t j= 1;
    for(std::vector<const Node *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++, j++)
      retval(j,1)= pos_node(**i);
    return retval;
  }

//! @brief Return the number of edges connected to this one.
size_t XC::Edge::getNumConnectedEdges(void) const
  {
    size_t retval= 0;
    const Pnt *p1= P1();
    if(p1)
      retval+= p1->getNumConnectedEdges();
    const Pnt *p2= P2();
    if(p2)
      retval+= p2->getNumConnectedEdges();
    return retval;
  }

//! @brief Return the number of edges connected to this one that
//! belong to the given set.
size_t XC::Edge::getNumConnectedEdges(const SetBase *s) const
  {
    size_t retval= 0;
    const Pnt *p1= P1();
    if(p1)
      retval+= p1->getNumConnectedEdges(s);
    const Pnt *p2= P2();
    if(p2)
      retval+= p2->getNumConnectedEdges(s);
    return retval;
  }

//! @brief Return the edges connected to this one.
std::set<const XC::Edge *> XC::Edge::getConnectedEdges(void) const
  {
    std::set<const Edge *> retval;
    const Pnt *p1= P1();
    if(p1)
      {
	const std::set<const Edge *> &edges_p1= p1->getConnectedEdges();
	retval.insert(edges_p1.begin(), edges_p1.end());
      }
    const Pnt *p2= P2();
    if(p2)
      {
	const std::set<const Edge *> &edges_p2= p2->getConnectedEdges();
	retval.insert(edges_p2.begin(), edges_p2.end());
      }
    return retval;
  }

//! @brief Return the edges connected to this one that belong to the given
//! set.
std::set<const XC::Edge *> XC::Edge::getConnectedEdges(const SetBase *s) const
  {
    std::set<const Edge *> retval;
    const Pnt *p1= P1();
    if(p1)
      {
	const std::set<const Edge *> &edges_p1= p1->getConnectedEdges(s);
	retval.insert(edges_p1.begin(), edges_p1.end());
      }
    const Pnt *p2= P2();
    if(p2)
      {
	const std::set<const Edge *> &edges_p2= p2->getConnectedEdges(s);
	retval.insert(edges_p2.begin(), edges_p2.end());
      }
    return retval;
  }

//! @brief Return the edges connected to this in a Python list.
boost::python::list XC::Edge::getConnectedEdgesPy(void) const
  {
    const std::set<const Edge *> tmp= this->getConnectedEdges();
    boost::python::list retval;
    for(std::set<const Edge *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	const Edge *pEdge= *i;	
        boost::python::object pyObj(boost::ref(*pEdge));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Return the edges connected to this one that belong to
//! the given set.
boost::python::list XC::Edge::getConnectedEdgesPy(const SetBase *s) const
  {
    const std::set<const Edge *> tmp= this->getConnectedEdges(s);
    boost::python::list retval;
    for(std::set<const Edge *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	const Edge *pEdge= *i;	
        boost::python::object pyObj(boost::ref(*pEdge));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Return the number of surfaces that touch the line.
size_t XC::Edge::getNumConnectedSurfaces(void) const
  { return surfaces_line.size(); }

//! @brief Return the number of surfaces that touch the line and
//! belong to the given set.
size_t XC::Edge::getNumConnectedSurfaces(const SetBase *s) const
  {
    size_t retval= 0;
    if(s)
      {
	if(!this->surfaces_line.empty())
	  {
	    std::set<const Face *>::const_iterator i= this->surfaces_line.begin();
	    for(;i!=this->surfaces_line.end();i++)
	      {
		const Face *f= *i;
		if(s->In(f))
		  retval++;
	      }
	  }
      }
    else
      retval= this->getNumConnectedSurfaces();
    return retval;
  }

//! @brief Return the surfaces that touch the line.
const std::set<const XC::Face *> &XC::Edge::getConnectedSurfaces(void) const
  { return surfaces_line; }

//! @brief Get the surfaces connected to this one that belong to the
//! given set.
std::set<const XC::Face *> XC::Edge::getConnectedSurfaces(const SetBase *s) const
  {
    std::set<const Face *> retval;
    if(s)
      {
	if(!this->surfaces_line.empty())
	  {
	    std::set<const Face *>::const_iterator i= this->surfaces_line.begin();
	    for(;i!=this->surfaces_line.end();i++)
	      {
		const Face *f= *i;
		if(s->In(f))
		  retval.insert(f);
	      }
	  }
      }
    else
      retval= this->getConnectedSurfaces();
    return retval;
  }

//! @brief Returns the faces from the given set that share this edge.
std::set<const XC::Face *> XC::Edge::getConnectedSurfaces(const DqPtrsFaces &faceSet) const
  {
    std::set<const XC::Face *> retval;
    std::set<const XC::Face *> tmp= this->getConnectedSurfaces();
    for(std::set<const XC::Face *>::const_iterator i= tmp.begin();i!=tmp.end();i++)
      {
	const Face *facePtr= faceSet.findTag((*i)->getTag());
	if(facePtr)
	  retval.insert(facePtr);
      }
    return retval;
  }

//! @brief Return the surfaces that touch this point (neighbors).
boost::python::list XC::Edge::getConnectedSurfacesPy(void) const
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

//! @brief Return the surfaces that touch this point and belong to
//! the given set.
boost::python::list XC::Edge::getConnectedSurfacesPy(const SetBase *s) const
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

//! @brief Return names of the surfaces that touch the line (neighbors).
const std::string &XC::Edge::getConnectedSurfacesNames(void) const
  {
    static std::string retval;
    retval= "";
    if(!surfaces_line.empty())
      {
        std::set<const Face *>::const_iterator i= surfaces_line.begin();
        retval+= (*i)->getName();
        i++;
        for(;i!=surfaces_line.end();i++)
          retval+= "," + (*i)->getName();
      }
    return retval;
  }

//! @brief Return the homologous sides to that passed as a parameter.
std::set<const XC::Edge *> XC::Edge::getHomologousSides(void) const
  {
    const Preprocessor *preprocessor= getPreprocessor();
    const MultiBlockTopology &mbt= preprocessor->getMultiBlockTopology();
    std::set<const Edge *> retval= mbt.getHomologousSides(this);
    return retval;
  }

//! @brief Compute the number of divisions for each line
//! to make it compatible with adjacent surface meshing.
size_t XC::calcula_ndiv_lados(const std::set<const XC::Edge *> &lados)
  {
    size_t retval= 0;
    if(!lados.empty())
      {
        std::set<const XC::Edge *>::const_iterator i= lados.begin();
        retval= (*i)->NDiv();
        i++;
        for(;i!=lados.end();i++)
	  { retval= std::max(retval,(*i)->NDiv()); }
      }
    return retval;
  }

//! @brief Return true if the line touches the surface (neighbor).
bool XC::Edge::isConnectedTo(const Face &s) const
  {
    std::set<const Face *>::const_iterator i= surfaces_line.find(&s);
    bool retval= i!=surfaces_line.end();
    return retval;
  }

//! @brief Return true if the line touches the body.
bool XC::Edge::isConnectedTo(const Body &b) const
  {
    for(std::set<const Face *>::const_iterator i= surfaces_line.begin(); i!=surfaces_line.end();i++)
      { if((*i)->isConnectedTo(b)) return true; }
    return false;
  }

//! @brief Return true if it is an end point of the line.
bool XC::Edge::isEndPoint(const Pnt &p) const
  { return ((&p == P1()) || (&p == P2()));  }

//! @brief Return the list of edges that have this point as starting or ending point.
std::set<const XC::Edge *> XC::get_connected_lines(const Pnt &p)
  { return p.getConnectedEdges(); }

//! @brief Return a matrix of positions along the line.
Pos3dArray XC::Edge::get_nodes_pos(void) const
  { return get_positions(); }

//! @brief Create the nodes for both end points of the edge.
void XC::Edge::create_nodes_on_endpoints(void)
  {
    if(verbosity>4)
      std::clog << "Creating nodes for '" << getName() << "' edge ends...";   
    if(!P1())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; start point undefined." << std::endl;
        return;
      }
    else
      P1()->create_nodes();

    Node *node_p1= P1()->getNode();
    assert(node_p1);
    ttzNodes(1,1,1)= node_p1; //Node of the start point.
      
    if(!P2())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; end point undefined." << std::endl;
        return;
      }
    else
      P2()->create_nodes();

    Node *node_p2= P2()->getNode();
    assert(node_p2);
    const size_t n_rows= ttzNodes.getNumberOfRows();
    const size_t n_cols= ttzNodes.getNumberOfColumns();
    ttzNodes(1,n_rows,n_cols)= node_p2; //Node of the end point.

    if(verbosity>4)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; created." << std::endl;
  }

//! @brief Creates interior nodes at the positions being passed as parameters.
std::vector<XC::Node *> XC::Edge::create_interior_nodes(const std::vector<Pos3d> &positions)
  {
    const size_t sz= positions.size();
    std::vector<XC::Node *> retval(sz,nullptr);
    if(ttzNodes.Null())
      {
	ttzNodes= NodePtrArray3d(1,1,sz+2); //interior nodes + 2 end points.
	if(getPreprocessor())
	  {
            create_nodes_on_endpoints();
	    for(size_t i=0;i<sz;i++)
	      retval[i]= create_node(positions[i],1,1,i+2);
	    if(verbosity>5)
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; created " << ttzNodes.NumPtrs() << " node(s)."
			<< std::endl;
	  }
      }
    else
      if(verbosity>2)
	std::clog << getClassName() << "::" << __FUNCTION__
		  << "; nodes from entity: '" << getName()
		  << "' already exist." << std::endl;
    return retval;
  }

//! @brief Create nodes on the positions argument.
void XC::Edge::create_nodes(const Pos3dArray &positions)
  {
    if(verbosity>4)
      std::clog << "Creating nodes for edge: '" << getName() << "'...";

 
    if(ttzNodes.Null())
      {
        if(!getPreprocessor())
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; preprocessor undefined." << std::endl;
        else
          {
            const size_t n_rows= positions.getNumberOfRows();
            const size_t n_cols= positions.getNumberOfColumns();
            ttzNodes= NodePtrArray3d(1,n_rows,n_cols);

            create_nodes_on_endpoints();

            if((n_rows*n_cols)>2) //If it has intermediate nodes...
              {
                const size_t end_row= std::max(n_rows-1,size_t(1));
                const size_t begin_row= (end_row == 1 ? 1 : 2);
                const size_t end_column= std::max(n_cols-1,size_t(1));
                const size_t begin_column= (end_column == 1 ? 1 : 2);
                for( size_t j= begin_row;j<=end_row;j++)
                  for( size_t k= begin_column;k<=end_column;k++)
                    create_node(positions(j,k),1,j,k);
              }
          }
        if(verbosity>4)
	  std::clog << getClassName() << "::" << __FUNCTION__
	            << "; created " << ttzNodes.NumPtrs()
		    << " node(s)." << std::endl;
      }
    else
      {
        if(verbosity>2)
          std::clog << getClassName() << "::" << __FUNCTION__
	            << "; nodes of the line: '" << getName() << "' already exist." << std::endl;
      }
    if(verbosity>4)
      std::clog << "created." << std::endl;
    const size_t numNodes= ttzNodes.NumPtrs();
    if(numNodes==0 && verbosity>0)
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; warning 0 nodes created for line: " << getName()
	        << std::endl;
  }

//! @brief Create nodes on objects.
void XC::Edge::create_nodes(void)
  {
    const Pos3dArray positions= get_nodes_pos();
    create_nodes(positions);
  }

//! @brief Trigger mesh generation.
void XC::Edge::genMesh(meshing_dir dm)
  {
    if(verbosity>3)
      std::clog << "Meshing edge...(" << getName() << ")...";   
    create_nodes();
    create_elements(dm);
    if(verbosity>3)
      std::clog << "done." << std::endl;
  }

//! @brief Ask Gmsh to create the line corresponding to this
//! edge.
int XC::Edge::create_gmsh_line(void) const
  {
    int retval= -1;
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Return a vector tangent to the line in point at parameter s.
const XC::Vector &XC::Edge::getTang(const double &s) const
  {
    static Vector retval(1);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Return the sets to which this edge belongs.
std::set<XC::SetBase *> XC::Edge::get_sets(void) const
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

//! @brief Add the «edge» to the sets passed as parameters.
void XC::Edge::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        Set *s= dynamic_cast<Set *>(*i);
        if(s) s->getLines().push_back(this);
      }
  }

//! @brief Return indices of the vertices.
std::vector<int> XC::Edge::getIndicesVertices(void) const
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

//! @brief Return the segments that compose the edge.
std::deque<Segment3d> XC::Edge::getSegments(void) const
  {
    const size_t nv= getNumberOfVertices(); // Number of vertices.
    const size_t ns= nv-1; // Number of segments.
    std::deque<Segment3d> retval(ns);
    Edge *this_no_const= const_cast<Edge *>(this);
    if(nv>=1)
      {
        for(size_t i=0;i<ns;i++)
	  {
	    const Pos3d &p0= getVertex(i)->getPos();
	    const Pos3d &p1= getVertex(i+1)->getPos();
	    Segment3d s(p0,p1);
	    s.set_owner(this_no_const);
	    retval[i]= s;
	  }
      }
    return retval;
  }

//! @brief Return the segments in a Python list.
boost::python::list XC::Edge::getSegmentsPy(void) const
  {
    boost::python::list retval;
    std::deque<Segment3d> tmpList= getSegments();
    for(std::deque<Segment3d>::const_iterator i= tmpList.begin();i!=tmpList.end();i++)
      {
	Segment3d tmp= *i;
	boost::python::object pyObj(tmp);
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Return the distance to the edge.
double XC::Edge::getDist(const Pos3d &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; this function must be redefined on"
              << " derived classes, NaN returned;" << std::endl;
    return std::numeric_limits<double>::quiet_NaN();
  }

//! @brief Return k-points.
XC::ID XC::Edge::getKPoints(void) const
  {
    ID retval(2);
    retval[0]= P1()->getTag();
    retval[1]= P2()->getTag();
    return retval;
  }

//! @brief Return the vertices.
std::deque<XC::Pnt *> XC::Edge::getVertices(void)
  {
    std::deque<Pnt *> retval(2);
    retval[0]= P1();
    retval[1]= P2();
    return retval;
  }

//! @brief Return the vertices.
std::deque<const XC::Pnt *> XC::Edge::getVertices(void) const
  {
    std::deque<const Pnt *> retval(2);
    retval[0]= P1();
    retval[1]= P2();
    return retval;
  }

//! @brief Return the vertices in a Python list.
boost::python::list XC::Edge::getVerticesPy(void)
  {
    boost::python::list retval;
    std::deque<Pnt *> tmp= getVertices();
    for(std::deque<Pnt *>::iterator i= tmp.begin();i!=tmp.end();i++)
      {
	Pnt *tmp= *i;
	boost::python::object pyObj(boost::ref(*tmp));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::Edge::getPyDict(void) const
  {
    boost::python::dict retval= EntMdlr::getPyDict();
    retval["ndiv"]= ndiv; 
    boost::python::list faceTags;
    for(std::set<const Face *>::const_iterator i= surfaces_line.begin(); i!=surfaces_line.end(); i++)
      {
	const Face *f= *i;
        faceTags.append(f->getTag());
      }
    retval["faceTags"]= faceTags;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::Edge::setPyDict(const boost::python::dict &d)
  {
    EntMdlr::setPyDict(d);
    ndiv= boost::python::extract<size_t>(d["ndiv"]);
    // The topology can't be updated here because the faces doesn't exist
    // yet (they are readed after the lines). The faceTags field is ignored.
    // boost::python::list faceTags= boost::python::extract<boost::python::list>(d["faceTags"]);
  }

