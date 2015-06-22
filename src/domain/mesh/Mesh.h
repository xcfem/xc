//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//Mesh.h

#ifndef Mesh_h
#define Mesh_h

#include "domain/mesh/MeshComponentContainer.h"
#include "utility/matrix/Vector.h"
#include "NodeLockers.h"
#include "solution/graph/graph/Graph.h"
#include "node/KDTreeNodes.h"
#include "element/KDTreeElements.h"

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
//! \defgroup Mesh Malla del modelo de elementos finitos.
//
//! \ingroup Mesh
//
//! @brief Nodos y elementos de la malla de elementos finitos.
class Mesh: public MeshComponentContainer
  {
  private:
    bool eleGraphBuiltFlag;
    bool nodeGraphBuiltFlag;

    Graph theNodeGraph;
    Graph theElementGraph;

    TaggedObjectStorage *theNodes; //!< Contenedor de nodos.
    SingleDomNodIter *theNodIter; //!< Iterador sobre nodos.
    KDTreeNodes kdtreeNodos; //!< KDTree para acelerar la búsqueda de nodos por coordenadas.
    std::vector<std::string> nombresCoordenadas; //!< Nombres de las coordenadas ("X","Y","Z",...).
    std::string nombreUnidades; //!< Nombres de las unidades ("cm","m","in",...).

    TaggedObjectStorage *theElements;
    SingleDomEleIter *theEleIter;
    KDTreeElements kdtreeElements; //!< KDTree para acelerar la búsqueda de elementos por coordenadas.

    Vector theBounds;
    int tagNodeCheckReactionException;//!< excepción para comprobación de reacciones (ver Domain::checkNodalReactions).

    NodeLockers lockers; //!< Para bloquear los nodos desactivados.

    void alloc_contenedores(void);
    void alloc_iters(void);
    bool check_contenedores(void) const;
    void init_bounds(void);
    void update_bounds(const Vector &);
    void add_node_to_domain(Node *);
    void add_element_to_domain(Element *);
    void add_nodes_to_domain(void);
    void add_elements_to_domain(void);

    Mesh(const Mesh &otra);
    Mesh &operator=(const Mesh &otra);
  protected:
    void libera(void);
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
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

    void EjecutaBloqueForEachElement(CmdStatus &,const std::string &);
    void EjecutaBloqueForEachNode(CmdStatus &,const std::string &);

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

    virtual any_const_ptr GetProp(const std::string &cod) const;
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
} // fin namespace XC

#endif


