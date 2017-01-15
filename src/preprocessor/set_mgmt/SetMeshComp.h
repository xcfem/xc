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
//  una información más detallada.
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
class SVD3d;
class Plano3d;

namespace XC {
class TrfGeom;
class SFreedom_Constraint;
class ID;
class Element;
class Node;
class Constraint;

class LimiteElemento
  {
    Element *ptr_elemento; //!< Puntero al elemento.
    int indice; //!< Índice del borde, arista o cara del elemento.
  };

//!  \ingroup Set
//! 
//!  \brief Conjunto de objetos.
//! 
//!  Un objeto SetMeshComp contiene un conjunto de 0 o más:
//!  - Nodos.
//!  - Elementos finitos.
//!  - Condiciones de contorno en desplazamientos.
class SetMeshComp: public SetBase
  {
  protected:
    template <class L>
    static void numera_lista(L &l);
    void numera(void);
  public:
    //Nodos.
    typedef DqPtrsNode::iterator nod_iterator; //!< Iterador sobre el conjunto de nodos.
    typedef DqPtrsNode::const_iterator nod_const_iterator; //!< Iterador sobre el conjunto de nodos.

    //Elementos.
    typedef DqPtrsElem::iterator elem_iterator; //!< Iterador sobre el conjunto de elementos.
    typedef DqPtrsElem::const_iterator elem_const_iterator; //!< Iterador sobre el conjunto de elementos.

    //Constraints.
    typedef DqPtrsConstraint lst_ptr_constraints; //!< Conjunto de constraints.
    typedef DqPtrsConstraint::iterator constraint_iterator; //!< Iterador sobre el conjunto de constraints.
    typedef DqPtrsConstraint::const_iterator constraint_const_iterator; //!< Iterador sobre el conjunto de constraints.

  private:
    DqPtrsNode nodos; //!< Conjunto de nodos.
    DqPtrsElem elementos; //!< Conjunto de elementos.
    lst_ptr_constraints constraints; //!< Conjunto de constraints.
  protected:
    void clearAll(void);
    void copia_listas(const SetMeshComp &);
    void agrega_listas(const SetMeshComp &);
    void agrega_listas_cond(const SetMeshComp &,const std::string &);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);


    void calc_resisting_force(void);

    void mueve(const Vector3d &);
  public:
    SetMeshComp(const std::string &nmb="",Preprocessor *preprocessor= NULL);
    SetMeshComp(const SetMeshComp &otro);
    SetMeshComp &operator=(const SetMeshComp &otro);

    //! @brief Devuelve el número de nodos del conjunto.
    size_t NumNodos(void) const
      { return nodos.size(); }
    //! @brief Agrega un nodo a la lista de nodos del conjunto.
    void agregaNodo(Node *nPtr);
    //! @brief Devuelve la lista de nodos del conjunto.
    virtual const DqPtrsNode &GetNodos(void) const
      { return nodos; }
    //! @brief Devuelve la lista de nodos del conjunto.
    virtual DqPtrsNode &GetNodos(void)
      { return nodos; }
    //! @brief Borra la lista de nodos del conjunto.
    void clearNodos(void)
      { nodos.clearAll(); }
    void sel_nodos_lista(const ID &);
    bool In(const Node *) const;

    //! @brief Devuelve el número de elementos del conjunto.
    size_t NumElementos(void) const
      { return elementos.size(); }
    //! @brief Agrega un elemento la lista de elementos del conjunto.
    void agregaElemento(Element *ePtr);
    //! @brief Devuelve la lista de elementos del conjunto.
    virtual const DqPtrsElem &GetElementos(void) const
      { return elementos; }
    //! @brief Devuelve la lista de elementos del conjunto.
    virtual DqPtrsElem &GetElementos(void)
      { return elementos; }
    //! @brief Borra la lista de elementos del conjunto.
    void clearElementos(void)
      { elementos.clearAll(); }
    void sel_elementos_lista(const ID &tags);
    bool In(const Element *) const;

    //! @brief Devuelve la lista de constraints del conjunto.
    virtual const DqPtrsConstraint &GetConstraints(void) const
      { return constraints; }
    //! @brief Devuelve la lista de constraints del conjunto.
    virtual DqPtrsConstraint &GetConstraints(void)
      { return constraints; }
    //! @brief Borra la lista de constraints del conjunto.
    void clearConstraints(void)
      { constraints.clearAll(); }
    void sel_constraints_lista(const ID &tags);

    void clear(void);

    void fix(const SFreedom_Constraint &);

    //nod_iterator nodos_begin(void);
    nod_const_iterator nodos_begin(void) const;
    //nod_iterator nodos_end(void);
    nod_const_iterator nodos_end(void) const;
    Node *buscaNodo(const int &tag);
    const Node *buscaNodo(const int &tag) const;
    bool InNodeTag(const int tag_nodo) const;
    bool InNodeTags(const ID &tag_nodos) const;
    std::set<int> getNodeTags(void) const;
    std::set<int> getElementTags(void) const;
    std::set<int> getConstraintTags(void) const;
    Node *getNearestNode(const Pos3d &p);
    const Node *getNearestNode(const Pos3d &p) const;


    elem_iterator elem_begin(void);
    elem_const_iterator elem_begin(void) const;
    elem_iterator elem_end(void);
    elem_const_iterator elem_end(void) const;
    Element *buscaElemento(const int &tag);
    const Element *buscaElemento(const int &tag) const;
    Element *getNearestElement(const Pos3d &p);
    const Element *getNearestElement(const Pos3d &p) const;

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

    void CompletaHaciaArriba(void);
    void CompletaHaciaAbajo(void);

    virtual void Transforma(const TrfGeom &trf);
    virtual void Transforma(const size_t &indice_trf);

    SVD3d getResistingSVD3d(const Plano3d &,const Pos3d &,const double &,const bool &) const;

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);


    ~SetMeshComp(void);
  };

//!  @brief Asigna índices a los objetos de la lista para poder emplearlos en VTK.
template <class L>
void XC::SetMeshComp::numera_lista(L &l)
  {
    size_t idx= 0;
    typedef typename L::iterator iterator;
    typedef typename L::reference reference;
    for(iterator i= l.begin();i!=l.end();i++,idx++)
      {
	reference ptr= *i;
        ptr->set_indice(idx);
      }
  }

} //end of XC namespace
#endif
