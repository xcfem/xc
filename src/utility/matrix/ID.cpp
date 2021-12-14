//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.10 $
// $Date: 2005/11/23 22:37:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/matrix/ID.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class implementation for XC::ID.
//
// What: "@(#) ID.C, revA"

#include "ID.h"
#include <cstdlib>


#include <boost/python/extract.hpp>

int XC::ID::ID_NOT_VALID_ENTRY= 0;

//! @brief Default constructor, sets size = 0;
XC::ID::ID(void)
  :CommandEntity(), std::vector<int>(0) {}


//! @brief Constructor used to allocate a ID of size size.
//!
//! To construct a ID of size \p idSize. The constructor creates an
//! integer array of size \p idSize to store the data and zeroes this
//! array by invoking Zero() on itself. If not enough memory is
//! available an error message is printed and an ID of size \f$0\f$ is
//! returned.  
XC::ID::ID(const int &size)
  :CommandEntity(), std::vector<int>(size,0) {}


//! @brief Constructor.
XC::ID::ID(const std::vector<int> &v)
  : CommandEntity(), std::vector<int>(v)
  {}

//! @brief Constructor (Python interface).
XC::ID::ID(const boost::python::list &l)
  : CommandEntity(), std::vector<int>(len(l))
  {
    // copy the data
    const size_t sz= size();
    for(size_t i=0; i<sz; i++)
      (*this)[i]= boost::python::extract<int>(l[i]);
  }

XC::ID::ID(const std::set<int> &setInt)
  : CommandEntity(), std::vector<int>(setInt.size())
  {
    // copy the data
    int count= 0;
    for(std::set<int>::const_iterator i= setInt.begin();i!=setInt.end();i++,count++)
      (*this)[count]= *i;
  }

//! @brief Zeros out the ID, i.e. sets all the components of the ID to
//! \f$0\f$. 
void XC::ID::Zero(void)
  { std::fill(begin(),end(),0); }

//! @brief Returns the position of 'value' in the vector.
//!
//! Will return the location the first location in the ID of the integer
//! \p x. If \p x is not in the ID a \f$-1\f$ is returned.
int XC::ID::getLocation(const int &value) const
  {
    const size_t sz= size();
    // search through ID for the value
    for(size_t i=0; i<sz; i++)
      if((*this)[i] == value)
        return i;
    // if we get here the value is not in the array
    return -1;
  }


int XC::ID::getLocationOrdered(const int &value) const
  {
    const size_t sz= size();    
    int middle = 0;
    int left = 0;
    int right = sz-1;
    if(sz!=0)
      {
        while(left <= right)
          {
            middle = (left + right)/2;
            double dataMiddle = (*this)[middle];
            if(value == dataMiddle)
              return middle;   // already there
            else if(value>dataMiddle)
              left = middle + 1;
            else 
              right = middle-1;
          }
      }
    // if we get here the value is not in the array
    return -1;
  }


//! @brief Remove value from the array.
//!
//! Will return the last location in the ID of the integer
//! \p x. If \p x is not in the ID a \f$-1\f$ is returned. All the integer
//! components \p x are removed from the ID and the length of the ID is
//! reduced by the number of the removed components. The \p arraySize
//! remains unchanged. 
int XC::ID::removeValue(const int &value)
  {
    int place = -1;
    
    auto it = std::find(begin(), end(), value);
    if(it != end())
      {
        place= std::distance(begin(), it);
        // really remove all elements equal to value
        erase(std::remove(begin(), end(), value), end());
      }
    return place;
  }    

//! @brief Reverse sequence.
void XC::ID::reverse(void)
  { std::reverse(begin(),end()); }

//! @brief Return the reversed sequence.
XC::ID XC::ID::getReversed(void) const
  {
    ID retval(*this);
    retval.reverse();
    return retval;
  }

//! @brief Changes the size of the array.
int XC::ID::resize(const int &newSize, const int &fill_value)
  {
    int retval= 0;
    if(newSize<0) // first check that newSize is valid
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; size specified " << newSize << " < 0\n";
        retval= -1;
      }
    else if(newSize==0)
      clear();
    else
      std::vector<int>::resize(newSize,fill_value);
    return retval;
  }

//! @brief Fills the array with the argument.
void XC::ID::fill(const int &fill_value)
  {
    const size_t sz= size();    
    for(size_t i=0; i<sz; i++)
      (*this)[i]= fill_value;
  }

//! @brief Returns the maximum of vector components.
const int &XC::ID::max(void) const
  { return *std::max_element(begin(),end()); }

//! @brief Returns the minimum of vector components.
const int &XC::ID::min(void) const
  { return *std::min_element(begin(),end()); }

//! @brief A function is defined to allow user to print the IDs using streams.
std::ostream &XC::operator<<(std::ostream &s, const XC::ID &V)
  {
    const size_t sz= V.size();
    for(size_t i=0; i<sz;i++) 
      s << V(i) << " ";
    return s;
  }

XC::ID XC::getIDFromIntPtr(const int *d,const int &size)
  {
    ID retval(std::vector<int>(d,d+size));
    if(!d)
      {
	std::cerr << __FUNCTION__
		  << "; can't copy from null pointer."
	          << std::endl;
      }
    return retval;
  }

// friend istream &operator>>(istream &s, ID &V)
//        A function is defined to allow user to input the data into a XC::ID which has already
//        been constructed with data, i.e. ID(int) or XC::ID(const XC::ID &) constructors.

/*
istream &operator>>(istream &s, ID &V)
{
    for(size_t i=0; i<V.Size(); i++) 
        s >> V(i);

    return s;
}
*/



