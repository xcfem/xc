// -*-c++-*-
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
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//SetMeshComp.h

#ifndef SETMESHCOMP_H
#define SETMESHCOMP_H

#include "SetBase.h"
#include "DqPtrs.h"
#include "DqPtrsNode.h"
#include "DqPtrsElem.h"
#include "DqPtrsConstraint.h"
#include <set>

class Pos3d;
class SlidingVectorsSystem3d;
class Plane;

namespace XC {
class TrfGeom;
class SFreedom_Constraint;
class ID;
class Element;
class Node;
class Constraint;

//!  @ingroup Set
//! 
//!  @brief Set of mesh components (nodes, elements and constraints).
//! 
//!  A SetMeshComp object contains zero or more:
//!  - Nodes.
//!  - Finite elements.
//!  - Essential boundary conditions (displacements).
class SetMeshComp: public SetBase
  {
  protected:
    template <class L>
    static void numera_lista(L &l);
    void numera(void);
  public:
    //Nodes.
    typedef DqPtrsNode::iterator nod_iterator; //!< Iterator over the node set.
    typedef DqPtrsNode::const_iterator nod_const_iterator; //!< Iterator over the node set.

    //Elements.
    typedef DqPtrsElem::iterator elem_iterator; //!< Iterator over the element set.
    typedef DqPtrsElem::const_iterator elem_const_iterator; //!< Iterator over the element set.

    //Constraints.
    typedef DqPtrsConstraint lst_ptr_constraints; //!< Set of constraints.
    typedef DqPtrsConstraint::iterator constraint_iterator; //!< Iterator over the set de constraints.
    typedef DqPtrsConstraint::const_iterator constraint_const_iterator; //!< Iterator over the set de constraints.

  private:
    DqPtrsNode nodes; //!< node set.
    DqPtrsElem elements; //!< element set.
    lst_ptr_constraints constraints; //!< constraints set.

  protected:
    void extend_lists(const SetMeshComp &);
    void substract_lists(const SetMeshComp &);
    void intersect_lists(const SetMeshComp &);
    void copy_lists(const SetMeshComp &);
    
    void clearAll(void);

    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);


    void calc_resisting_force(void);

    void move(const Vector3d &);
  public:
    SetMeshComp(const std::string &nmb="",Preprocessor *preprocessor= nullptr);
    SetMeshComp(const SetMeshComp &);
    SetMeshComp &operator=(const SetMeshComp &);
    SetMeshComp &operator+=(const SetMeshComp &);
    SetMeshComp &operator-=(const SetMeshComp &);
    SetMeshComp &operator*=(const SetMeshComp &);

    void extend(const DqPtrsNode &);
    void extend(const DqPtrsElem &);
    void extend(const lst_ptr_constraints &);
    void extend(const SetMeshComp &);
    
    std::string getStrName(void) const;
    void newName(const std::string &);
    void rename(const std::string &);
    
    //! @brief Returns the number of nodes.
    size_t getNumberOfNodes(void) const
      { return nodes.size(); }
    //! @brief Appends a node.
    void addNode(Node *nPtr);
    //! @brief Return the node container.
    virtual const DqPtrsNode &getNodes(void) const
      { return nodes; }
    //! @brief Return the nodes container.
    virtual DqPtrsNode &getNodes(void)
      { return nodes; }
    //! @brief Sets the nodes.
    void setNodes(const DqPtrsNode &nds)
      { nodes= nds; }
    //! @brief Clears out the nodes.
    void clearNodes(void)
      { nodes.clearAll(); }
    void sel_nodes_from_list(const ID &);
    bool In(const Node *) const;
    SetMeshComp pickNodesInside(const std::string &, const GeomObj3d &, const double &tol= 0.0);
    BND3d Bnd(const double &) const;

    //! @brief Returns the number of elements.
    size_t getNumberOfElements(void) const
      { return elements.size(); }
    //! @brief Adds an element.
    void addElement(Element *ePtr);
    //! @brief Returns the element container.
    virtual const DqPtrsElem &getElements(void) const
      { return elements; }
    //! @brief Returns the elements container.
    virtual DqPtrsElem &getElements(void)
      { return elements; }
    //! @brief Sets the elements.
    void setElements(const DqPtrsElem &els)
      { elements= els; }
    //! @brief Clears out the elements.
    void clearElements(void)
      { elements.clearAll(); }
    void sel_elements_from_list(const ID &tags);
    bool In(const Element *) const;
    SetMeshComp pickElemsInside(const std::string &, const GeomObj3d &, const double &tol= 0.0);
    SetMeshComp pickElemsOfType(const std::string &, const std::string &);
    SetMeshComp pickElemsOfDimension(const std::string &, const size_t &);
    inline boost::python::list getElementTypesPy(void) const
      { return elements.getTypesPy(); }
    inline boost::python::list getElementDimensionsPy(void) const
      { return elements.getDimensionsPy(); }
    inline boost::python::list getElementMaterialNamesPy(void) const
      { return elements.getMaterialNamesPy(); }
    SetMeshComp pickElemsOfMaterial(const std::string &, const std::string &);

    //! @brief Return the constraints container.
    virtual const DqPtrsConstraint &GetConstraints(void) const
      { return constraints; }
    //! @brief Return the constraints container.
    virtual DqPtrsConstraint &GetConstraints(void)
      { return constraints; }
    //! @brief Sets the constraints.
    void setConstraints(const DqPtrsConstraint &cts)
      { constraints= cts; }
    //! @brief Clears out the constraints.
    void clearConstraints(void)
      { constraints.clearAll(); }
    void sel_constraints_from_list(const ID &tags);

    void appendFromGeomEntity(const SetBase &);

    void clear(void);
    bool empty(void) const;    

    void fix(const SFreedom_Constraint &);

    //nod_iterator nodes_begin(void);
    nod_const_iterator nodes_begin(void) const;
    //nod_iterator nodes_end(void);
    nod_const_iterator nodes_end(void) const;
    Node *findNode(const int &tag);
    const Node *findNode(const int &tag) const;
    bool InNodeTag(const int) const;
    bool InNodeTags(const ID &) const;
    std::set<int> getNodeTags(void) const;
    std::set<int> getElementTags(void) const;
    std::set<int> getConstraintTags(void) const;
    Node *getNearestNode(const Pos3d &p);
    const Node *getNearestNode(const Pos3d &p) const;


    elem_iterator elem_begin(void);
    elem_const_iterator elem_begin(void) const;
    elem_iterator elem_end(void);
    elem_const_iterator elem_end(void) const;
    Element *findElement(const int &);
    const Element *findElement(const int &) const;
    Element *getNearestElement(const Pos3d &);
    const Element *getNearestElement(const Pos3d &) const;

    void kill_elements(void);
    void alive_elements(void);
    size_t getNumDeadElements(void) const;
    size_t getNumLiveElements(void) const;
    size_t getNumDeadNodes(void) const;
    size_t getNumLiveNodes(void) const;

    constraint_iterator constraints_begin(void);
    constraint_const_iterator constraints_begin(void) const;
    constraint_iterator constraints_end(void);
    constraint_const_iterator constraints_end(void) const;
    Constraint *buscaConstraint(const int &tag);
    const Constraint *buscaConstraint(const int &tag) const;

    void fillUpwards(void);
    void fillDownwards(void);

    virtual void Transform(const TrfGeom &trf);

    SlidingVectorsSystem3d getResistingSlidingVectorsSystem3d(const Plane &,const Pos3d &,const double &,const bool &) const;
    Matrix getTangentStiff(const Node &);
    Matrix getInitialStiff(const Node &);

    void createInertiaLoads(const Vector &);

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);

    SetMeshComp operator+(const SetMeshComp &) const;
    SetMeshComp operator-(const SetMeshComp &) const;
    SetMeshComp operator*(const SetMeshComp &) const;

    ~SetMeshComp(void);
  };


//!  @brief Set indices to the object to allow its use in VTK.
template <class L>
void XC::SetMeshComp::numera_lista(L &l)
  {
    size_t idx= 0;
    typedef typename L::iterator iterator;
    typedef typename L::reference reference;
    for(iterator i= l.begin();i!=l.end();i++,idx++)
      {
	reference ptr= *i;
        ptr->set_index(idx);
      }
  }

} //end of XC namespace
#endif
