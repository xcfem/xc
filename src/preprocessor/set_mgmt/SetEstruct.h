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
//SetEstruct.h

#ifndef SETESTRUCT_H
#define SETESTRUCT_H

#include "SetBase.h"
#include <map>
#include "domain/mesh/node/NodeTopology.h"

namespace XC {
class TritrizPtrElem;
class Node;
class Element;

//!  \ingroup Set
//! 
//!  \brief Conjunto estructurado, es decir que puede devolver
//!  un puntero a un nodo o a un elemento a partir de sus índices i,j y k.
class SetEstruct: public SetBase
  {
  protected:
    friend class Preprocessor;
    void agrega_elementos(const TritrizPtrElem &);
    virtual const Node *get_node_ptr_from_stack(const std::string &cod) const;
    virtual const Element *get_element_ptr_from_stack(const std::string &cod) const;

    NodePtrSet getNodePtrSet(void);
  public:
    SetEstruct(const std::string &nmb="",Preprocessor *preprocessor= NULL);
    SetEstruct(const SetEstruct &otro);
    SetEstruct &operator=(const SetEstruct &otro);
    virtual SetEstruct *getCopy(void) const= 0;
    virtual size_t GetNumCapasNodos(void) const= 0;
    virtual size_t GetNumFilasNodos(void) const= 0;
    virtual size_t GetNumColsNodos(void) const= 0;
    virtual size_t GetNumCapasElementos(void) const= 0;
    virtual size_t GetNumFilasElementos(void) const= 0;
    virtual size_t GetNumColsElementos(void) const= 0;
    size_t NumNodos(void) const;
    size_t NumElementos(void) const;
    bool In(const Node *) const;
    bool In(const Element *) const;

    bool EsFilaI(void) const;
    bool EsFilaJ(void) const;
    bool EsFilaK(void) const;
    bool EsCapaICte(void) const;
    bool EsCapaJCte(void) const;
    bool EsCapaKCte(void) const;
    size_t Dimension(void) const;
    std::string GetStrTipo(void) const;

    virtual Node *GetNodo(const size_t &i=1,const size_t &j=1,const size_t &k=1)= 0;
    virtual const Node *GetNodo(const size_t &i=1,const size_t &j=1,const size_t &k=1) const= 0;

    Node *getNodeI(const size_t &i);
    Node *getNodeIJ(const size_t &i,const size_t &j);
    inline Node *getNodeIJK(const size_t &i,const size_t &j,const size_t &k)
      { return GetNodo(i,j,k); }

    virtual Element *GetElemento(const size_t &i=1,const size_t &j=1,const size_t &k=1)= 0;
    virtual const Element *GetElemento(const size_t &i=1,const size_t &j=1,const size_t &k=1) const= 0;

    inline Element *getElementI(const size_t &i)
      { return GetElemento(i,1,1); }
    inline Element *getElementIJ(const size_t &i,const size_t &j)
      { return GetElemento(i,j,1); }
    inline Element *getElementIJK(const size_t &i,const size_t &j,const size_t &k)
      { return GetElemento(i,j,k); }

    std::set<int> getNodeTags(void) const;
    boost::python::list getNodes(void);
    std::set<int> getElementTags(void) const;
    boost::python::list getElements(void);
    ElementEdges getElementEdges(void);
  };
} //end of XC namespace
#endif
