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
//ElementEdges.cpp

#include "ElementEdges.h"
#include "domain/mesh/element/Element.h"
#include "domain/domain/Domain.h"


#include "xc_basic/src/matrices/m_int.h"
#include "utility/matrix/ID.h"


//! @brief Constructor.
XC::ElementEdges::ElementEdges(void)
  : EntCmd() {}

//! @brief Asigna los punteros a partir de los identificadores de elemento.
void XC::ElementEdges::setPtrs(Domain *theDomain, const ID &theElementTags, const ID &theEdgesIndex)
  {
    size_t sz= theElementTags.Size();
    clear();
    if(theDomain)
      {
        resize(sz);
        for(size_t i=0; i<sz; i++)
          {
            Element *tmp= theDomain->getElement(theElementTags(i));
            
            if(tmp)
              (*this)[i]= ElementEdge(tmp,theEdgesIndex(i));
            else
              {
                std::cerr << "WARNING - XC::ElementEdges::setDomain - ele with tag "
	                  << theElementTags(i) << " does not exist in the domain\n";
              }
          }
      }
  }

//! @brief Devuelve un iterador al elemento cuyo tag se pasa como parámetro.
XC::ElementEdges::iterator XC::ElementEdges::find(const int &tag)
  {
    iterator retval= end();
    for(iterator i= begin();i!=end();i++)
      if((*i).getElementPtr()->getTag() == tag)
        { retval= i; break; }
    return retval;
  }

//! @brief Devuelve un iterador al elemento cuyo tag se pasa como parámetro.
XC::ElementEdges::const_iterator XC::ElementEdges::find(const int &tag) const
  {
    const_iterator retval= end();
    for(const_iterator i= begin();i!=end();i++)
      if((*i).getElementPtr()->getTag() == tag)
        { retval= i; break; }
    return retval;
  }


//! @brief Devuelve el índice el elemento en el vector,
//! si no lo encuentra devuelve -1.
//XXX: Funciona mal si el elemento aparece más de una vez en el vector.
int XC::ElementEdges::getLocElement(const Element *ptr) const
  {
    int retval= -1;
    const size_t sz= size();
    for(size_t i= 0;i<sz;i++)
      if((*this)[i].getElementPtr() == ptr)
        { retval= i; break; }
    return retval;
  }

//! @brief Elimina el elemento cuyo tag se pasa como parámetro.
size_t XC::ElementEdges::removeElement(const int &tag) 
  {
    iterator i= find(tag);
    if(i!=end())
      erase(i,i);
    return size();
  }



XC::ElementEdges::const_reference XC::ElementEdges::operator()(const size_t &i) const
  { return (*this)[i]; }
XC::ElementEdges::reference XC::ElementEdges::operator()(const size_t &i)
  { return (*this)[i]; }
