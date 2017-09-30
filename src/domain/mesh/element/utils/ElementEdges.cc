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

//! @brief Set the pointers from the element identifiers.
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
                std::cerr << getClassName() << "::" << __FUNCTION__
		          << "; Warning! ele with tag "
	                  << theElementTags(i) << " does not exist in the domain\n";
              }
          }
      }
  }

//! @brief Returns an iterator to the element identified by the
//! tag being passed as parameter.
XC::ElementEdges::iterator XC::ElementEdges::find(const int &tag)
  {
    iterator retval= end();
    for(iterator i= begin();i!=end();i++)
      if((*i).getElementPtr()->getTag() == tag)
        { retval= i; break; }
    return retval;
  }

//! @brief Returns an iterator to the element identified by the
//! tag being passed as parameter.
XC::ElementEdges::const_iterator XC::ElementEdges::find(const int &tag) const
  {
    const_iterator retval= end();
    for(const_iterator i= begin();i!=end();i++)
      if((*i).getElementPtr()->getTag() == tag)
        { retval= i; break; }
    return retval;
  }


//! @brief Returns the element index on the vector,
//! returns -1 if not found.
//XXX: It doesn't works if the elements appears more than once in the vector.
int XC::ElementEdges::getLocElement(const Element *ptr) const
  {
    int retval= -1;
    const size_t sz= size();
    for(size_t i= 0;i<sz;i++)
      if((*this)[i].getElementPtr() == ptr)
        { retval= i; break; }
    return retval;
  }

//! @brief Removes the element identified by the tag being passed as parameter.
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
