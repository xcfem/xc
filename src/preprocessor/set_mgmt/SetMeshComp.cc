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
//SetMeshComp.cc

#include "SetMeshComp.h"
#include "Set.h"
#include "domain/domain/Domain.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/multi_block_topology/trf/TrfGeom.h"
#include "domain/constraints/SFreedom_Constraint.h"
#include "domain/constraints/MFreedom_Constraint.h"
#include <domain/constraints/MRMFreedom_Constraint.h>
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include "solution/graph/graph/Graph.h"
#include "solution/graph/graph/Vertex.h"
#include "utility/matrix/ID.h"

#include "utility/geom/pos_vec/SlidingVectorsSystem3d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/geom/d3/HalfSpace3d.h"
#include "utility/geom/d3/BND3d.h"
#include "utility/geom/coo_sys/ref_sys/PrincipalAxes3D.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::SetMeshComp::SetMeshComp(const std::string &nmb,Preprocessor *md)
  : SetBase(nmb,md), nodes(this), elements(this), constraints(this) {}

//! @brief Copy constructor.
XC::SetMeshComp::SetMeshComp(const SetMeshComp &other)
  : SetBase(other)
  { copy_lists(other); }

//! @brief Assignment operator.
XC::SetMeshComp &XC::SetMeshComp::operator=(const SetMeshComp &other)
  {
    SetBase::operator=(other);
    copy_lists(other);
    return *this;
  }

//! @brief += operator.
XC::SetMeshComp &XC::SetMeshComp::operator+=(const SetMeshComp &other)
  {
    const std::string oldName= getStrName();
    SetBase::operator+=(other);
    extend_lists(other);
    MapSet &map_set= getPreprocessor()->get_sets();
    if(map_set.exists(oldName)) // already allocated.
      map_set.rename(oldName,getStrName());
    return *this;
  }

//! @brief -= operator.
XC::SetMeshComp &XC::SetMeshComp::operator-=(const SetMeshComp &other)
  {
    const std::string oldName= getStrName();
    SetBase::operator-=(other);
    substract_lists(other);
    MapSet &map_set= getPreprocessor()->get_sets();
    if(map_set.exists(oldName)) // already allocated.
      map_set.rename(oldName,getStrName());
    return *this;
  }

//! @brief *= operator (intersection).
XC::SetMeshComp &XC::SetMeshComp::operator*=(const SetMeshComp &other)
  {
    const std::string oldName= getStrName();
    SetBase::operator*=(other);
    intersect_lists(other);
    MapSet &map_set= getPreprocessor()->get_sets();
    if(map_set.exists(oldName)) // already allocated.
      map_set.rename(oldName,getStrName());
    return *this;
  }

//! @brief Return a copy of the object name.
std::string XC::SetMeshComp::getStrName(void) const
  { return EntMdlrBase::getName(); }

//! @brief Set the object name.
void XC::SetMeshComp::newName(const std::string &newName)
  { rename(newName); }
  
//! @brief Rename object.
void XC::SetMeshComp::rename(const std::string &newName)
  {
    const std::string oldName= getStrName();
    if(oldName!=newName)
      {
        MapSet &map_set= getPreprocessor()->get_sets();
        map_set.rename(oldName,newName);
	if(newName!=getStrName())
	  {
	    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	              << "; something went wrong, name is: "
	              << getStrName() << " and must be: "
	              << newName
		      << Color::def << std::endl;
	    setName(newName);
	  }
      }
  }

//! @brief Copy (into this set) the lists from the set being passed as parameter.
void XC::SetMeshComp::copy_lists(const SetMeshComp &other)
  {
    nodes= other.nodes;
    nodes.set_owner(this);
    elements= other.elements;
    elements.set_owner(this);
    constraints= other.constraints;
    constraints.set_owner(this);
  }

//! @brief Appends to this set the nodes from the argument.
void XC::SetMeshComp::extend(const DqPtrsNode &nds)
  { nodes.extend(nds); }

//! @brief Appends to this set the elements from the argument.
void XC::SetMeshComp::extend(const DqPtrsElem &elems)
  { elements.extend(elems); }
      
//! @brief Appends to this set the constraints from the argument.
void XC::SetMeshComp::extend(const lst_ptr_constraints &cs)
  { constraints.extend(cs); }
       
//! @brief Appends to this set the objects from the argument set.
void XC::SetMeshComp::extend(const SetMeshComp &other)
  { extend_lists(other); }

//! @brief Appends to this set the nodes from the argument.
void XC::SetMeshComp::substract(const DqPtrsNode &nds)
  {
    nodes= this->nodes-nds;
    nodes.set_owner(this);
  }

//! @brief Appends to this set the elements from the argument.
void XC::SetMeshComp::substract(const DqPtrsElem &elems)
  {
    elements= this->elements-elems;
    elements.set_owner(this);
  }
      
//! @brief Appends to this set the constraints from the argument.
void XC::SetMeshComp::substract(const lst_ptr_constraints &cs)
  {
    constraints= this->constraints-cs;
    constraints.set_owner(this);
  }
       
//! @brief Appends to this set the objects from the argument set.
void XC::SetMeshComp::substract(const SetMeshComp &other)
  { substract_lists(other); }

//! @brief Appends to this set the objects from the argument set.
void XC::SetMeshComp::extend_lists(const SetMeshComp &other)
  {
    nodes.extend(other.nodes);
    elements.extend(other.elements);
    constraints.extend(other.constraints);
  }

//! @brief Remove the objects of the argument.
void XC::SetMeshComp::substract_lists(const SetMeshComp &other)
  {
    this->substract(other.nodes);
    this->substract(other.elements);
    this->substract(other.constraints);
  }

//! @brief Remove the objects that doesn't also belong to the argument.
void XC::SetMeshComp::intersect_lists(const SetMeshComp &other)
  {
    nodes= nodes*other.nodes;
    nodes.set_owner(this);
    elements= elements*other.elements;
    elements.set_owner(this);
    constraints= constraints*other.constraints;
    constraints.set_owner(this);
  }
	
	
//! @brief Appends to this set the objects the nodes and elements from the set
//! being passed as parameter.
void XC::SetMeshComp::appendFromGeomEntity(const SetBase &s)
  {
    Domain *dom= getPreprocessor()->getDomain();
    const std::set<int> nodeTags= s.getNodeTags();
    for(std::set<int>::const_iterator i= nodeTags.begin();i!=nodeTags.end();i++)
      nodes.push_back(dom->getNode(*i));
      
    const std::set<int> elementTags= s.getElementTags();
    for(std::set<int>::const_iterator i= elementTags.begin();i!=elementTags.end();i++)
      elements.push_back(dom->getElement(*i));
  }

//! @brief Clears all the objects of the lists.
void XC::SetMeshComp::clear(void)
  {
    //SetBase::clear();
    nodes.clear();
    constraints.clear();
    elements.clear();
  }

//! @brief Return true if the set is empty.
bool XC::SetMeshComp::empty(void) const
  { return ( nodes.empty() && elements.empty() && constraints.empty() ); }


//!  @brief Clears all objects from the set.
void XC::SetMeshComp::clearAll(void)
  {
    SetBase::clearPyProps();
    nodes.clearAll();
    constraints.clearAll();
    elements.clearAll();
  }

//! @brief Adds the pointer to node being passed as parameter.
void XC::SetMeshComp::addNode(Node *nPtr)
  { nodes.push_back(nPtr); }

//! @brief Adds the pointer to element being passed as parameter.
void XC::SetMeshComp::addElement(Element *ePtr)
  { elements.push_back(ePtr); }

//! @brief Returns true if the node belongs to the set.
bool XC::SetMeshComp::In(const Node *n) const
  { return nodes.in(n); }

//! @brief Return true if the distance to the given point is smaller
//! than the given one.
//! @param p: point to measure the distance to.
//! @param d: distance threshold.
bool XC::SetMeshComp::isCloserThan(const Pos3d &p, const double &d) const
  {
    bool retval= false;
    if(!nodes.empty())
      retval= nodes.isCloserThan(p, d);
    if(!retval)
      if(!elements.empty())
	retval= elements.isCloserThan(p, d);
    return retval;
  }

//! @brief Return true if the distance to the given point is smaller
//! than the given one.
//! @param p: point to measure the distance to.
//! @param d: distance threshold.
bool XC::SetMeshComp::isCloserThan(const GeomObj::list_Pos3d &vertices, const double &d) const
  {
    bool retval= false;
    if(!nodes.empty())
      retval= nodes.isCloserThan(vertices, d);
    if(!retval)
      if(!elements.empty())
	retval= elements.isCloserThan(vertices, d);
    return retval;
  }

//! @brief Remove the given node from the set.
bool XC::SetMeshComp::remove(Node *n)
  { return nodes.remove(n); }

//! @brief Returns true if the element belongs to the set.
bool XC::SetMeshComp::In(const Element *e) const
  { return elements.in(e); }

//! @brief Remove the given element from the set.
bool XC::SetMeshComp::remove(Element *e)
  { return elements.remove(e); }

//! @brief Remove the given constraint from the set.
bool XC::SetMeshComp::remove(Constraint *c)
  { return constraints.remove(c); }

//! @brief Returns true if the constraint belongs to the set.
bool XC::SetMeshComp::In(const Constraint *c) const
  { return constraints.in(c); }

//! @brief Assigns index to each object in the set (nodes,elements,points...)
//! to be able to use them in VTK.
void XC::SetMeshComp::numerate(void)
  {
    nodes.numerate();
    elements.numerate();
    numerate_list(constraints);
  }

//! @brief Moves the nodes.
void XC::SetMeshComp::move(const Vector3d &desplaz)
  { nodes.move(desplaz); }

//! @brief Aplies the transformation to the positions of the nodes.
void XC::SetMeshComp::Transform(const TrfGeom &trf)
  { nodes.transforma(trf); }

//! @brief Applies the single freedom constraint being passed as parameter to the nodes.
void XC::SetMeshComp::fix(const SFreedom_Constraint &spc)
  {
    BoundaryCondHandler &cl= getPreprocessor()->getBoundaryCondHandler();
    for(nod_const_iterator i= nodes_begin();i!=nodes_end();i++)
      {
        const int tag_nod= (*i)->getTag();
        cl.addSFreedom_Constraint(tag_nod,spc);
      }
  }

// //! @brief Returns an iterator that points to the first item of the node container.
// XC::SetMeshComp::nod_iterator XC::SetMeshComp::nodes_begin(void)
//   { return nodes.begin(); }
//! @brief Returns an iterator that points to the first item of the node container.
XC::SetMeshComp::nod_const_iterator XC::SetMeshComp::nodes_begin(void) const
  { return nodes.begin(); }
// //! @brief Returns an iterator that points to the past-the-end item in the node container.
// XC::SetMeshComp::nod_iterator XC::SetMeshComp::nodes_end(void)
//   { return nodes.end(); }
//! @brief Returns an iterator that points to the past-the-end item in the node container.
XC::SetMeshComp::nod_const_iterator XC::SetMeshComp::nodes_end(void) const
  { return nodes.end(); }

//! @brief Returns (if it exists) a pointer to the node
//! which tag is being passed as parameter.
XC::Node *XC::SetMeshComp::findNode(const int &tag)
  { return nodes.findNode(tag); }

//! @brief Returns (if it exists) a pointer to the node
//! which tag is being passed as parameter.
const XC::Node *XC::SetMeshComp::findNode(const int &tag) const
  { return nodes.findNode(tag); }

// //! @brief Returns an iterator which points to principio de la lista de elements.
// XC::SetMeshComp::elem_iterator XC::SetMeshComp::elem_begin(void)
//   { return elements.begin(); }
//! @brief Returns an iterator which points to principio de la lista de elements.
XC::SetMeshComp::elem_const_iterator XC::SetMeshComp::elem_begin(void) const
  { return elements.begin(); }
// //! @brief Returns an iterator apuntando tras el final de la lista de elements.
// XC::SetMeshComp::elem_iterator XC::SetMeshComp::elem_end(void)
//   { return elements.end(); }
//! @brief Returns an iterator apuntando tras el final de la lista de elements.
XC::SetMeshComp::elem_const_iterator XC::SetMeshComp::elem_end(void) const
  { return elements.end(); }

//! @brief Returns an iterator which points to principio de la lista de constraints.
XC::SetMeshComp::constraint_iterator XC::SetMeshComp::constraints_begin(void)
  { return constraints.begin(); }
//! @brief Returns an iterator which points to principio de la lista de constraints.
XC::SetMeshComp::constraint_const_iterator XC::SetMeshComp::constraints_begin(void) const
  { return constraints.begin(); }
//! @brief Returns an iterator apuntando tras el final de la lista de constraints.
XC::SetMeshComp::constraint_iterator XC::SetMeshComp::constraints_end(void)
  { return constraints.end(); }
//! @brief Returns an iterator apuntando tras el final de la lista de constraints.
XC::SetMeshComp::constraint_const_iterator XC::SetMeshComp::constraints_end(void) const
  { return constraints.end(); }

//! @brief Returns (if it exists) a pointer to the node
//! identified by the tag being passed as parameter.
XC::Element *XC::SetMeshComp::findElement(const int &tag)
  { return elements.findElement(tag); }

//! @brief Returns (if it exists) a pointer to the node
//! identified by the tag being passed as parameter.
const XC::Element *XC::SetMeshComp::findElement(const int &tag) const
  { return elements.findElement(tag); }

//! @brief Returns, if it exists, a pointer to the constraint
//! which tag is being passed as parameter.
XC::Constraint *XC::SetMeshComp::buscaConstraint(const int &tag)
  {
    Constraint *retval= nullptr;
    Constraint *tmp= nullptr;
    for(lst_ptr_constraints::iterator i= constraints.begin();i!=constraints.end();i++)
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

//! @brief Returns, if it exists, a pointer to the constraint
//! which tag is being passed as parameter.
const XC::Constraint *XC::SetMeshComp::buscaConstraint(const int &tag) const
  {
    const Constraint *retval= nullptr;
    const Constraint *tmp= nullptr;
    for(lst_ptr_constraints::const_iterator i= constraints.begin();i!=constraints.end();i++)
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

//! @brief Returns the number of active elements.
size_t XC::SetMeshComp::getNumLiveElements(void) const
  { return elements.getNumLiveElements(); }

//! @brief Returns the number of inactive elements.
size_t XC::SetMeshComp::getNumDeadElements(void) const
  { return elements.getNumDeadElements(); }

//! @brief Returns the number of active nodes.
size_t XC::SetMeshComp::getNumLiveNodes(void) const
  { return nodes.getNumLiveNodes(); }


//! @brief Returns the number of inactive nodes.
size_t XC::SetMeshComp::getNumDeadNodes(void) const
  { return nodes.getNumDeadNodes(); }

//! @brief Deactivates the elements.
void XC::SetMeshComp::kill_elements(void)
  { elements.kill_elements(); }

//! @brief Reactivates the elements.
void XC::SetMeshComp::alive_elements(void)
  { elements.alive_elements(); }

//! @brief Compute the internal forces on each element.
void XC::SetMeshComp::calc_resisting_force(void)
  { elements.calc_resisting_force(); }

//! @brief Return the resultant of the forces over the nodes
//! near to the plane, of the elements behind the plane.
SlidingVectorsSystem3d XC::SetMeshComp::getResistingSlidingVectorsSystem3d(const Plane &plane,const Pos3d &centro,const double &tol,const bool &inc_inertia) const
  {
    //XX Can be enhanced computing the resultant in the intersection of
    //the element edges with the plane and interpolating the value at this
    //point from the values in the nodes at the ends of the edge.
    const double tol2= tol*tol;
    const Node *ptrNod= nullptr;
    const HalfSpace3d se(plane);
    std::set<const Node *> nodes_on_plane;
    std::set<const Element *> connected_to_node;
    std::set<const Element *> connected_elements;
    for(DqPtrsNode::const_iterator i= nodes.begin();i!=nodes.end();i++)
      {
        ptrNod= *i;
        if(ptrNod)
          if(ptrNod->isAlive())
            {
              if(plane.dist2(ptrNod->getInitialPosition3d())<tol2)
                {
                  connected_to_node= ptrNod->getConnectedElements();
                  if(!connected_to_node.empty())
                    {
                      nodes_on_plane.insert(ptrNod); //Node on the plane.
                      for(std::set<const Element *>::const_iterator j= connected_to_node.begin();
                          j!=connected_to_node.end();j++)
                        {
                          if((*j)->getNodePtrs().In(se)) //Element behind the plane.
                            connected_elements.insert(*j);
                          else if((*j)->getNodePtrs().Corta(plane)) //Element in the plane.
                            connected_elements.insert(*j);
                        }
                    }
	        }
            }
      }
    SlidingVectorsSystem3d retval(centro);
    for(std::set<const Node *>::const_iterator i= nodes_on_plane.begin();i!=nodes_on_plane.end();i++)
      retval+= (*i)->getResistingSlidingVectorsSystem3d(connected_elements,inc_inertia);
    return retval;    
  }

//! @brief Return the contribution of the elements
//! to the tangent stiffness of the node argument.
//!
//! Each of the elements connected to the node contributes to the stiffness
//! of the model in that node. This methods return the contribution (if any)
//! of the elements being passed as parameter.
XC::Matrix XC::SetMeshComp::getTangentStiff(const Node &n)
  { return n.getTangentStiff(elements.getConstPtrSet()); }

//! @brief Return the contribution of the elements
//! to the tangent stiffness of the node argument.
//!
//! Each of the elements connected to the node contributes to the stiffness
//! of the model in that node. This methods return the contribution (if any)
//! of the elements being passed as parameter.
XC::Matrix XC::SetMeshComp::getInitialStiff(const Node &n)
  { return n.getInitialStiff(elements.getConstPtrSet()); }

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
boost::python::list XC::SetMeshComp::createInertiaLoads(const Vector &accel)
  {
    boost::python::list retval= nodes.createInertiaLoads(accel);
    retval.extend(elements.createInertiaLoads(accel));
    return retval;
  }

//! @brief Return the total mass matrix.
XC::Matrix XC::SetMeshComp::getTotalMass(void) const
  {
    Matrix retval= nodes.getTotalMass();
    retval+= elements.getTotalMass();
    return retval;
  }

//! @brief Return the total mass matrix component for the DOF argument.
double XC::SetMeshComp::getTotalMassComponent(const int &dof) const
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

//! @brief Returns true if the node with the tag
//! being passed as parameter, belongs to the set.
bool XC::SetMeshComp::InNodeTag(const int tag_node) const
  { return nodes.InNodeTag(tag_node); }

//! @brief Returns true if the nodes, with the tags
//! being passed as parameter, belongs to the set.
bool XC::SetMeshComp::InNodeTags(const ID &tag_nodes) const
  { return nodes.InNodeTags(tag_nodes); }

//! @brief Returns node tags.
std::set<int> XC::SetMeshComp::getNodeTags(void) const
  { return nodes.getTags(); }

//! @brief Returns the node closest to the point being passed as parameter.
XC::Node *XC::SetMeshComp::getNearestNode(const Pos3d &p)
  { return nodes.getNearest(p); }

//! @brief Returns the node closest to the point being passed as parameter.
const XC::Node *XC::SetMeshComp::getNearestNode(const Pos3d &p) const
  {
    SetMeshComp *this_no_const= const_cast<SetMeshComp *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Returns the element tags.
std::set<int> XC::SetMeshComp::getElementTags(void) const
  { return elements.getTags(); }

//! @brief Returns the element closest to the point being passed as parameter.
XC::Element *XC::SetMeshComp::getNearestElement(const Pos3d &p)
  { return elements.getNearest(p); }

//! @brief Returns the element closest to the point being passed as parameter.
const XC::Element *XC::SetMeshComp::getNearestElement(const Pos3d &p) const
  {
    SetMeshComp *this_no_const= const_cast<SetMeshComp *>(this);
    return this_no_const->getNearestElement(p);
  }

//! @brief Returns the tags de los constraints.
std::set<int> XC::SetMeshComp::getConstraintTags(void) const
  {
    std::set<int> retval;
    for(constraint_const_iterator i= constraints.begin();i!=constraints.end();i++)
      retval.insert((*i)->getTag());
    return retval;
  }

//! @brief Appends to the set being passed as parameter
//! the nodes that touch any of the elements of the set.
void XC::SetMeshComp::fillDownwards(void)
  {
    const DqPtrsElem &elems= elements;
    for(elem_const_iterator i= elems.begin();i!=elems.end();i++)
      {
        Element *elem= (*i);
        assert(elem);
        NodePtrs &theNodes= elem->getNodePtrs();
        for(NodePtrs::iterator j= theNodes.begin();j!=theNodes.end();j++)
          nodes.push_back(*j);
      }
  }

//! @brief Appends to this set the objects that make reference to one
//! or more of the objects that already are in the set.
void XC::SetMeshComp::fillUpwards(void)
  {
    const size_t numNodes= nodes.size();
    const size_t numElements= elements.size();
    if((numNodes>0) || (numElements>0))
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << "; implementation pending."
		<< Color::def << std::endl;
    Node *ptrNod= nullptr;
    Node::ElementPtrSet connected_to_node;
    for(DqPtrsNode::iterator i= nodes.begin();i!=nodes.end();i++)
      {
        ptrNod= *i;
        if(ptrNod)
	  {
            connected_to_node= ptrNod->getConnectedElements();
            if(!connected_to_node.empty())
              {
		
                for(std::set<Element *>::iterator j= connected_to_node.begin(); j!=connected_to_node.end();j++)
		  { elements.push_back(*j); }
	      }
	  }
      }
  }

//! @brief Select the nodes identified by the tags being passed as parameters.
void XC::SetMeshComp::sel_nodes_from_list(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= getPreprocessor();
        if(preprocessor)
          for(size_t i= 0;i<sz;i++)
            nodes.push_back(preprocessor->getDomain()->getNode(tags(i)));
        else
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << "; preprocessor needed."
		    << Color::def << std::endl;
      }
  }

//! @brief Return a new set that contains the nodes that lie insiof the
//! geometric object.
//!
//! @param newSetName: name for the new set.
//! @param geomObj: geometric object that must contain the nodes.
//! @param tol: tolerance for "In" function.
XC::SetMeshComp XC::SetMeshComp::pickNodesInside(const std::string &newSetName, const GeomObj3d &geomObj, const double &tol)
  {
    SetMeshComp retval(newSetName);
    retval.nodes= nodes.pickNodesInside(geomObj,tol);
    retval.nodes.set_owner(&retval);
    return retval;    
  }

//! @brief Return a new set that contains the nodes that lie insiof the
//! geometric object.
//!
//! @param newSetName: name for the new set.
//! @param geomObj: geometric object that must contain the nodes.
//! @param tol: tolerance for "In" function.
XC::SetMeshComp XC::SetMeshComp::pickNodesInside(const std::string &newSetName, const GeomObj2d &geomObj, const double &tol)
  {
    SetMeshComp retval(newSetName);
    retval.nodes= nodes.pickNodesInside(geomObj,tol);
    retval.nodes.set_owner(&retval);
    return retval;    
  }

//! @brief Return the nodes current position boundary.
//!
//! @param factor: scale factor for the current position
//!                initPos+ factor * nodDisplacement.
BND3d XC::SetMeshComp::Bnd(const double &factor) const
  {
    BND3d retval= nodes.Bnd(factor);
    if(nodes.empty())
      std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	        << " node container empty for set: '"
	        << this->getName()
	        << "'. Call fillDownwards?"
	        << Color::def << std::endl;
    retval+= elements.Bnd(factor);
    return retval;
  }

//! @brief Return the orientation of the node cloud.
//! @param factor: scale factor for the current position
//!                initPos+ factor * nodDisplacement.
PrincipalAxes3D XC::SetMeshComp::getOrientation(const double &factor) const
  { return nodes.getOrientation(factor); }

//! @brief Selects the elements identified by the tags being passed as parameters.
void XC::SetMeshComp::sel_elements_from_list(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= getPreprocessor();
        if(preprocessor)
          for(size_t i= 0;i<sz;i++)
            elements.push_back(preprocessor->getDomain()->getElement(tags(i)));
        else
          std::cerr << Color::red << getClassName() << __FUNCTION__
	            << "; preprocessor needed."
		    << Color::def << std::endl;
      }
  }

//! @brief Return a new set that contains the elements that lie inside of the
//! geometric object.
//!
//! @param newSetName: name for the new set.
//! @param geomObj: geometric object that must contain the elements.
//! @param tol: tolerance for "In" function.
XC::SetMeshComp XC::SetMeshComp::pickElemsInside(const std::string &newSetName, const GeomObj3d &geomObj, const double &tol)
  {
    SetMeshComp retval(newSetName);
    retval.elements= elements.pickElemsInside(geomObj, tol);
    retval.elements.set_owner(&retval);
    return retval;
  }

//! @brief Return a new set that contains the elements that lie inside of the
//! geometric object.
//!
//! @param newSetName: name for the new set.
//! @param geomObj: geometric object that must contain the elements.
//! @param tol: tolerance for "In" function.
XC::SetMeshComp XC::SetMeshComp::pickElemsInside(const std::string &newSetName, const GeomObj2d &geomObj, const double &tol)
  {
    SetMeshComp retval(newSetName);
    retval.elements= elements.pickElemsInside(geomObj, tol);
    retval.elements.set_owner(&retval);
    return retval;
  }

//! @brief Return a new set with the elements that cross (i.e. have nodes
//! inside and outside) the given geometric object.
//!
//! @param newSetName: name for the new set.
//! @param geomObj: geometric object that must contain the elements.
//! @param tol: tolerance for "In" function.
XC::SetMeshComp XC::SetMeshComp::pickElemsCrossing(const std::string &newSetName, const GeomObj2d &geomObj, const double &tol)
  {
    SetMeshComp retval(newSetName);
    retval.elements= elements.pickElemsCrossing(geomObj, tol);
    retval.elements.set_owner(&retval);
    return retval;
  }

//! @brief Return a new set with the elements that cross (i.e. have nodes
//! inside and outside) the given geometric object.
//!
//! @param newSetName: name for the new set.
//! @param geomObj: geometric object that must contain the elements.
//! @param tol: tolerance for "In" function.
XC::SetMeshComp XC::SetMeshComp::pickElemsCrossing(const std::string &newSetName, const GeomObj3d &geomObj, const double &tol)
  {
    SetMeshComp retval(newSetName);
    retval.elements= elements.pickElemsCrossing(geomObj, tol);
    retval.elements.set_owner(&retval);
    return retval;
  }

//! @brief Return a new set that contains the elements that match the
//! type.
//!
//! @param newSetName: name for the new set.
//! @param typeName: string that must be contained in the class name.
XC::SetMeshComp XC::SetMeshComp::pickElemsOfType(const std::string &newSetName, const std::string &typeName)
  {
    SetMeshComp retval(newSetName);
    retval.elements= elements.pickElemsOfType(typeName);
    retval.elements.set_owner(&retval);
    return retval;
  }

//! @brief Return a new set that contains the elements of the specified
//! dimension.
//!
//! @param newSetName: name for the new set.
//! @param targetDim: element dimension (point: 0, line: 1, surface: 2, volume: 3)
XC::SetMeshComp XC::SetMeshComp::pickElemsOfDimension(const std::string &newSetName, const size_t &targetDim)
  {
    SetMeshComp retval(newSetName);
    retval.elements= elements.pickElemsOfDimension(targetDim);
    retval.elements.set_owner(&retval);
    return retval;
  }

//! @brief Return a new set that contains the elements who have the material
//! being passed as parameter.
//!
//! @param newSetName: name for the new set.
//! @param targetName: name of the target material.
XC::SetMeshComp XC::SetMeshComp::pickElemsOfMaterial(const std::string &newSetName, const std::string &targetName)
  {
    SetMeshComp retval(newSetName);
    retval.elements= elements.pickElemsOfMaterial(targetName);
    retval.elements.set_owner(&retval);
    return retval;
  }

//! @brief Add constraint to the set.
void XC::SetMeshComp::addConstraint(Constraint *c)
  { constraints.push_back(c); }

//! @brief Select the constraints identified by the tags.
//!
//! @param tags: identifiers of the constraints.
void XC::SetMeshComp::sel_constraints_from_list(const ID &tags, const ID &cTypes)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= getPreprocessor();
        Constraint *tmp= nullptr;
        if(preprocessor)
          for(size_t i= 0;i<sz;i++)
            {
	      const int cType= cTypes[i]; // Constraint type.
	      const int tag= tags(i); // Identifier.
	      switch(cType)
		{
		  case 0:
		    tmp= preprocessor->getDomain()->getConstraints().getSFreedom_Constraint(tag);
		    break;
		  case 1:
		    tmp= preprocessor->getDomain()->getConstraints().getMFreedom_Constraint(tag);
		    break;
		  case 2:
		    tmp= preprocessor->getDomain()->getConstraints().getMRMFreedom_Constraint(tag);
		    break;
		  default:
		    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			      << "; constraint identified by: "
			      << tag << " has an unknown type: " << cType
			      << Color::def << std::endl;
		    break;
		}
              if(tmp)
                constraints.push_back(tmp);
              else
		std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		          << "; constraint identified by: "
                          << tag << " not found."
			  << Color::def << std::endl;
            }

        else
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << "preprocessor needed."
		    << Color::def << std::endl;
      }
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::SetMeshComp::getDbTagData(void) const
  {
    static DbTagData retval(19);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::SetMeshComp::sendData(Communicator &comm)
  {
    int res= SetBase::sendData(comm);
    res+= nodes.sendTags(3,4,getDbTagData(),comm);
    res+= elements.sendTags(5,6,getDbTagData(),comm);
    res+= constraints.sendTags(7,8,getDbTagData(),comm);
    const ID &cTypes= constraints.getTypes();
    res+= comm.sendID(cTypes,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::SetMeshComp::recvData(const Communicator &comm)
  {
    ID tmp;
    int res= SetBase::recvData(comm);
    tmp= nodes.receiveTags(3,4,getDbTagData(),comm);
    sel_nodes_from_list(tmp);
    tmp= elements.receiveTags(5,6,getDbTagData(),comm);
    sel_elements_from_list(tmp);
    tmp= constraints.receiveTags(7,8,getDbTagData(),comm);
    ID cTypes;
    res+= comm.receiveID(cTypes,getDbTagData(),CommMetaData(9));
    sel_constraints_from_list(tmp, cTypes);
    return res;
  }

//!  @brief Destructor.
XC::SetMeshComp::~SetMeshComp(void)
  { clearAll(); }

//! @brief Sends object through the communicator argument.
int XC::SetMeshComp::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::SetMeshComp::recvSelf(const Communicator &comm)
  {
    inicComm(19);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids."
	        << Color::def << std::endl;
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr  << getClassName() << "::" << __FUNCTION__
		     << "; failed to receive data."
	             << Color::def << std::endl;
      }
    return res;
  }

//! @brief Return the union of both objects.
XC::SetMeshComp XC::SetMeshComp::operator+(const SetMeshComp &b) const
  {
    SetMeshComp retval(*this);
    retval+=b;
    return retval;
  }

//! @brief Return the difference.
XC::SetMeshComp XC::SetMeshComp::operator-(const SetMeshComp &b) const
  {
    SetMeshComp retval(*this);
    retval-=b;
    return retval;
  }

//! @brief Return the intersection.
XC::SetMeshComp XC::SetMeshComp::operator*(const SetMeshComp &b) const
  {
    SetMeshComp retval(*this);
    retval*=b;
    return retval;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::SetMeshComp::getPyDict(void) const
  {
    boost::python::dict retval= SetBase::getPyDict();
    boost::python::list nodeTags;
    for(nod_const_iterator i= nodes.begin(); i!=nodes.end(); i++)
      {
	const Node *n= *i;
        nodeTags.append(n->getTag());
      }
    retval["nodeTags"]= nodeTags;
    boost::python::list elementTags;
    for(elem_const_iterator i= elements.begin(); i!=elements.end(); i++)
      {
	const Element *e= *i;
        elementTags.append(e->getTag());
      }
    retval["elementTags"]= elementTags;
    boost::python::list constraintTags;
    for(constraint_const_iterator i= constraints.begin(); i!=constraints.end(); i++)
      {
	const Constraint *c= *i;
        constraintTags.append(c->getTag());
      }
    retval["constraintTags"]= constraintTags;
    boost::python::list constraintTypes;
    for(constraint_const_iterator i= constraints.begin(); i!=constraints.end(); i++)
      {
	const Constraint *c= *i;
	int cType= -1;
	if(c)
	  {
	    const SFreedom_Constraint *spc= dynamic_cast<const SFreedom_Constraint *>(c);
	    if(spc)
	      cType= 0;
	    else
	      {
		const MFreedom_Constraint *mfc= dynamic_cast<const MFreedom_Constraint *>(c);
		if(mfc)
		  cType= 1;
		else
		  {
		    const MRMFreedom_Constraint *mrmf= dynamic_cast<const MRMFreedom_Constraint *>(c);
		    if(mrmf)
		      cType= 2;
		  }
	      }
	  }
        constraintTypes.append(cType);
      }
    retval["constraintTypes"]= constraintTypes;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::SetMeshComp::setPyDict(const boost::python::dict &d)
  {
    SetBase::setPyDict(d);
    boost::python::list nodeTags= boost::python::extract<boost::python::list>(d["nodeTags"]);
    const size_t numNodes= boost::python::len(nodeTags);
    boost::python::list elementTags= boost::python::extract<boost::python::list>(d["elementTags"]);
    const size_t numElements= boost::python::len(elementTags);
    ID nIds(numNodes);
    for(size_t i= 0; i<numNodes; i++)
      {
	const size_t nodeTag= boost::python::extract<int>(nodeTags[i]);
	nIds[i]= nodeTag;
      }
    sel_nodes_from_list(nIds);
    ID eIds(numElements);
    for(size_t i= 0; i<numElements; i++)
      {
	const size_t elementTag= boost::python::extract<int>(elementTags[i]);
	eIds[i]= elementTag;
      }
    sel_elements_from_list(eIds);
    
    boost::python::list constraintTags= boost::python::extract<boost::python::list>(d["constraintTags"]);
    const size_t numConstraints= boost::python::len(constraintTags);
    boost::python::list constraintTypes= boost::python::extract<boost::python::list>(d["constraintTypes"]);
    ID cIds(numConstraints);
    ID cTypes(numConstraints);
    for(size_t i= 0; i<numConstraints; i++)
      {
	const int constraintTag= boost::python::extract<int>(constraintTags[i]);
	cIds[i]= constraintTag;
	const int constraintType= boost::python::extract<int>(constraintTypes[i]);
	cTypes[i]= constraintType;
      }
    sel_constraints_from_list(cIds, cTypes);
  }
