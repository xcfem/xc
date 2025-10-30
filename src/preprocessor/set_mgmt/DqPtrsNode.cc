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

#include "DqPtrsNode.h"
#include "domain/mesh/node/Node.h"
#include "domain/load/NodalLoad.h"
#include "preprocessor/multi_block_topology/trf/TrfGeom.h"
#include "utility/functions/algebra/ExprAlgebra.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/d3/BND3d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/geom/lists/utils_list_pos3d.h"
#include "utility/geom/coo_sys/ref_sys/PrincipalAxes3D.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::DqPtrsNode::DqPtrsNode(CommandEntity *owr)
  : DqPtrsKDTree<Node,KDTreeNodes>(owr) {}

//! @brief Copy constructor.
XC::DqPtrsNode::DqPtrsNode(const DqPtrsNode &other)
  : DqPtrsKDTree<Node,KDTreeNodes>(other)
  { }

//! @brief Copy constructor.
XC::DqPtrsNode::DqPtrsNode(const std::deque<Node *> &ts)
  : DqPtrsKDTree<Node,KDTreeNodes>(ts)
  { }

//! @brief Copy constructor.
XC::DqPtrsNode::DqPtrsNode(const std::set<const Node *> &st)
  : DqPtrsKDTree<Node,KDTreeNodes>(st)
  {}

//! @brief Assignment operator.
XC::DqPtrsNode &XC::DqPtrsNode::operator=(const DqPtrsNode &other)
  {
    DqPtrsKDTree<Node,KDTreeNodes>::operator=(other);
    return *this;
  }

//! @brief += operator.
XC::DqPtrsNode &XC::DqPtrsNode::operator+=(const DqPtrsNode &other)
  {
    extend(other);
    return *this;
  }

//! @brief Desplaza los nodes of the set.
void XC::DqPtrsNode::move(const Vector3d &desplaz)
  {
    for(iterator i= begin();i!=end();i++)
      (*i)->Move(desplaz);
    create_tree();
  }

//! @brief Applies the transformation to the elements of the set.
void XC::DqPtrsNode::transforma(const TrfGeom &trf)
  {
    //Transforma 
    for(iterator i= begin();i!=end();i++)
      (*i)->Transform(trf);
    create_tree();
  }

//! @brief Returns (if it exists) a pointer to the node
//! cuyo tag is being passed as parameter.
XC::Node *XC::DqPtrsNode::findNode(const int &tag)
  {
    Node *retval= nullptr;
    Node *tmp= nullptr;
    for(iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          {
            if(tag == tmp->getTag())
              {
                retval= tmp;
                break;
              }
          }
      }
    return retval;
  }

//! @brief Returns (if it exists) a pointer to the node
//! cuyo tag is being passed as parameter.
const XC::Node *XC::DqPtrsNode::findNode(const int &tag) const
  {
    const Node *retval= nullptr;
    const Node *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          {
            if(tag == tmp->getTag())
              {
                retval= tmp;
                break;
              }
          }
      }
    return retval;
  }

//! @brief Returns the number of nodes of the set which are active.
size_t XC::DqPtrsNode::getNumLiveNodes(void) const
  {
    size_t retval= 0;
    const Node *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          if(tmp->isAlive()) retval++;
      }
    return retval;
  }

//! @brief Returns the number of nodes of the set which are inactive.
size_t XC::DqPtrsNode::getNumDeadNodes(void) const
  {
    size_t retval= 0;
    const Node *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          if(tmp->isDead()) retval++;
      }
    return retval;
  }

//!  @brief Set indices to the objects to allow its use in VTK.
void XC::DqPtrsNode::numerate(void)
  {
    size_t idx= 0;
    for(iterator i= begin();i!=end();i++,idx++)
      {
	Node *ptr= *i;
        ptr->set_index(idx);
      }
  }

//! @brief Creates the inertia load that correspond to the acceleration
//! argument.
boost::python::list XC::DqPtrsNode::createInertiaLoads(const Vector &accel)
  {
    boost::python::list retval;
    for(iterator i= begin();i!=end();i++)
      {
        const NodalLoad *loadPtr= (*i)->createInertiaLoad(accel);
	if(loadPtr)
	  {
	    boost::python::object pyObj(boost::ref(*loadPtr));
	    retval.append(pyObj);
	  }
      }
    return retval;
  }

//! @brief Return the total mass matrix.
XC::Matrix XC::DqPtrsNode::getTotalMass(void) const
  {
    Matrix retval;
    if(!empty())
      {
	const_iterator i= begin();
	retval= (*i)->getMass();
	i++;
	for(;i!=end();i++)
	  { retval+= (*i)->getMass(); }
      }
    return retval;
  }

//! @brief Return the total mass matrix component for the DOF argument.
double XC::DqPtrsNode::getTotalMassComponent(const int &dof) const
  {
    const Matrix totalMass= getTotalMass();
    const size_t sz= totalMass.noRows();
    Vector J(sz);
    J(dof)= 1.0;
    Vector tmp(sz);
    tmp.addMatrixVector(1.0, totalMass, J, 1.0);
    const double retval= dot(J,tmp);
    return retval;
  }

//! @brief Returns true if the node identified by the tag
//! being passed as parameter, belongs to the set.
bool XC::DqPtrsNode::InNodeTag(const int tag_node) const
  {
    for(const_iterator i= begin();i!=end();i++)
      if(tag_node == (*i)->getTag()) return true;
    return false;
  }

//! @brief Returns true if the nodes, with the tags
//! are being passed as parameter, belong to the set.
bool XC::DqPtrsNode::InNodeTags(const ID &tag_nodes) const
  {
    const int sz= tag_nodes.Size();
    for(int i=0;i<sz;i++)
      if(!InNodeTag(tag_nodes(i))) return false;
    return true;
  }

//! @brief Returns the tags de los nodes.
std::set<int> XC::DqPtrsNode::getTags(void) const
  {
    std::set<int> retval;
    for(const_iterator i= begin();i!=end();i++)
      retval.insert((*i)->getTag());
    return retval;
  }

//! @brief Return true if the distance to the given point is smaller
//! than the given one.
//! @param p: point to measure the distance to.
//! @param d: distance threshold.
bool  XC::DqPtrsNode::isCloserThan(const Pos3d &p, const double &d) const
  {
    bool retval= false;
    const Node *n= this->getNearest(p);
    if(n)
      {
	const Pos3d nodePos= n->getInitialPosition3d();
        retval= (nodePos.dist(p)<=d);
      }
    return retval;
  }

//! @brief Return true if the distance to all the vertices of the given sequence
//! is smaller than the given one.
//! @param s: segment to measure the distance to.
//! @param d: distance threshold.
bool XC::DqPtrsNode::isCloserThan(const GeomObj::list_Pos3d &vertices, const double &d) const
  {
    bool retval= false;
    if(!vertices.empty())
      {
	GeomObj::list_Pos3d::const_iterator i= vertices.begin();
        const Pos3d &pi= *i;
	retval= this->isCloserThan(pi, d);
	if(!retval)
	  {
	    i++;
	    for(;i!=vertices.end();i++)
	      {
		const Pos3d &pj= *i;
		retval= this->isCloserThan(pj, d);
		if(retval)
		  break;
	      }
	  }
      }
    return retval;
  }

//! @brief Return a container with the nodes that lie inside the
//! geometric object.
//!
//! @param geomObj: geometric object that must contain the nodes.
//! @param tol: tolerance for "In" function.
XC::DqPtrsNode XC::DqPtrsNode::pickNodesInside(const GeomObj3d &geomObj, const double &tol)
  {
    DqPtrsNode retval;
    for(iterator i= begin();i!=end();i++)
      {
        Node *n= (*i);
        assert(n);
	if(n->In(geomObj,tol))
	  retval.push_back(n);
      }
    return retval;    
  }

//! @brief Return a container with the nodes that lie inside the
//! geometric object.
//!
//! @param geomObj: geometric object that must contain the nodes.
//! @param tol: tolerance for "In" function.
XC::DqPtrsNode XC::DqPtrsNode::pickNodesInside(const GeomObj2d &geomObj, const double &tol)
  {
    DqPtrsNode retval;
    for(iterator i= begin();i!=end();i++)
      {
        Node *n= (*i);
        assert(n);
	if(n->In(geomObj,tol))
	  retval.push_back(n);
      }
    return retval;
  }

//! @brief Return the nodes current position boundary.
//!
//! @param factor: scale factor for the current position
//!                initPos+ factor * nodDisplacement.
BND3d XC::DqPtrsNode::Bnd(const double &factor= 1.0) const
  {
    BND3d retval;
    if(!empty())
      {
	const_iterator i= begin();
	const Node *n= (*i);
	const Pos3d p= n->getCurrentPosition3d(factor);
	retval= BND3d(p,p);
	i++;
	for(;i!=end();i++)
	  {
	    const Node *n= (*i);
	    assert(n);
	    retval+= n->getCurrentPosition3d(factor);
	  }
      }
    return retval;    
  }

//! @brief Returns the centroid of the nodes.
//!
//! @param factor: scale factor for the current position
//!                initPos+ factor * nodDisplacement.
Pos3d XC::DqPtrsNode::getCentroid(const double &factor= 1.0) const
  {
    Pos3d retval;
    if(!empty())
      {
	const size_t sz= size();
        Vector3d vectorPos;
	for(const_iterator i= begin();i!=end();i++)
	  {
	    const Node *n= (*i);
	    assert(n);
	    vectorPos+= n->getCurrentPosition3d(factor).VectorPos();
	  }
        retval+= (vectorPos*1/sz);
      }
    return retval;
  }

//! @brief Returns the regression plane from the positions of the nodes.
//!
//! @param factor: scale factor for the current position
//!                initPos+ factor * nodDisplacement.
Plane XC::DqPtrsNode::getRegressionPlane(const double &factor= 1.0) const
  {
    Plane retval;
    const size_t sz= size();
    if(sz>3) //At least three points to define a plane.
      {
        GeomObj3d::list_Pos3d lp;
	for(const_iterator i= begin();i!=end();i++)
	  {
	    const Node *n= (*i);
	    assert(n);
	    lp.push_back(n->getCurrentPosition3d(factor));
	  }
        retval= Plane(lp);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; at least 3 nodes are needed. This set contains: "
	        << sz << " nodes."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Return the orientation of the node cloud.
//! @param factor: scale factor for the current position
//!                initPos+ factor * nodDisplacement.
PrincipalAxes3D XC::DqPtrsNode::getOrientation(const double &factor= 1.0) const
  {
    GeomObj::list_Pos3d tmp;
    for(const_iterator i= begin(); i!=end(); i++)
      {
	const Node *n= (*i);
	assert(n);
	tmp.push_back(n->getCurrentPosition3d(factor));
      }
    return get_principal_axes_of_inertia(tmp);
  }

//! @brief Return the union of both containers.
XC::DqPtrsNode XC::operator+(const DqPtrsNode &a,const DqPtrsNode &b)
  {
    DqPtrsNode retval(a);
    retval+=b;
    return retval;
  }

//! @brief Return the nodes in a that are not in b.
XC::DqPtrsNode XC::operator-(const DqPtrsNode &a,const DqPtrsNode &b)
  {
    DqPtrsNode retval;
    for(DqPtrsNode::const_iterator i= a.begin();i!=a.end();i++)
      {
        Node *n= (*i);
        assert(n);
	if(!b.in(n)) //If not in b.
	  retval.push_back(n);
      }
    return retval;
  }

//! @brief Return the nodes in a that are also in b.
XC::DqPtrsNode XC::operator*(const DqPtrsNode &a,const DqPtrsNode &b)
  {
    DqPtrsNode retval;
    for(DqPtrsNode::const_iterator i= a.begin();i!=a.end();i++)
      {
        Node *n= (*i);
        assert(n);
	if(b.in(n)) //If also in b.
	  retval.push_back(n);
      }
    return retval;    
  }
