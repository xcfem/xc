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
//SetMeshComp.cc

#include "SetMeshComp.h"
#include "Set.h"
#include "domain/domain/Domain.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/multi_block_topology/trf/TrfGeom.h"
#include "domain/constraints/SFreedom_Constraint.h"
#include "domain/constraints/MFreedom_Constraint.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include "solution/graph/graph/Graph.h"
#include "solution/graph/graph/Vertex.h"
#include "utility/matrix/ID.h"

#include "xc_utils/src/geom/pos_vec/SlidingVectorsSystem3d.h"
#include "xc_utils/src/geom/d2/Plane.h"
#include "xc_utils/src/geom/d3/HalfSpace3d.h"
#include "xc_utils/src/geom/d3/BND3d.h"



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
    SetBase::operator+=(other);
    extend_lists(other);
    return *this;
  }

//! @brief -= operator.
XC::SetMeshComp &XC::SetMeshComp::operator-=(const SetMeshComp &other)
  {
    SetBase::operator-=(other);
    substract_lists(other);
    return *this;
  }

//! @brief *= operator (intersection).
XC::SetMeshComp &XC::SetMeshComp::operator*=(const SetMeshComp &other)
  {
    SetBase::operator*=(other);
    intersect_lists(other);
    return *this;
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

//! @brief Appends to this set the objects from the set
//! being passed as parameter.
void XC::SetMeshComp::extend_lists(const SetMeshComp &other)
  {
    nodes.extend(other.nodes);
    elements.extend(other.elements);
    constraints.extend(other.constraints);
  }

//! @brief Remove the objects of the argument.
void XC::SetMeshComp::substract_lists(const SetMeshComp &other)
  {
    nodes= nodes-other.nodes;
    elements= elements-other.elements;
    constraints= constraints-other.constraints;
  }

//! @brief Remove the objects that doesn't also belong to the argument.
void XC::SetMeshComp::intersect_lists(const SetMeshComp &other)
  {
    nodes= nodes*other.nodes;
    elements= elements*other.elements;
    constraints= constraints*other.constraints;
  }
	
	
//! @brief Appends to this set the objects the nodes and elements from the set
//! being passed as parameter.
void XC::SetMeshComp::appendFromGeomEntity(const SetBase &s)
  {
    Domain *dom= getPreprocessor()->getDomain();
    const std::set<int> nodeTags= s.getNodeTags();
    for(std::set<int>::const_iterator i= nodeTags.begin();i!=nodeTags.end();i++)
      nodes.push_back(dom->getNode(*i));
      
    const std::set<int> elementTags= getElementTags();
    for(std::set<int>::const_iterator i= nodeTags.begin();i!=nodeTags.end();i++)
      nodes.push_back(dom->getNode(*i));
  }

//!  @brief Clears all the objectsof the lists.
void XC::SetMeshComp::clear(void)
  {
    //SetBase::clear();
    nodes.clear();
    constraints.clear();
    elements.clear();
  }

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

//! @brief Returns true if the element belongs to the set.
bool XC::SetMeshComp::In(const Element *e) const
  { return elements.in(e); }

//! @brief Assigns index to each object in the set (nodes,elements,points...)
//! to be able to use them in VTK.
void XC::SetMeshComp::numera(void)
  {
    nodes.numera();
    elements.numera();
    numera_lista(constraints);
  }

//! @brief Moves the nodes.
void XC::SetMeshComp::mueve(const Vector3d &desplaz)
  { nodes.mueve(desplaz); }

//! @brief Aplies the transformation to the positions of the nodes.
void XC::SetMeshComp::Transforma(const TrfGeom &trf)
  { nodes.transforma(trf); }

//! @brief Applies to the set the transformation with the identifier being passed as parameter.
void XC::SetMeshComp::Transforma(const size_t &indice_trf)
  {
    TrfGeom *trf= getPreprocessor()->getMultiBlockTopology().getTransformacionesGeometricas().busca(indice_trf);
    if(trf)
      Transforma(*trf);
  }

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

//! @brief Calcula los esfuerzos on each uno de los elements.
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


//! @brief Returns the tags de los elements.
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
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; implementation pending." << std::endl;
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
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; preprocessor needed." << std::endl;
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
      std::clog << getClassName() << "::" << __FUNCTION__
	        << " node container empty. Call fillDownwards?"
	        << std::endl;
    retval+= elements.Bnd(factor);
    return retval;
  }

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
          std::cerr << getClassName() << __FUNCTION__
	            << "; preprocessor needed." << std::endl;
      }
  }

//! @brief Return a new set that contains the elements that lie insiof the
//! geometric object.
//!
//! @param newSetName: name for the new set.
//! @param geomObj: geometric object that must contain the elements.
//! @param tol: tolerance for "In" function.
XC::SetMeshComp XC::SetMeshComp::pickElemsInside(const std::string &newSetName, const GeomObj3d &geomObj, const double &tol)
  {
    SetMeshComp retval(newSetName);
    retval.elements= elements.pickElemsInside(geomObj, tol);
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
    return retval;
  }

//! @brief Select the constraints identified by the tags.
//!
//! @param tags: identifiers of the constraints.
void XC::SetMeshComp::sel_constraints_from_list(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= getPreprocessor();
        Constraint *tmp= nullptr;
        if(preprocessor)
          for(size_t i= 0;i<sz;i++)
            {
              if((tmp= preprocessor->getDomain()->getConstraints().getSFreedom_Constraint(tags(i))))
                constraints.push_back(tmp);
              else if((tmp=preprocessor->getDomain()->getConstraints().getMFreedom_Constraint(tags(i))))
                constraints.push_back(tmp);
              else
		std::cerr << getClassName() << "::" << __FUNCTION__
		          << "; constraint idenfified by: "
                          << tags(i) << " not found." << std::endl;
            }

        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "preprocessor needed." << std::endl;
      }
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::SetMeshComp::getDbTagData(void) const
  {
    static DbTagData retval(19);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::SetMeshComp::sendData(CommParameters &cp)
  {
    int res= SetBase::sendData(cp);
    res+= nodes.sendTags(3,4,getDbTagData(),cp);
    res+= elements.sendTags(5,6,getDbTagData(),cp);
    res+= constraints.sendTags(7,8,getDbTagData(),cp);
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::SetMeshComp::recvData(const CommParameters &cp)
  {
    ID tmp;
    int res= SetBase::recvData(cp);
    tmp= nodes.receiveTags(3,4,getDbTagData(),cp);
    sel_nodes_from_list(tmp);
    tmp= elements.receiveTags(5,6,getDbTagData(),cp);
    sel_elements_from_list(tmp);
    tmp= constraints.receiveTags(7,8,getDbTagData(),cp);
    sel_constraints_from_list(tmp);
    return res;
  }

//!  @brief Destructor.
XC::SetMeshComp::~SetMeshComp(void)
  { clearAll(); }

//! @brief Sends object through the channel being passed as parameter.
int XC::SetMeshComp::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::SetMeshComp::recvSelf(const CommParameters &cp)
  {
    inicComm(19);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Return the union of both objects.
XC::SetMeshComp XC::operator+(const XC::SetMeshComp &a,const XC::SetMeshComp &b)
  {
    XC::SetMeshComp retval(a);
    retval+=b;
    return retval;
  }

//! @brief Return the difference.
XC::SetMeshComp XC::operator-(const XC::SetMeshComp &a,const XC::SetMeshComp &b)
  {
    XC::SetMeshComp retval(a);
    retval-=b;
    return retval;
  }

//! @brief Return the intersection.
XC::SetMeshComp XC::operator*(const XC::SetMeshComp &a,const XC::SetMeshComp &b)
  {
    XC::SetMeshComp retval(a);
    retval*=b;
    return retval;
  }
