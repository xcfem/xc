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
//SetEstruct.cc

#include "SetEstruct.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/utils/ElementEdges.h"





//! @brief Constructor.
XC::SetEstruct::SetEstruct(const std::string &nmb,Preprocessor *md)
  : SetBase(nmb,md) {}

//! @brief Copy constructor.
XC::SetEstruct::SetEstruct(const SetEstruct &otro)
  : SetBase(otro) {}

//! @brief Operator asignación.
XC::SetEstruct &XC::SetEstruct::operator=(const SetEstruct &otro)
  {
    SetBase::operator=(otro);
    return *this;
  }

size_t XC::SetEstruct::NumNodos(void) const
  { return GetNumCapasNodos()*GetNumFilasNodos()*GetNumColsNodos(); }
size_t XC::SetEstruct::NumElementos(void) const
  { return GetNumCapasElementos()*GetNumFilasElementos()*GetNumColsElementos(); }

//! @brief Returns true if the único índice que varía es el I.
bool XC::SetEstruct::EsFilaI(void) const
  {
    if((GetNumFilasNodos()>1) || (GetNumColsNodos()>1)) return false;
    return true;
  }

//! @brief Returns true if the único índice que varía es el J.
bool XC::SetEstruct::EsFilaJ(void) const
  {
    if((GetNumCapasNodos()>1) || (GetNumColsNodos()>1)) return false;
    return true;
  }

//! @brief Returns true if the único índice que varía es el K.
bool XC::SetEstruct::EsFilaK(void) const
  {
    if((GetNumCapasNodos()>1) || (GetNumFilasNodos()>1)) return false;
    return true;
  }

//! @brief Returns true iflos indices que varían son los J y K.
bool XC::SetEstruct::EsCapaICte(void) const
  { return (GetNumCapasNodos()==1); }

//! @brief Returns true iflos indices que varían son los I y K.
bool XC::SetEstruct::EsCapaJCte(void) const
  { return (GetNumFilasNodos()==1); }

//! @brief Returns true iflos indices que varían son los I y J.
bool XC::SetEstruct::EsCapaKCte(void) const
  { return (GetNumColsNodos()==1); }

//! @brief Returns the set type filaI, filaJ, filaK, capaICte,...
std::string XC::SetEstruct::GetStrTipo(void) const
  {
    if(EsFilaI()) return "filaI";
    if(EsFilaJ()) return "filaJ";
    if(EsFilaK()) return "filaK";
    if(EsCapaICte()) return "capaICte";
    if(EsCapaJCte()) return "capaJCte";
    if(EsCapaKCte()) return "capaKCte";
    return "bloqueIJK";
  }

//! @brief Return the dimensión del rango (0,1,2 ó 3).
size_t XC::SetEstruct::Dimension(void) const
  {
    size_t retval= 0;
    if(GetNumCapasNodos()>1) retval++;
    if(GetNumFilasNodos()>1) retval++;
    if(GetNumColsNodos()> 1) retval++;
    return retval;
  }

//! @brief Returns the pointers to the set nodes.
XC::NodePtrSet XC::SetEstruct::getNodePtrSet(void)
  {
    NodePtrSet retval;
    const size_t ncapas= GetNumCapasNodos();
    const size_t nfilas= GetNumFilasNodos();
    const size_t ncols= GetNumColsNodos();
    for(size_t i= 1;i<=ncapas;i++)
      for(size_t j= 1;j<=nfilas;j++)
        for(size_t k= 1;k<=ncols;k++)
          retval.insert(GetNodo(i,j,k));
    return retval;    
  }

//! @brief Returns the element edges with both end nodes
//! belong to the nodes of the set.
XC::ElementEdges XC::SetEstruct::getElementEdges(void)
  {
    const NodePtrSet nodos= getNodePtrSet();
    return getElementEdgesEntreNodos(nodos);
  }

//! @brief Agrega al modelo los elementos being passed as parameters.
void XC::SetEstruct::agrega_elementos(const TritrizPtrElem &elementos)
  {
    const size_t capas= elementos.GetCapas();
    if(capas<1) return;
    const size_t filas= elementos(1).getNumFilas();
    const size_t cols= elementos(1).getNumCols();
    for(register size_t i= 1;i<=capas;i++)
      for(register size_t j= 1;j<=filas;j++)
        for(register size_t k= 1;k<=cols;k++)
          get_preprocessor()->getElementLoader().Agrega(elementos(i,j,k));
  }

//! @brief Returns the tags de los nodos.
std::set<int> XC::SetEstruct::getNodeTags(void) const
  {
    std::set<int> retval;
    const size_t ncapas= GetNumCapasNodos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasNodos();
        const size_t ncols= GetNumColsNodos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              retval.insert(GetNodo(i,j,k)->getTag());
      }
    return retval;
  }

boost::python::list XC::SetEstruct::getNodes(void)
  {
    boost::python::list retval;
    const size_t ncapas= GetNumCapasNodos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasNodos();
        const size_t ncols= GetNumColsNodos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              {
                Node *tmp= GetNodo(i,j,k);
                boost::python::object pyObj(boost::ref(*tmp));
                retval.append(pyObj);
              }
      }
    return retval;
  }


//! @brief Returns the tags de los elementos.
std::set<int> XC::SetEstruct::getElementTags(void) const
  {
    std::set<int> retval;
    const size_t ncapas= GetNumCapasElementos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasElementos();
        const size_t ncols= GetNumColsElementos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              retval.insert(GetElemento(i,j,k)->getTag());
      }
    return retval;
  }

boost::python::list XC::SetEstruct::getElements(void)
  {
    boost::python::list retval;
    const size_t ncapas= GetNumCapasElementos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasElementos();
        const size_t ncols= GetNumColsElementos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              {
                Element *tmp= GetElemento(i,j,k);
                boost::python::object pyObj(boost::ref(*tmp));
                retval.append(pyObj);
              }
      }
    return retval;
  }

//! @brief Returns true if the nodo belongs to the set.
bool XC::SetEstruct::In(const Node *n) const
  {
    bool retval= false;
    const size_t ncapas= GetNumCapasNodos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasNodos();
        const size_t ncols= GetNumColsNodos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              if(GetNodo(i,j,k)==n)
                {
                  retval= true;
                  break;
                }
      }
    return retval;
  }

//! @brief Returns true if the element belongs to the set.
bool XC::SetEstruct::In(const Element *e) const
  {
    bool retval= false;
    const size_t ncapas= GetNumCapasElementos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasElementos();
        const size_t ncols= GetNumColsElementos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              if(GetElemento(i,j,k)==e)
                {
                  retval= true;
                  break;
                }
      }
    return retval;
  }


XC::Node *XC::SetEstruct::getNodeI(const size_t &i)
  {
    if(EsFilaI())
      return GetNodo(i,1,1);
    else if(EsFilaJ())
      return GetNodo(1,i,1);
    else if(EsFilaK())
      return GetNodo(1,1,i);
    else
      {
	std::cerr << "SetEstruct::getNodoI; node set is not one-dimensional." << std::endl;
        return nullptr;
      }
  }

XC::Node *XC::SetEstruct::getNodeIJ(const size_t &i,const size_t &j)
  {
    //XXX Falta obtener la capa (no ncesariemente será la 1),
    if(EsCapaICte())
      return GetNodo(1,i,j);
    else if(EsCapaJCte()) 
      return GetNodo(i,1,j);
    else if(EsCapaKCte()) 
      return GetNodo(i,j,1);
    else
      {
	std::cerr << "SetEstruct::getNodoI; el node set no es bidimensional." << std::endl;
        return nullptr;
      } 
  }
