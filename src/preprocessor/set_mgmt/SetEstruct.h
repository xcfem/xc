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
