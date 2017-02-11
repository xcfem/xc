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
//SetBase.cc

#include "SetBase.h"

#include <boost/any.hpp>
#include "domain/constraints/SFreedom_Constraint.h"


#include "utility/matrix/ID.h"
#include "preprocessor/Preprocessor.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

//! @brief Constructor.
XC::SetBase::SetBase(const std::string &nmb,Preprocessor *md)
  : EntMdlrBase(nmb,md) {}

//! @brief Generates a finite element mesh from the set components.
void XC::SetBase::genMesh(meshing_dir dm)
  {
    std::cerr << "Método genMesh() no implementado." << std::endl;
  }

//! @brief Impone desplazamiento nulo en los nodos de este conjunto.
void XC::SetBase::fix(const SFreedom_Constraint &)
  {
    std::cerr << "Método fix() no implementado." << std::endl;
  }

//! @brief Devuelve los tags de los nodos en un vector de enteros.
const XC::ID &XC::SetBase::getIdNodeTags(void) const
  {
    static ID retval;
    const std::set<int> tmp= getNodeTags();
    if(!tmp.empty())
      {
        const size_t sz= tmp.size();
	retval.resize(sz);
        size_t conta= 0;
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++,conta++)
          retval[conta]= *i;
      }
    return retval;
  }

//! @brief Devuelve los tags de los elementos en un vector de enteros.
const XC::ID &XC::SetBase::getIdElementTags(void) const
  {
    static ID retval;
    const std::set<int> tmp= getElementTags();
    if(!tmp.empty())
      {
        const size_t sz= tmp.size();
	retval.resize(sz);
        size_t conta= 0;
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++,conta++)
          retval[conta]= *i;
      }
    return retval;
  }

//! @brief Devuelve verdadero si el punto pertenece al conjunto.
bool XC::SetBase::In(const Pnt *) const
  { return false; }

//! @brief Devuelve verdadero si el «edge» pertenece al conjunto.
bool XC::SetBase::In(const Edge *) const
  { return false; }

//! @brief Devuelve verdadero si la superficie pertenece al conjunto.
bool XC::SetBase::In(const Face *) const
  { return false; }

//! @brief Devuelve verdadero si el cuerpo pertenece al conjunto.
bool XC::SetBase::In(const Body *) const
  { return false; }

//! @brief Devuelve verdadero si la «uniform grid» pertenece al conjunto.
bool XC::SetBase::In(const UniformGrid *) const
  { return false; }

//! @brief Reset tributary areas (or lengths, or volumes) for the nodes that
//! are connected to the set elements.
void XC::SetBase::resetTributarias(void) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetPreprocessor()->getDomain();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->resetTributarias();
            else
	      std::cerr << "SetBase::resetTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el domain." << std::endl;    
  }
 
//! @brief Calcula las longitudes tributarias correspondientes a cada
//! nodo de los elementos del conjunto.
void XC::SetBase::calculaLongsTributarias(bool initialGeometry) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetPreprocessor()->getDomain();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->calculaLongsTributarias(initialGeometry);
            else
	      std::cerr << "SetBase::calculaLongsTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el domain." << std::endl;
  }

//! @brief Calcula las áreas tributarias correspondientes a cada
//! nodo de los elementos del conjunto.
void XC::SetBase::calculaAreasTributarias(bool initialGeometry) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetPreprocessor()->getDomain();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->calculaAreasTributarias(initialGeometry);
            else
	      std::cerr << "SetBase::calculaLongsTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el domain." << std::endl;
  }

//! @brief Calcula los volúmenes tributarios correspondientes a cada
//! nodo de los elementos del conjunto.
void XC::SetBase::calculaVolsTributarios(bool initialGeometry) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetPreprocessor()->getDomain();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->calculaVolsTributarios(initialGeometry);
            else
	      std::cerr << "SetBase::calculaLongsTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el domain." << std::endl;
  }


