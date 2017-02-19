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
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "preprocessor/cad/trf/TrfGeom.h"
#include "domain/constraints/SFreedom_Constraint.h"
#include "domain/constraints/MFreedom_Constraint.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include "solution/graph/graph/Graph.h"
#include "solution/graph/graph/Vertex.h"
#include "utility/matrix/ID.h"

#include "xc_utils/src/geom/pos_vec/SVD3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d3/SemiEspacio3d.h"



//! @brief Constructor.
XC::SetMeshComp::SetMeshComp(const std::string &nmb,Preprocessor *md)
  : SetBase(nmb,md), nodos(this), elementos(this), constraints(this) {}

//! @brief Copy constructor.
XC::SetMeshComp::SetMeshComp(const SetMeshComp &otro)
  : SetBase(otro)
  { copia_listas(otro); }

//! @brief Operator asignación.
XC::SetMeshComp &XC::SetMeshComp::operator=(const SetMeshComp &otro)
  {
    SetBase::operator=(otro);
    copia_listas(otro);
    return *this;
  }

//! @brief Copia las listas de objetos del conjunto s.
//!
//! Copia en ESTE objeto las listas de objetos del conjunto
//! being passed as parameter.
void XC::SetMeshComp::copia_listas(const SetMeshComp &otro)
  {
    nodos= otro.nodos;
    nodos.set_owner(this);
    elementos= otro.elementos;
    elementos.set_owner(this);
    constraints= otro.constraints;
    constraints.set_owner(this);
  }

//! @brief Agrega a este conjunto los objetos del conjunto
//! being passed as parameter.
void XC::SetMeshComp::agrega_listas(const SetMeshComp &otro)
  {
    nodos.agrega(otro.nodos);
    elementos.agrega(otro.elementos);
    constraints.agrega(otro.constraints);
  }

// //! @brief Agrega a este conjunto los objetos del conjunto
// //! being passed as parameter que cumplen la condición.
// void XC::SetMeshComp::agrega_listas_cond(const SetMeshComp &otro,const std::string &cond)
//   {
//     nodos.agrega_cond(otro.nodos,cond);
//     elementos.agrega_cond(otro.elementos,cond);
//     constraints.agrega_cond(otro.constraints,cond);
//   }

//!  \brief Borra todos los objetos de las listas.
void XC::SetMeshComp::clear(void)
  {
    //SetBase::clear();
    nodos.clear();
    constraints.clear();
    elementos.clear();
  }

//!  \brief Borra todos los objetos del conjunto.
void XC::SetMeshComp::clearAll(void)
  {
    SetBase::clearPyProps();
    nodos.clearAll();
    constraints.clearAll();
    elementos.clearAll();
  }

//! @brief Agrega the pointer a nodo being passed as parameter.
void XC::SetMeshComp::agregaNodo(Node *nPtr)
  { nodos.push_back(nPtr); }

//! @brief Agrega el elemento being passed as parameter.
void XC::SetMeshComp::agregaElemento(Element *ePtr)
  { elementos.push_back(ePtr); }

//! @brief Returns true if the node belongs to the set.
bool XC::SetMeshComp::In(const Node *n) const
  { return nodos.in(n); }

//! @brief Returns true if the element belongs to the set.
bool XC::SetMeshComp::In(const Element *e) const
  { return elementos.in(e); }

//! @brief Asigna índices a los objetos del conjunto (nodos,elementos,puntos...) poder emplearlos en VTK.
void XC::SetMeshComp::numera(void)
  {
    nodos.numera();
    elementos.numera();
    numera_lista(constraints);
  }

//! @brief Desplaza los nodos del conjunto.
void XC::SetMeshComp::mueve(const Vector3d &desplaz)
  { nodos.mueve(desplaz); }

//! @brief Aplica la transformación a los elementos del conjunto.
void XC::SetMeshComp::Transforma(const TrfGeom &trf)
  { nodos.transforma(trf); }

//! @brief Applies to the set the transformation with the identifier being passed as parameter.
void XC::SetMeshComp::Transforma(const size_t &indice_trf)
  {
    TrfGeom *trf= get_preprocessor()->getCad().getTransformacionesGeometricas().busca(indice_trf);
    if(trf)
      Transforma(*trf);
  }

//! @brief Impone desplazamiento nulo en los nodos de este conjunto.
void XC::SetMeshComp::fix(const SFreedom_Constraint &spc)
  {
    ConstraintLoader &cl= GetPreprocessor()->getConstraintLoader();
    for(nod_const_iterator i= nodos_begin();i!=nodos_end();i++)
      {
        const int tag_nod= (*i)->getTag();
        cl.addSFreedom_Constraint(tag_nod,spc);
      }
  }

// //! @brief Returns an iterator which points to principio de la lista de nodos.
// XC::SetMeshComp::nod_iterator XC::SetMeshComp::nodos_begin(void)
//   { return nodos.begin(); }
//! @brief Returns an iterator which points to principio de la lista de nodos.
XC::SetMeshComp::nod_const_iterator XC::SetMeshComp::nodos_begin(void) const
  { return nodos.begin(); }
// //! @brief Returns an iterator apuntando tras el final de la lista de nodos.
// XC::SetMeshComp::nod_iterator XC::SetMeshComp::nodos_end(void)
//   { return nodos.end(); }
//! @brief Returns an iterator apuntando tras el final de la lista de nodos.
XC::SetMeshComp::nod_const_iterator XC::SetMeshComp::nodos_end(void) const
  { return nodos.end(); }

//! @brief Returns (if it exists) a pointer to the node
//! cuyo tag se pasa como parámetro.
XC::Node *XC::SetMeshComp::buscaNodo(const int &tag)
  { return nodos.buscaNodo(tag); }

//! @brief Returns (if it exists) a pointer to the node
//! cuyo tag se pasa como parámetro.
const XC::Node *XC::SetMeshComp::buscaNodo(const int &tag) const
  { return nodos.buscaNodo(tag); }

// //! @brief Returns an iterator which points to principio de la lista de elementos.
// XC::SetMeshComp::elem_iterator XC::SetMeshComp::elem_begin(void)
//   { return elementos.begin(); }
//! @brief Returns an iterator which points to principio de la lista de elementos.
XC::SetMeshComp::elem_const_iterator XC::SetMeshComp::elem_begin(void) const
  { return elementos.begin(); }
// //! @brief Returns an iterator apuntando tras el final de la lista de elementos.
// XC::SetMeshComp::elem_iterator XC::SetMeshComp::elem_end(void)
//   { return elementos.end(); }
//! @brief Returns an iterator apuntando tras el final de la lista de elementos.
XC::SetMeshComp::elem_const_iterator XC::SetMeshComp::elem_end(void) const
  { return elementos.end(); }

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
//! cuyo tag se pasa como parámetro.
XC::Element *XC::SetMeshComp::buscaElemento(const int &tag)
  { return elementos.buscaElemento(tag); }

//! @brief Returns (if it exists) a pointer to the node
//! cuyo tag se pasa como parámetro.
const XC::Element *XC::SetMeshComp::buscaElemento(const int &tag) const
  { return elementos.buscaElemento(tag); }

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

//! @brief Returns the número de elementos del conjunto que están activos.
size_t XC::SetMeshComp::getNumLiveElements(void) const
  { return elementos.getNumLiveElements(); }

//! @brief Returns the número de elementos del conjunto que están inactivos.
size_t XC::SetMeshComp::getNumDeadElements(void) const
  { return elementos.getNumDeadElements(); }

//! @brief Returns the número de nods del conjunto que están activos.
size_t XC::SetMeshComp::getNumLiveNodes(void) const
  { return nodos.getNumLiveNodes(); }


//! @brief Returns the número de nodos del conjunto que están inactivos.
size_t XC::SetMeshComp::getNumDeadNodes(void) const
  { return nodos.getNumDeadNodes(); }

//! @brief Desactiva los elementos del conjunto.
void XC::SetMeshComp::kill_elements(void)
  { elementos.kill_elements(); }

//! @brief Reactiva los elementos del conjunto.
void XC::SetMeshComp::alive_elements(void)
  { elementos.alive_elements(); }

//! @brief Calcula los esfuerzos en cada uno de los elementos.
void XC::SetMeshComp::calc_resisting_force(void)
  { elementos.calc_resisting_force(); }

//! @brief Return the resultante de las fuerzas que ejercen sobre
//! los nodos próximos al plano, los elementos situados tras el plano.
SVD3d XC::SetMeshComp::getResistingSVD3d(const Plano3d &plano,const Pos3d &centro,const double &tol,const bool &inc_inertia) const
  {
    //XX Se puede mejorar calculando la resultante en la intersección de
    //los lados del elemento con el plano e interpolando el valor en ese
    //punto a partir de los valores en los nodos extremos del lado.
    const double tol2= tol*tol;
    const Node *ptrNod= nullptr;
    const SemiEspacio3d se(plano);
    std::set<const Node *> nodos_plano;
    std::set<const Element *> connected_to_node;
    std::set<const Element *> connected_elements;
    for(DqPtrsNode::const_iterator i= nodos.begin();i!=nodos.end();i++)
      {
        ptrNod= *i;
        if(ptrNod)
          if(ptrNod->isAlive())
            {
              if(plano.dist2(ptrNod->getPosInicial3d())<tol2)
                {
                  connected_to_node= ptrNod->getConnectedElements();
                  if(!connected_to_node.empty())
                    {
                      nodos_plano.insert(ptrNod); //Nodo sobre el plano.
                      for(std::set<const Element *>::const_iterator j= connected_to_node.begin();
                          j!=connected_to_node.end();j++)
                        {
                          if((*j)->getNodePtrs().In(se)) //Elemento tras el plano.
                            connected_elements.insert(*j);
                          else if((*j)->getNodePtrs().Corta(plano)) //Elemento sobre el plano.
                            connected_elements.insert(*j);
                        }
                    }
	        }
            }
      }
    SVD3d retval(centro);
    for(std::set<const Node *>::const_iterator i= nodos_plano.begin();i!=nodos_plano.end();i++)
      retval+= (*i)->getResistingSVD3d(connected_elements,inc_inertia);
    return retval;    
  }

//! @brief Returns true if the node with the tag
//! being passed as parameter, belongs to the set.
bool XC::SetMeshComp::InNodeTag(const int tag_nodo) const
  { return nodos.InNodeTag(tag_nodo); }

//! @brief Returns true if the nodes, with the tags
//! being passed as parameter, belongs to the set.
bool XC::SetMeshComp::InNodeTags(const ID &tag_nodos) const
  { return nodos.InNodeTags(tag_nodos); }

//! @brief Returns node tags.
std::set<int> XC::SetMeshComp::getNodeTags(void) const
  { return nodos.getTags(); }

//! @brief Returns the nodo más próximo al punto being passed as parameter.
XC::Node *XC::SetMeshComp::getNearestNode(const Pos3d &p)
  { return nodos.getNearestNode(p); }

//! @brief Returns the nodo más próximo al punto being passed as parameter.
const XC::Node *XC::SetMeshComp::getNearestNode(const Pos3d &p) const
  {
    SetMeshComp *this_no_const= const_cast<SetMeshComp *>(this);
    return this_no_const->getNearestNode(p);
  }


//! @brief Returns the tags de los elementos.
std::set<int> XC::SetMeshComp::getElementTags(void) const
  { return elementos.getTags(); }

//! @brief Returns the elemento más próximo al punto being passed as parameter.
XC::Element *XC::SetMeshComp::getNearestElement(const Pos3d &p)
  { return elementos.getNearestElement(p); }

//! @brief Returns the elemento más próximo al punto being passed as parameter.
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


//! @brief Agrega al conjunto being passed as parameter
//! los elementos que intervienen en la
//! definición de los que ya están en el conjunto.
void XC::SetMeshComp::CompletaHaciaAbajo(void)
  {
    const DqPtrsElem &elems= elementos;
    for(elem_const_iterator i= elems.begin();i!=elems.end();i++)
      {
        Element *elem= (*i);
        assert(elem);
        NodePtrs &theNodes= elem->getNodePtrs();
        for(NodePtrs::iterator j= theNodes.begin();j!=theNodes.end();j++)
          nodos.push_back(*j);
      }
  }

//! @brief Appends to this set the objects that make reference to one
//! or more of the objects that already are in the set.
void XC::SetMeshComp::CompletaHaciaArriba(void)
  {
    std::cerr << "SetMeshComp::CompletaHaciaArriba() implementation pending." << std::endl;
  }

//! @brief Selecciona los nodos cuyos tags se pasan como parámetro.
void XC::SetMeshComp::sel_nodos_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= get_preprocessor();
        if(preprocessor)
          for(size_t i= 0;i<sz;i++)
            nodos.push_back(preprocessor->getDomain()->getNode(tags(i)));
        else
          std::cerr << "SetMeshComp::sel_nodos_lista; necesito un preprocesador." << std::endl;
      }
  }

//! @brief Selecciona los elementos cuyos tags se pasan como parámetro.
void XC::SetMeshComp::sel_elementos_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= get_preprocessor();
        if(preprocessor)
          for(size_t i= 0;i<sz;i++)
            elementos.push_back(preprocessor->getDomain()->getElement(tags(i)));
        else
          std::cerr << "SetMeshComp::sel_elementos_lista; necesito un preprocesador." << std::endl;
      }
  }

//! @brief Selecciona las coacciones cuyos tags se pasan como parámetro.
void XC::SetMeshComp::sel_constraints_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= get_preprocessor();
        Constraint *tmp= nullptr;
        if(preprocessor)
          for(size_t i= 0;i<sz;i++)
            {
              if((tmp= preprocessor->getDomain()->getConstraints().getSFreedom_Constraint(tags(i))))
                constraints.push_back(tmp);
              else if((tmp=preprocessor->getDomain()->getConstraints().getMFreedom_Constraint(tags(i))))
                constraints.push_back(tmp);
              else
		std::cerr << "SetMeshComp::sel_constraints_lista;no se encontró la coacción de tag: "
                          << tags(i) << std::endl;
            }

        else
          std::cerr << "SetMeshComp::sel_constraints_lista; necesito un preprocesador." << std::endl;
      }
  }

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::SetMeshComp::getDbTagData(void) const
  {
    static DbTagData retval(19);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::SetMeshComp::sendData(CommParameters &cp)
  {
    int res= SetBase::sendData(cp);
    res+= nodos.sendTags(3,4,getDbTagData(),cp);
    res+= elementos.sendTags(5,6,getDbTagData(),cp);
    res+= constraints.sendTags(7,8,getDbTagData(),cp);
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::SetMeshComp::recvData(const CommParameters &cp)
  {
    ID tmp;
    int res= SetBase::recvData(cp);
    tmp= nodos.receiveTags(3,4,getDbTagData(),cp);
    sel_nodos_lista(tmp);
    tmp= elementos.receiveTags(5,6,getDbTagData(),cp);
    sel_elementos_lista(tmp);
    tmp= constraints.receiveTags(7,8,getDbTagData(),cp);
    sel_constraints_lista(tmp);
    return res;
  }

//!  \brief Destructor.
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
      std::cerr << nombre_clase() << "::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::SetMeshComp::recvSelf(const CommParameters &cp)
  {
    inicComm(19);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
