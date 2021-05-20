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
//ElementPtrs.cpp

#include "ElementPtrs.h"
#include "domain/mesh/element/Element.h"
#include "domain/domain/Domain.h"


#include "utility/matrices/m_int.h"
#include "utility/matrix/ID.h"


//! @brief Constructor.
XC::ElementPtrs::ElementPtrs(void)
  : theElements(0) {}

//! @brief Destructor.
XC::ElementPtrs::~ElementPtrs(void)
  { theElements.clear(); }

//! @brief Set the element pointers from the element identifiers being passed
//! as parameter.
void XC::ElementPtrs::setPtrs(Domain *theDomain, const ID &theElementTags)
  {
    const size_t sz= theElementTags.Size();
    if(sz==0)
      std::clog << getClassName() << "::" << __FUNCTION__
                << "; no element identifiers." << std::endl;
    theElements.clear();
    if(theDomain)
      {
        theElements.resize(sz);
        for(size_t i=0; i<sz; i++)
          {
	    const int eleTag= theElementTags(i);
            theElements[i]= theDomain->getElement(eleTag);
            if(!theElements[i])
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; WARNING - ele with tag: "
			  << eleTag << " does not exist in the domain\n";
              }
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; domain not set."
	        << std::endl;
  }

//! @brief Returns an iterator to the element identified by the
//! tag being passed as parameter.
XC::ElementPtrs::iterator XC::ElementPtrs::find(const int &tag)
  {
    iterator retval= end();
    for(iterator i= begin();i!=end();i++)
      if((*i)->getTag() == tag)
        retval= i;
    return retval;
  }

//! @brief Returns an iterator to the element identified by the
//! tag being passed as parameter.
XC::ElementPtrs::const_iterator XC::ElementPtrs::find(const int &tag) const
  {
    const_iterator retval= end();
    for(const_iterator i= begin();i!=end();i++)
      if((*i)->getTag() == tag)
        retval= i;
    return retval;
  }

XC::Element *XC::ElementPtrs::findPtr(const int &tag)
  {
    XC::Element *retval= nullptr;
    iterator i= find(tag);
    if(i!=end())
      retval= *i;
    return retval;
  }

const XC::Element *XC::ElementPtrs::findPtr(const int &tag) const
  {
    const XC::Element *retval= nullptr;
    const_iterator i= find(tag);
    if(i!=end())
      retval= *i;
    return retval;
  }


//! @brief Erases the element identified by the tag being passed as parameter.
size_t XC::ElementPtrs::removeElement(const int &tag) 
  {
    iterator i= find(tag);
    if(i!=end())
      theElements.erase(i,i);
    return size();
  }



XC::ElementPtrs::const_reference XC::ElementPtrs::operator()(const size_t &i) const
  { return theElements[i]; }
XC::ElementPtrs::reference XC::ElementPtrs::operator()(const size_t &i)
  { return theElements[i]; }
XC::ElementPtrs::const_reference XC::ElementPtrs::operator[](const size_t &i) const
  { return theElements[i]; }
XC::ElementPtrs::reference XC::ElementPtrs::operator[](const size_t &i)
  { return theElements[i]; }
