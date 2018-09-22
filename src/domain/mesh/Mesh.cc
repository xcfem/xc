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
//Mesh.cc


#include "Mesh.h"
#include "domain/domain/Domain.h"
#include "utility/recorder/Recorder.h"
#include <cstdlib>
#include "domain/mesh/element/utils/RayleighDampingFactors.h"

#include <domain/mesh/element/Element.h>
#include <domain/mesh/node/Node.h>
#include "domain/mesh/element/ElementIter.h"
#include "domain/mesh/node/NodeIter.h"
#include <domain/domain/single/SingleDomEleIter.h>
#include <domain/domain/single/SingleDomNodIter.h>

#include <utility/tagged/storage/MapOfTaggedObjects.h>
#include <utility/tagged/storage/MapOfTaggedObjectsIter.h>

#include <solution/graph/graph/Vertex.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>


#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include <climits>
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

#include "utility/actor/actor/MovableVector.h"

//! @brief Frees memory occupied by mesh components.
//! this calls delete on all components of the model,
//! i.e. calls delete on all that is added to the model.
//! WARNING: if 3rd constructor, TaggedObjectStorage objects passed
//! must have been created with new_ and nowhere else must the
//! destructor be called.
void XC::Mesh::free_mem(void)
  {
    clearAll(); //delete the objects in the mesh

    // delete all the storage objects
    // SEGMENT FAULT WILL OCCUR IF THESE OBJECTS WERE NOT CONSTRUCTED
    // USING NEW
    if(theEleIter) delete theEleIter;
    theEleIter= nullptr;
    if(theElements) delete theElements;
    theElements= nullptr;
    if(theNodIter) delete theNodIter;
    theNodIter= nullptr;
    if(theNodes) delete theNodes;
    theNodes= nullptr;
  }

//! @brief Allocates memory for containers.
void XC::Mesh::alloc_containers(void)
  {
    // init the arrays for storing the mesh components
    theNodes= new MapOfTaggedObjects(this,"node");
    theElements= new MapOfTaggedObjects(this,"element");
  }

//! @brief Allocates memory for iterators.
void XC::Mesh::alloc_iters(void)
  {
    // init the iters
    theNodIter= new SingleDomNodIter(theNodes);
    theEleIter= new SingleDomEleIter(theElements);
  }

//! @brief Check that there is enough memory for the containers.
bool XC::Mesh::check_containers(void) const
  {
    // check that there was space to create the data structures
    if (theElements ==0 || theNodes == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; out of memory.\n";
        return false;
      }
    else
      return true;
  }

//! @brief Inicializa los el BND del domain.
void XC::Mesh::init_bounds(void)
  { theBounds.Zero(); }

//! @brief Constructor.
XC::Mesh::Mesh(CommandEntity *owr)
  :MeshComponentContainer(owr,DOMAIN_TAG_Mesh), eleGraphBuiltFlag(false), nodeGraphBuiltFlag(false),
   theBounds(6), lockers(this)
  {
    alloc_containers();
    alloc_iters();
    check_containers();
    init_bounds();
    tagNodeCheckReactionException= -1;
  }

//! @brief Constructor.
XC::Mesh::Mesh(CommandEntity *owr,TaggedObjectStorage &theNodesStorage,TaggedObjectStorage &theElementsStorage)
  : MeshComponentContainer(owr,DOMAIN_TAG_Mesh), eleGraphBuiltFlag(false),
    nodeGraphBuiltFlag(false), theNodes(&theNodesStorage), theElements(&theElementsStorage), theBounds(6), lockers(this)
  {
    // init the iters
    alloc_iters();
    if(!check_containers()) exit(-1);

    // check that the containers are empty
    if(theElements->getNumComponents()!=0 || theNodes->getNumComponents()!=0)
       std::cerr << getClassName() << "::" << __FUNCTION__
		 << "(&, & ...) - out of memory.\n";
    init_bounds();
    tagNodeCheckReactionException= -1;
  }


//! @brief Constructor.
XC::Mesh::Mesh(CommandEntity *owr,TaggedObjectStorage &theStorage)
  : MeshComponentContainer(owr,DOMAIN_TAG_Mesh),
    eleGraphBuiltFlag(false), nodeGraphBuiltFlag(false),
    theBounds(6), lockers(this)
  {
    // init the arrays for storing the mesh components
    theStorage.clearAll(); // clear the storage just in case populated
    theElements = &theStorage;
    theNodes= theStorage.getEmptyCopy();

    alloc_iters();// init the iters
    check_containers();
    init_bounds();
    tagNodeCheckReactionException= -1;
  }


//! @brief Deletes from domain all its components (nodes, elements, loads and constraints).
//! GENERAL NOTE ON REMOVAL OF COMPONENTS:
//!   downward casts (while bad) are o.k. as only the type
//!  of components can be added to the storage objects, e.g.
//!  only elements can be added to theElements therefore
//!  casting a XC::MeshComponent from theElements to an XC::Element is o.k.
void XC::Mesh::clearAll(void)
  {
    // clean out the containers
    if(theElements) theElements->clearAll();
    if(theNodes) theNodes->clearAll();
    lockers.clearAll();

    // set the bounds around the origin
    theBounds.Zero();


    // rest the flag to be as initial
    nodeGraphBuiltFlag= false;
    eleGraphBuiltFlag= false;
  }

//! @brief Destructor.
XC::Mesh::~Mesh(void)
  { free_mem(); }


//! @brief Assigns Stress Reduction Factor for element deactivation.
void XC::Mesh::setDeadSRF(const double &d)
  { Element::setDeadSRF(d); }

//! @brief Appends an element to the mesh.
void XC::Mesh::add_element_to_domain(Element *element)
  {
    Domain *dom= getDomain();
    element->setDomain(dom);
    element->update();

    // mark the domain as having been changed
    dom->domainChange();
    kdtreeElements.insert(*element);
  }
//! @brief Must only to be called from recvSelf.
void XC::Mesh::add_elements_to_domain(void)
  {
    Domain *dom= getDomain();
    Element *elePtr;
    ElementIter &theElemIter = this->getElements();
    while((elePtr = theElemIter()) != 0)
      {
        elePtr->setDomain(dom);
        elePtr->update();
        // mark the domain as having been changed
        dom->domainChange();
        kdtreeElements.insert(*elePtr);
      }
  }

//! @brief Appends to the mesh the element being passed as parameter.
//!
//! To add the element pointed to by theElementPtr to the domain. 
//! In addition the domain always checks to ensure
//! that no other element with the same tag exists in the domain.
//! If the checks are successful, the element is added to
//! the mesh by invoking addComponent(theElePtr) on
//! the container for the elements. The mesh then invokes 
//! add_element_to_domain() on the element. The call returns true
//! if the element is added, otherwise an error message is
//! displayed ant false is returned.
bool XC::Mesh::addElement(Element *element)
  {
    if(!element)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; pointer to element is null." << std::endl;
        return false;
      }
    const int eleTag = element->getTag();


    // check if an Element with the same tag already exists in the XC::Mesh
    TaggedObject *other = theElements->getComponentPtr(eleTag);
    if(other)
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; element with tag " << eleTag
		  << " already exists in model.\n";
        return false;
      }


    // add the element to the container object for the elements
    bool result = theElements->addComponent(element);
    if(result)
      add_element_to_domain(element);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; element " << eleTag
		<< " could not be added to container.\n";
    return result;
  }

//! @brief Actualiza los límites del domain.
void XC::Mesh::update_bounds(const Vector &crds)
  {
    const int dim = crds.Size();
    if(dim >= 1)
      {
        const double x = crds(0);
        if(x < theBounds(0)) theBounds(0) = x;
        if(x > theBounds(3)) theBounds(3) = x;
      }
    if(dim >= 2)
      {
        const double y = crds(1);
        if(y < theBounds(1)) theBounds(1) = y;
        if(y > theBounds(4)) theBounds(4) = y;
      }
    if(dim == 3)
      {
        const double z = crds(2);
        if(z < theBounds(2)) theBounds(2) = z;
        if(z > theBounds(5)) theBounds(5) = z;
      }
  }

//! @brief Adds a node to the domain.
void XC::Mesh::add_node_to_domain(Node *node)
  {
    Domain *dom= getDomain();
    node->setDomain(dom);
    dom->domainChange();
    update_bounds(node->getCrds());
    kdtreeNodes.insert(*node);
  }

//! @brief Must only to be called from recvSelf.
void XC::Mesh::add_nodes_to_domain(void)
  {
    Domain *dom= getDomain();
    Node *nodePtr= nullptr;
    NodeIter &theNodeIter = this->getNodes();
    while((nodePtr = theNodeIter()) != nullptr)
      {
        nodePtr->setDomain(dom);
        dom->domainChange();
        update_bounds(nodePtr->getCrds());
        kdtreeNodes.insert(*nodePtr);
      }
  }

//! @brief Adds to the domain the node being passed as parameter.
//!
//! To add the node pointed to by \p node to the mesh. 
//! The domain first checks that no other node with the same tag,
//! has been previously added to the domain. The
//! mesh will then add the node to it's node container object, by
//! invoking addComponent(node). If successful, the mesh
//! invokes add_node_to_domain(node) on the node.
//! The call returns \p true if the
//! node was added, otherwise an error is printed and false is returned.
bool XC::Mesh::addNode(Node * node)
  {
    int nodTag = node->getTag();

    TaggedObject *other = theNodes->getComponentPtr(nodTag);
    if(other)
      {
        std::clog << getClassName() << "::" << __FUNCTION__
	          << "; node with tag " << nodTag
		  << " already exists in model.\n";
        return false;
      }
    bool result= theNodes->addComponent(node);
    if(result)
      add_node_to_domain(node);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; node with tag " << nodTag
		<< " could not be added to container.\n";
    return result;
  }


//! @brief Deletes the element identified by the tag being passed as parameter.
//!
//! To remove the element whose tag is given by \p tag from the
//! mesh. This is achieved by invoking {\em removeComponent(tag)} on the
//! container for the elements. 
//! Returns \f$0\f$ if no such element exists in the domain. Otherwise 
//! the domain invokes {\em setDomain(nullptr)} on the element (using a cast to
//! go from a TaggedObject to an Element, which is safe as only an
//! Element objects are added to this container) and {\em
//! domainChange()} on itself before a pointer to the element is returned. 
bool XC::Mesh::removeElement(int tag)
  {
    // remove the object from the container
    bool res= theElements->removeComponent(tag);
    
    if(res)
      {
        Domain *dom= getDomain();

        Element *elem= dom->getElement(tag);
        if(elem) kdtreeElements.erase(*elem);

        dom->domainChange(); //mark the domain as having changed
      }
    return res;
  }

//! @brief Remove from mesh the node identified by the argument.
//!
//! To remove the node whose tag is given by \p tag from the domain. 
//! This is achieved ty by invoking {\em removeComponent(tag)} on the container
//! for the nodes. 
//! Returns \f$0\f$ if no such node exists in the domain. If the node is to be
//! removed the domain invokes {\em setDomain(nullptr)} on the node and {\em
//! domainChange()} on itself before a pointer to the Node is returned. 
bool XC::Mesh::removeNode(int tag)
  {

    // remove the object from the container
    bool res= theNodes->removeComponent(tag);

    if(res)
      {
        Domain *dom= getDomain();

        Node *nod= dom->getNode(tag);
        if(nod) kdtreeNodes.erase(*nod);

        // mark the domain has having changed
        dom->domainChange();
      }
    return res;
  }

//! @brief Clears the pointers to node DOF groups.
void XC::Mesh::clearDOF_GroupPtr(void)
  {
    Node *nodePtr= nullptr;
    NodeIter &theNodeIter = this->getNodes();
    while((nodePtr = theNodeIter()) != 0)
      nodePtr->setDOF_GroupPtr(nullptr);
  }

//! @brief Returns an iterator to the mesh elements.
XC::ElementIter &XC::Mesh::getElements()
  {
    theEleIter->reset();
    return *theEleIter;
  }

//! @brief Returns an iterator a the nodes del domain.
XC::NodeIter &XC::Mesh::getNodes()
  {
    theNodIter->reset();
    return *theNodIter;
  }

//! @brief Returns true if the mesh has an element with the tag being passed as parameter.
bool XC::Mesh::existElement(int tag)
 { return theElements->existComponent(tag); }


//! @brief Returns a pointer to the element identified by the tag being passed as parameter.
XC::Element *XC::Mesh::getElement(int tag)
  {
    Element *result= nullptr;
    TaggedObject *mc = theElements->getComponentPtr(tag);

    // if not there return 0 otherwise perform a cast and return that
    if(mc)
      result= dynamic_cast<Element *>(mc);
    return result;
  }

//! @brief Returns a pointer to the element identified by the tag being passed as parameter.
const XC::Element *XC::Mesh::getElement(int tag) const
  {
    const Element *result= nullptr;
    const TaggedObject *mc = theElements->getComponentPtr(tag);

    // if not there return 0 otherwise perform a cast and return that
    if(mc)
      result= dynamic_cast<const Element *>(mc);
    return result;
  }

//! @brief Returns a pointer to the nearest element to the point being passed as parameter.
XC::Element *XC::Mesh::getNearestElement(const Pos3d &p)
  {
    Element *retval= const_cast<Element *>(kdtreeElements.getNearest(p));
    return retval;
  }

//! @brief Returns a pointer to the nearest element to the point being passed as parameter.
const XC::Element *XC::Mesh::getNearestElement(const Pos3d &p) const
  {
    Mesh *this_no_const= const_cast<Mesh *>(this);
    return this_no_const->getNearestElement(p);
  }

//! @brief Returns true if the mesh has a node with the tag being passed as parameter.
bool XC::Mesh::existNode(int tag)
 { return theNodes->existComponent(tag); }

//! @brief Returns a pointer to the node which tag being passed as parameter.
XC::Node *XC::Mesh::getNode(int tag)
  {
    TaggedObject *mc = theNodes->getComponentPtr(tag);

    // if not there return 0 otherwise perform a cast and return that
    if(!mc) return nullptr;
    Node *result= dynamic_cast<Node *>(mc);
    return result;
  }

//! @brief Returns a pointer to the node which tag being passed as parameter.
const XC::Node *XC::Mesh::getNode(int tag) const
  {
    const TaggedObject *mc = theNodes->getComponentPtr(tag);

    // if not there return 0 otherwise perform a cast and return that
    if(!mc) return nullptr;
    const Node *result= dynamic_cast<const Node *>(mc);
    return result;
  }

//! @brief Returns the node closest to the point being passed as parameter.
XC::Node *XC::Mesh::getNearestNode(const Pos3d &p)
  {
    Node *retval= const_cast<Node *>(kdtreeNodes.getNearest(p));
    return retval;
  }

//! @brief Returns the node closest to the point being passed as parameter.
const XC::Node *XC::Mesh::getNearestNode(const Pos3d &p) const
  {
    Mesh *this_no_const= const_cast<Mesh *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Freezes inactive nodes (prescribes zero displacement for all DOFs
//! on inactive nodes).
void XC::Mesh::freeze_dead_nodes(const std::string &nmbLocker)
  {
    NodeLocker *locker= lockers.busca_node_locker(nmbLocker);
    if(!locker) //New case.
      locker= lockers.newNodeLocker(nmbLocker);
    if(locker)
      {
        NodeIter &theNodeIter= getNodes();
        Node *nodPtr= nullptr;
        while((nodPtr = theNodeIter()) != nullptr)
          nodPtr->freeze_if_dead(locker);
        getDomain()->addNodeLocker(locker);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; can't find locker: '"
		<< nmbLocker << "'\n";
  }

//! @brief Clears the constraints over activated nodes 
//! previously created by the freeze method.
void XC::Mesh::melt_alive_nodes(const std::string &nmbLocker)
  {
    NodeLocker *locker= lockers.busca_node_locker(nmbLocker);
    if(locker)
      {
        getDomain()->removeNodeLocker(locker); 
        NodeIter &theNodeIter= getNodes();
        Node *nodPtr= nullptr;
        while((nodPtr = theNodeIter()) != nullptr)
          nodPtr->melt_if_alive(locker);
        lockers.borraNodeLocker(nmbLocker);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; can't find locker: '"
		<< nmbLocker << "'\n";
  }

//! @brief Returns the number of elements.
int XC::Mesh::getNumElements(void) const
  { return theElements->getNumComponents(); }

//! @brief Returns the number of nodes.
int XC::Mesh::getNumNodes(void) const
  { return theNodes->getNumComponents(); }

//! @brief Returns the number of active elements on the mesh.
size_t XC::Mesh::getNumLiveElements(void) const
  {
    size_t retval= 0;
    Mesh *this_no_const= const_cast<Mesh *>(this);
    ElementIter &theElemIter = this_no_const->getElements();
    const Element *elePtr= nullptr;
    while((elePtr = theElemIter()) != nullptr)
      {
        if(elePtr->isAlive())
          retval++;
      }
    return retval;
  }

//! @brief Returns the number of inactive elements on the mesh.
size_t XC::Mesh::getNumDeadElements(void) const
  {
    size_t retval= 0;
    Mesh *this_no_const= const_cast<Mesh *>(this);
    ElementIter &theElemIter = this_no_const->getElements();
    const Element *elePtr= nullptr;
    while((elePtr = theElemIter()) != nullptr)
      {
        if(elePtr->isDead())
          retval++;
      }
    return retval;
  }

//! @brief Returns the number of active nodes on the mesh.
size_t XC::Mesh::getNumLiveNodes(void) const
  {
    size_t retval= 0;
    Mesh *this_no_const= const_cast<Mesh *>(this);
    NodeIter &theNodeIter= this_no_const->getNodes();
    const Node *nodPtr= nullptr;
    while((nodPtr = theNodeIter()) != nullptr)
      {
        if(nodPtr->isAlive())
          retval++;
      }
    return retval;
  }

//! @brief Returns the number of inactive nodes on the mesh.
size_t XC::Mesh::getNumDeadNodes(void) const
  {
    size_t retval= 0;
    Mesh *this_no_const= const_cast<Mesh *>(this);
    NodeIter &theNodeIter= this_no_const->getNodes();
    const Node *nodPtr= nullptr;
    while((nodPtr = theNodeIter()) != nullptr)
      {
        if(nodPtr->isDead())
          retval++;
      }
    return retval;
  }

//! @brief Returns the number of frozen nodes on the mesh.
size_t XC::Mesh::getNumFrozenNodes(void) const
  {
    size_t retval= 0;
    Mesh *this_no_const= const_cast<Mesh *>(this);
    NodeIter &theNodeIter= this_no_const->getNodes();
    const Node *nodPtr= nullptr;
    while((nodPtr = theNodeIter()) != nullptr)
      {
        if(nodPtr->isFrozen())
          retval++;
      }
    return retval;
  }

//! @brief Returns the number of free nodes on the mesh.
size_t XC::Mesh::getNumFreeNodes(void) const
  {
    size_t retval= 0;
    Mesh *this_no_const= const_cast<Mesh *>(this);
    NodeIter &theNodeIter= this_no_const->getNodes();
    const Node *nodPtr= nullptr;
    while((nodPtr = theNodeIter()) != nullptr)
      {
        if(nodPtr->isFree())
          retval++;
      }
    return retval;
  }

//! @brief Returns the boundary of the finite element model.
//!
//! To return the bounding rectangle for the mesh. The information is
//! contained in a Vector of size 6 containing in the following order
//! \{xmin, ymin, zmin, xmax, ymax, zmax\}. This information is built up
//! as nodes are added to the domain, initially all are set to \f$0\f$ in the
//! constructor. 
const XC::Vector &XC::Mesh::getPhysicalBounds(void)
  { return theBounds; }

//! @brief Builds the elements graph of the mesh (if not builded yet) and
//! returns a reference to it.
//! 
//! Returns the current element graph (the connectivity of the elements
//! in the mesh). If the \p eleChangeFlag has been set
//! to \p true the method will invoke {\em buildEleGraph(theEleGraph)}
//! on itself before returning the graph. The vertices in the element
//! graph are to be labeled \f$0\f$ through \f$numEle-1\f$. The vertices
//! references contain the elemental tags.  
XC::Graph &XC::Mesh::getElementGraph(void)
  {
    if(!eleGraphBuiltFlag)
      {
        // create an empty graph
        theElementGraph= Graph(this->getNumElements()+START_VERTEX_NUM);

        // now build the graph
        if(this->buildEleGraph(theElementGraph) == 0)
          eleGraphBuiltFlag = true;
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to build the element graph.\n";
      }
    // return the Graph
    return theElementGraph;
  }


//! @brief Builds (if needed) the graph of the domain nodes and
//! returns a reference to it.
//!
//! Returns the current node graph (the connectivity of the nodes in
//! the domain). If the \p nodeChangeFlag has been set to \p true the
//! will invoke {\em buildNodeGraph(theNodeGraph)} on itself before
//! returning the graph. The vertices in the node graph are to be labeled
//! \f$0\f$ through \f$numNode-1\f$. The Vertices references contain the nodal
//! tags.
XC::Graph &XC::Mesh::getNodeGraph(void)
  {
    if(!nodeGraphBuiltFlag)
      { // try to get a graph as big as we should need
        theNodeGraph= Graph(this->getNumNodes()+START_VERTEX_NUM);

        // now build the graph
        if(this->buildNodeGraph(theNodeGraph) == 0)
            nodeGraphBuiltFlag = true;
        else
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to build the node graph.\n";
      }
    // return the XC::Graph
    return theNodeGraph;
  }

//! @brief Return the masa modal efectiva 
//! corresponding to the mode i.
const double XC::Mesh::getEffectiveModalMass(int mode) const
  {
    double retval= 0;
    Node *nodePtr= nullptr;
    Mesh *this_no_const= const_cast<Mesh *>(this);
    NodeIter &theNodeIter = this_no_const->getNodes();
    while((nodePtr = theNodeIter()) != 0)
      retval+= nodePtr->getEffectiveModalMass(mode);
    return retval;
  }

//! @brief Returns the masas modales efectivas.
XC::Vector XC::Mesh::getEffectiveModalMasses(const int &numModes) const
  {
    Vector retval(numModes);
    for(int i= 1;i<=numModes;i++)
      retval[i-1]= getEffectiveModalMass(i);
    return retval;
  }

//! @brief Loop over nodes and elements getting them to first zero their loads
void XC::Mesh::zeroLoads(void)
  {
    Node *nodePtr= nullptr;
    NodeIter &theNodeIter = this->getNodes();
    while((nodePtr = theNodeIter()) != 0)
      nodePtr->zeroUnbalancedLoad();
    Element *elePtr;
    ElementIter &theElemIter = this->getElements();
    while((elePtr = theElemIter()) != 0)
      if(elePtr->isSubdomain() == false)
        elePtr->zeroLoad();
  }

//! @brief Inicializa.
int XC::Mesh::initialize(void)
  {
    Element *elePtr= nullptr;
    ElementIter &theElemIter = this->getElements();
    while((elePtr = theElemIter()) != 0)
      elePtr->getInitialStiff();
    return 0;
  }

//! @brief Set Rayleigh damping factors.
int XC::Mesh::setRayleighDampingFactors(const RayleighDampingFactors &rF)
  {
    int result = 0;
    Element *elePtr;
    ElementIter &theElemIter = this->getElements();
    while((elePtr = theElemIter()) != 0)
      result += elePtr->setRayleighDampingFactors(rF);

    Node *nodePtr;
    NodeIter &theNodeIter = this->getNodes();
    while((nodePtr = theNodeIter()) != 0)
      { result += nodePtr->setRayleighDampingFactor(rF.getAlphaM()); }
    return result;
  }

//! @brief Commits mesh state.
int XC::Mesh::commit(void)
  {
    // invoke commit on all nodes and elements in the mesh
    Node *nodePtr= nullptr;
    NodeIter &theNodeIter = this->getNodes();
    while((nodePtr = theNodeIter()) != 0)
      { nodePtr->commitState(); }

    Element *elePtr= nullptr;
    ElementIter &theElemIter = this->getElements();
    while((elePtr = theElemIter()) != 0)
      { elePtr->commitState(); }

    return 0;
  }

//! @brief Returns the mesh to its last commited state.
int XC::Mesh::revertToLastCommit(void)
  {
    //
    // first invoke revertToLastCommit  on all nodes and elements in the mesh
    //

    Node *nodePtr;
    NodeIter &theNodeIter = this->getNodes();
    while((nodePtr = theNodeIter()) != 0)
      nodePtr->revertToLastCommit();

    Element *elePtr;
    ElementIter &theElemIter = this->getElements();
    while((elePtr = theElemIter()) != 0)
      { elePtr->revertToLastCommit(); }

    return update();
  }

//! @brief Return the mesh into its initial state.
int XC::Mesh::revertToStart(void)
  {
    //
    // first invoke revertToStart  on all nodes and
    // elements in the mesh
    //

    Node *nodePtr;
    NodeIter &theNodeIter = this->getNodes();
    while((nodePtr = theNodeIter()) != 0)
      nodePtr->revertToStart();

    Element *elePtr;
    ElementIter &theElements = this->getElements();
    while((elePtr = theElements()) != 0)
      { elePtr->revertToStart(); }

    return update();
  }

//! @brief Update the element's state.
//! 
//! Called by the domain to update the state of the
//! mesh. Iterates over all the elements and invokes {\em update()}. 
int XC::Mesh::update(void)
  {
    int ok = 0;

    // invoke update on all the ele's
    ElementIter &theEles = this->getElements();
    Element *theEle;
    while((theEle = theEles()) != 0)
      { ok += theEle->update(); }

    if(ok != 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh failed in update.\n";
    return ok;
  }



//! @brief Returns true if the modelo ha cambiado.
void XC::Mesh::setGraphBuiltFlags(const bool &f)
  {
    nodeGraphBuiltFlag= f;
    eleGraphBuiltFlag= f;
  }

//! @brief Imprime el domain.
void XC::Mesh::Print(std::ostream &s, int flag)
  {

    s << "Current Mesh Information:\n";

    s << "\nNODE DATA: NumNodes: " << theNodes->getNumComponents() << "\n";
    theNodes->Print(s, flag);

    s << "\nELEMENT DATA: NumEle: " << theElements->getNumComponents() << "\n";
    theElements->Print(s, flag);
  }

std::ostream &XC::operator<<(std::ostream &s, Mesh &M)
  {
    M.Print(s);
    return s;
  }

//! @brief Builds the element's graph.
//!
//! A method which will cause the mesh to discard the current element
//! graph and build a new one based on the element connectivity. Returns
//! \f$0\f$ if successful otherwise \f$-1\f$ is returned along with an error
//! message. 
int XC::Mesh::buildEleGraph(Graph &theEleGraph)
  {
    int numVertex = this->getNumElements();

    // see if quick return

    if(numVertex == 0) return 0;


    // create another vertices array which aids in adding edges

    int maxEleNum = 0;
    Element *elePtr;
    ElementIter &eleIter = this->getElements();
    while((elePtr = eleIter()) != 0)
      if(elePtr->getTag() > maxEleNum)
        maxEleNum = elePtr->getTag();

    std::vector<int> theElementTagVertices(maxEleNum+1);

    for(int j=0; j<=maxEleNum; j++)
      theElementTagVertices[j] = -1;
    std::clog << getClassName() << "::" << __FUNCTION__
	      << "; buildEleGraph numVertex maxEleNum " << numVertex
	      << " " << maxEleNum << std::endl;
    // now create the vertices with a reference equal to the element number.
    // and a tag which ranges from 0 through numVertex-1

    ElementIter &eleIter2 = this->getElements();
    int count = START_VERTEX_NUM;
    while((elePtr = eleIter2()) != 0)
      {
        int ElementTag = elePtr->getTag();
        Vertex vrt(count,ElementTag);
        theEleGraph.addVertex(vrt);
        theElementTagVertices[ElementTag] = count++;
      }

    // We now need to determine which elements are asssociated with each node.
    // As this info is not in the XC::Node interface we must build it; which we
    // do using vertices for each node, when we addVertex at thes nodes we
    // will not be adding vertices but element tags.

    int maxNodNum = 0;
    Node *nodPtr;
    NodeIter &nodeIter = this->getNodes();
    while((nodPtr = nodeIter()) != 0)
      if(nodPtr->getTag() > maxNodNum)
        maxNodNum = nodPtr->getTag();

    std::vector<Vertex> theNodeTagVertices(maxNodNum+1);

    // now create the vertices with a reference equal to the node number.
    // and a tag which ranges from 0 through numVertex-1 and placed in
    // theNodeTagVertices at a position equal to the node's tag.

    NodeIter &nodeIter2 = this->getNodes();
    count= START_VERTEX_NUM;
    while((nodPtr = nodeIter2()) != 0)
      {
        int nodeTag = nodPtr->getTag();
        Vertex vrt(count++,nodeTag);
        theNodeTagVertices[nodeTag] = vrt;
      }

    // now add the the Elements to the nodes
    ElementIter &eleIter3 = this->getElements();

    while((elePtr = eleIter3()) != 0)
      {
        int eleTag = elePtr->getTag();
        const ID &id = elePtr->getNodePtrs().getExternalNodes();

        int size = id.Size();
        for(int i=0; i<size; i++)
          theNodeTagVertices[id(i)].addEdge(eleTag);
      }


    // now add the edges to the vertices of our element graph;
    // this is done by looping over the Node vertices, getting their
    // Adjacenecy and adding edges between elements with common nodes
    for(int k=0; k<=maxNodNum; k++)
      {
        const std::set<int> &id = theNodeTagVertices[k].getAdjacency();
        for(std::set<int>::const_iterator i=id.begin(); i!=id.end(); i++)
          {
            int Element1 = *i;
            int vertexTag1 = theElementTagVertices[Element1];

            for(std::set<int>::const_iterator j=id.begin(); j!=id.end(); j++)
              if(i!=j)
                {
                  const int Element2= *j;
                  const int vertexTag2 = theElementTagVertices[Element2];
                  // addEdge() adds for both vertices - do only once
                  if(vertexTag1 > vertexTag2)
                    theEleGraph.addEdge(vertexTag1,vertexTag2);
                  theEleGraph.addEdge(vertexTag2,vertexTag1);
                }
          }
      }
    return 0;
  }

//! @brief Builds the node graph.
//!
//! A method which will cause the mesh to discard the current node
//! graph and build a new one based on the node connectivity. Returns
//! \f$0\f$ if successful otherwise \f$-1\f$ is returned along with an error
//! message. 
int XC::Mesh::buildNodeGraph(Graph &theNodeGraph)
  {
    int numVertex = this->getNumNodes();

    if(numVertex == 0)
      { return 0; }

    // create another vertices array which aids in adding edges

    int maxNodNum= 0;
    Node *nodPtr= nullptr;
    NodeIter &nodeIter = this->getNodes();
    while((nodPtr = nodeIter()) != 0)
      if(nodPtr->getTag() > maxNodNum)
        maxNodNum = nodPtr->getTag();

    std::vector<int> theNodeTagVertices(maxNodNum+1,-1);

    // now create the vertices with a reference equal to the node number.
    // and a tag which ranges from START_VERTEX_NUM through
    // numNodes+START_VERTEX_NUM

    NodeIter &nodeIter2 = this->getNodes();
    int count = START_VERTEX_NUM;
    while((nodPtr = nodeIter2()) != 0)
      {
        int nodeTag = nodPtr->getTag();
        Vertex vrt(count,nodeTag);
        theNodeGraph.addVertex(vrt); // add the vertex to the graph
        theNodeTagVertices[nodeTag]= count++;
      }

    // now add the edges, by looping over the Elements, getting their
    // IDs and adding edges between all elements who share a node.

    Element *elePtr= nullptr;
    ElementIter &eleIter = this->getElements();

    while((elePtr = eleIter()) != 0)
      {
        const ID &id = elePtr->getNodePtrs().getExternalNodes();
        int size = id.Size();
        for(int i=0; i<size; i++)
          {
            int node1 = id(i);
            int vertexTag1 = theNodeTagVertices[node1];
            for(int j=0; j<size; j++)
              if(i != j)
                {
                  int node2 = id(j);
                  int vertexTag2 = theNodeTagVertices[node2];

                  // addEdge() adds for both vertices - do only once
                  if(vertexTag1 > vertexTag2)
                    theNodeGraph.addEdge(vertexTag1,vertexTag2);
                }
          }
      }
    return 0;
  }

//! @brief Returns a vector to store the dbTags
//! de los miembros of the clase.
XC::DbTagData &XC::Mesh::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::Mesh::sendData(CommParameters &cp)
  {
    int res=cp.sendMovable(*theNodes,getDbTagData(),CommMetaData(0));
    res+= cp.sendMovable(*theElements,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(theBounds,getDbTagData(),CommMetaData(2));
    res+= cp.sendInt(tagNodeCheckReactionException,getDbTagData(),CommMetaData(3));
    res+= cp.sendMovable(lockers,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Mesh::recvData(const CommParameters &cp)
  {
    int res= theNodes->recibe<Node>(getDbTagDataPos(0),cp,&FEM_ObjectBroker::getNewNode);
    add_nodes_to_domain();
    res+= theElements->recibe<Element>(getDbTagDataPos(1),cp,&FEM_ObjectBroker::getNewElement);
    add_elements_to_domain();

    res+= cp.receiveVector(theBounds,getDbTagData(),CommMetaData(2));
    res+= cp.receiveInt(tagNodeCheckReactionException,getDbTagData(),CommMetaData(3));
    res+= cp.receiveMovable(lockers,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Mesh::sendSelf(CommParameters &cp)
  {
    inicComm(5); 
    int res= sendData(cp);
    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
              << "; channel failed to send data.\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::Mesh::recvSelf(const CommParameters &cp)
  {
    // first we get the data about the state of the mesh for this cTag
    inicComm(5);
    int res= cp.receiveIdData(getDbTagData(),getDbTag());
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
              << "; channel failed to recv the initial ID.\n";
    else
      res+= recvData(cp);
    return res;
  }
  
//! @brief Returns the component of the displacement of the node wich the tag is being passed as parameter.
//! @param nodeTag: node identifier.
//! @param dof: degree of freedom component.
//! @param errorFlag: error indicator.
double XC::Mesh::getNodeDisp(int nodeTag, int dof,int &errorFlag)
  {
    double result = 0.0;
    errorFlag = 0;
    const Node *theNode= getNode(nodeTag);
    if(!theNode)
      {
        errorFlag = -1;
        return 0.0;
      }
    const Vector &disp = theNode->getTrialDisp();
    if(dof < disp.Size() && dof >= 0)
      {
        result= disp(dof);
      }
    return result;
  }

//! @brief Set the mass matrix for the node identified by the argument.
int XC::Mesh::setMass(const XC::Matrix &mass, int nodeTag)
  {
    Node *theNode = this->getNode(nodeTag);
    if(theNode == nullptr)
      { return -1;}
    return theNode->setMass(mass);
  }

//! @brief Assign the exception for reaction checking (see Mesh::checkNodalReactions).
void XC::Mesh::setNodeReactionException(const int &n)
  { tagNodeCheckReactionException= n; }

//! @brief Checks that all free nodes have zero reactions.
bool XC::Mesh::checkNodalReactions(const double &tol)
  {
    bool retval= true;
    const Node *theNode= nullptr;
    double max_norm_reac= 0.1; //Small enough.
    NodeIter &theNodes = this->getNodes();
    while((theNode = theNodes()) != 0)
      if(theNode->getTag()!=tagNodeCheckReactionException)
        max_norm_reac= std::max(max_norm_reac,theNode->getReaction().Norm2()); 

    theNode= nullptr;
    NodeIter &theNodes2 = this->getNodes();
    while((theNode = theNodes2()) != 0)
      if(theNode->getTag()!=tagNodeCheckReactionException)
        {
	  const bool tmp= theNode->checkReactionForce(max_norm_reac*tol);
	  if(retval) //if it's already false there is no need to check.
	    retval= tmp;
	}
    return retval;
  }

//! @brief Calculate nodal reaction forces and moments.
//! @param inclInertia: if True, the unbalance load vector for each node is calculated including inertial forces.
//! @param tol: tolerance for the checking of reactions.
int XC::Mesh::calculateNodalReactions(bool inclInertia, const double &tol)
  {
    Node *theNode= nullptr;
    NodeIter &theNodes = this->getNodes();
    while((theNode = theNodes()) != nullptr)
      { theNode->resetReactionForce(inclInertia); }

    Element *theElement= nullptr;
    ElementIter &theElements = this->getElements();
    while((theElement = theElements()) != 0)
      theElement->addResistingForceToNodalReaction(inclInertia);

    checkNodalReactions(tol);

    return 0;
  }


