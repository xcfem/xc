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
//Mesh.h

#ifndef Mesh_h
#define Mesh_h

#include "domain/mesh/MeshComponentContainer.h"
#include "utility/matrix/Vector.h"
#include "NodeLockers.h"
#include "solution/graph/graph/Graph.h"
#include "node/KDTreeNodes.h"
#include "element/utils/KDTreeElements.h"

class Pos3d;

namespace XC {
class Element;
class Node;

class ElementIter;
class NodeIter;
class SingleDomEleIter;
class SingleDomNodIter;

class Graph;
class NodeGraph;
class ElementGraph;
class FEM_ObjectBroker;
class TaggedObjectStorage;
class RayleighDampingFactors;

//! \ingroup Dom
//
//! \defgroup Mesh Finite element mesh.
//
//! \ingroup Mesh
//
//! @brief Finite element mesh.
class Mesh: public MeshComponentContainer
  {
  private:
    bool eleGraphBuiltFlag;
    bool nodeGraphBuiltFlag;

    Graph theNodeGraph;
    Graph theElementGraph;

    TaggedObjectStorage *theNodes; //!< Node container.
    SingleDomNodIter *theNodIter; //!< Node iterator.
    KDTreeNodes kdtreeNodes; //!< space-partitioning data structure for organizing nodes.
    std::vector<std::string> nombresCoordenadas; //!< Coordinate names ("X","Y","Z",...).
    std::string nombreUnidades; //!< Units names ("cm","m","in",...).

    TaggedObjectStorage *theElements;
    SingleDomEleIter *theEleIter;
    KDTreeElements kdtreeElements; //!< space-partitioning data structure for organizing nodes. Search finite element by its position (x,y,x).

    Vector theBounds;
    int tagNodeCheckReactionException;//!< Exception for checking reactions (see Domain::checkNodalReactions).

    NodeLockers lockers; //!< To block deactivated (dead) nodes.

    void alloc_containers(void);
    void alloc_iters(void);
    bool check_containers(void) const;
    void init_bounds(void);
    void update_bounds(const Vector &);
    void add_node_to_domain(Node *);
    void add_element_to_domain(Element *);
    void add_nodes_to_domain(void);
    void add_elements_to_domain(void);

    Mesh(const Mesh &otra);
    Mesh &operator=(const Mesh &otra);
  protected:
    void free_mem(void);
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    Mesh(EntCmd *owr);
    Mesh(EntCmd *owr,TaggedObjectStorage &theNodesStorage, TaggedObjectStorage &theElementsStorage);
    Mesh(EntCmd *owr,TaggedObjectStorage &theStorageType);
    virtual ~Mesh(void);

    // methods to populate a mesh
    virtual bool addNode(Node *);
    virtual bool removeNode(int tag);

    virtual bool addElement(Element *);
    virtual bool removeElement(int tag);

    virtual void clearAll(void);

    void setNodeReactionException(const int &);
    void checkNodalReactions(const double &);

    void clearDOF_GroupPtr(void);

    void setGraphBuiltFlags(const bool &f);

    int initialize(void);
    virtual int setRayleighDampingFactors(const RayleighDampingFactors &rF);

     // methods to access the components of a mesh
    inline const TaggedObjectStorage *nodes(void) const
      { return theNodes; }
    inline TaggedObjectStorage *nodes(void)
      { return theNodes; }
    inline const TaggedObjectStorage *elements(void) const
      { return theElements; }
    inline TaggedObjectStorage *elements(void)
      { return theElements; }
    virtual ElementIter &getElements();
    virtual NodeIter &getNodes();
    inline const NodeLockers &getNodeLockers(void) const
      { return lockers; }
    inline NodeLockers &getNodeLockers(void)
      { return lockers; }

    bool existElement(int tag);
    virtual Element *getElement(int tag);
    virtual const Element *getElement(int tag) const;
    Element *getNearestElement(const Pos3d &p);
    const Element *getNearestElement(const Pos3d &p) const;
    bool existNode(int tag);
    virtual Node *getNode(int tag);
    virtual const Node *getNode(int tag) const;
    Node *getNearestNode(const Pos3d &p);
    const Node *getNearestNode(const Pos3d &p) const;

    // methods to query the state of the mesh
    virtual int getNumElements(void) const;
    virtual int getNumNodes(void) const;
    size_t getNumDeadElements(void) const;
    size_t getNumLiveElements(void) const;
    size_t getNumDeadNodes(void) const;
    size_t getNumLiveNodes(void) const;
    size_t getNumFrozenNodes(void) const;
    size_t getNumFreeNodes(void) const;
    virtual const Vector &getPhysicalBounds(void);

    inline const std::vector<std::string> &getNombresCoordenadas(void) const
      { return nombresCoordenadas; }
    inline std::string getNombreUnidades(void) const
      { return nombreUnidades; }


     // methods to get element and node graphs
    virtual int buildEleGraph(Graph &theEleGraph);
    virtual int buildNodeGraph(Graph &theNodeGraph);
    virtual Graph &getElementGraph(void);
    virtual Graph &getNodeGraph(void);

    virtual int commit(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);
    int update(void);

    void freeze_dead_nodes(const std::string &nmbLocker);
    void melt_alive_nodes(const std::string &nmbLocker);

    const double getEffectiveModalMass(int mode) const;
    Vector getEffectiveModalMasses(const int &numModes) const;

    void zeroLoads(void);

    virtual void Print(std::ostream &s, int flag =0);
    friend std::ostream &operator<<(std::ostream &s, Mesh &M);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    // nodal methods required in mesh interface for parallel interprter
    virtual double getNodeDisp(int nodeTag, int dof, int &errorFlag);
    virtual int setMass(const Matrix &mass, int nodeTag);

    virtual int calculateNodalReactions(bool inclInertia, const double &);

    static void setDeadSRF(const double &);
  };
} // end of XC namespace

#endif


