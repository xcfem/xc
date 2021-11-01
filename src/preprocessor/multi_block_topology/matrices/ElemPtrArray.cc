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
//ElemPtrArray.cc

#include "ElemPtrArray.h"
#include "domain/mesh/element/Element.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "boost/lexical_cast.hpp"

//! @brief Returns (if it exists) a pointer to the element
//! identified by the tag being passed as parameter.
XC::Element *XC::ElemPtrArray::findElement(const int &tag)
  {
    Element *retval= nullptr;
    Element *tmp= nullptr;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          tmp= operator()(j,k);
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

//! @brief Returns (if it exists) a pointer to the element
//! identified by the tag being passed as parameter.
const XC::Element *XC::ElemPtrArray::findElement(const int &tag) const
  {
    const Element *retval= nullptr;
    const Element *tmp= nullptr;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          tmp= operator()(j,k);
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

//! @brief Returns the node closest to the point being passed as parameter.
XC::Element *XC::ElemPtrArray::getNearestElement(const Pos3d &p)
  {
    Element *retval= nullptr, *ptrElem= nullptr;
    double d= DBL_MAX;
    double tmp;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    if(numberOfRows*numberOfColumns>1000)
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; the element matrix has "
                << numberOfRows*numberOfColumns << " components. "
                << " It is better to search by coordinates in the associated set."
                << std::endl;
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          ptrElem= operator()(j,k);
          tmp= ptrElem->getDist2(p);
          if(tmp<d)
            {
              d= tmp;
              retval= ptrElem;
            }
        }
    return retval;
  }

//! @brief Returns the node closest to the point being passed as parameter.
const XC::Element *XC::ElemPtrArray::getNearestElement(const Pos3d &p) const
  {
    ElemPtrArray *this_no_const= const_cast<ElemPtrArray *>(this);
    return this_no_const->getNearestElement(p);
  }

//! @brief Returns a Python list containing the elements of this array.
boost::python::list XC::ElemPtrArray::getPyElementList(void) const
  {
    boost::python::list retval;
    if(!Null())
      {
	const size_t numberOfRows= getNumberOfRows();
	const size_t numberOfColumns= getNumberOfColumns();
	for(size_t j= 1;j<=numberOfRows;j++)
	  for(size_t k= 1;k<=numberOfColumns;k++)
	    {
	      const Element *elem= operator()(j,k);
   	      retval.append(elem);
	    }
      }
    return retval;
  }
