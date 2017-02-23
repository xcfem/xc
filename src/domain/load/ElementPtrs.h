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
//ElementPtrs.h
                                                                        
#ifndef ElementPtrs_h
#define ElementPtrs_h

#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
class Element;
class Domain;
class ID;
 

//! @ingroup ElemLoads
//
//! @brief Pointers to the elements affected by the load.
class ElementPtrs: public EntCmd
  {
  public:
    typedef std::vector<Element *> vector_ptr_elements;
    typedef vector_ptr_elements::const_reference const_reference;
    typedef vector_ptr_elements::reference reference;
    typedef vector_ptr_elements::iterator iterator;
    typedef vector_ptr_elements::const_iterator const_iterator;

  private:
    vector_ptr_elements theElements; //!< element set.
  protected:

  public:
    ElementPtrs(void);
    ~ElementPtrs(void);

    virtual void setPtrs(Domain *theDomain, const ID &theElementTags);

    virtual size_t removeElement(const int &tag);

    inline size_t size(void) const
      { return theElements.size(); }
    inline const_reference front() const
      { return theElements.front(); }
    inline reference front()
      { return theElements.front(); }
    inline const_reference back() const
      { return theElements.back(); }
    inline reference back()
      { return theElements.back(); }
    inline const_iterator begin() const
      { return theElements.begin(); }
    inline iterator begin()
      { return theElements.begin(); }
    inline const_iterator end() const
      { return theElements.end(); }
    inline iterator end()
      { return theElements.end(); }
    iterator find(const int &tag);
    const_iterator find(const int &tag) const;
    Element *findPtr(const int &tag);
    const Element *findPtr(const int &tag) const;

    const_reference operator()(const size_t &i) const;
    reference operator()(const size_t &i);
    const_reference operator[](const size_t &i) const;
    reference operator[](const size_t &i);


  };
} // end of XC namespace

#endif

