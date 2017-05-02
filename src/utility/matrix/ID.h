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
// $Source: /usr/local/cvs/OpenSees/SRC/utility/matrix/ID.h,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for ID.
// ID is a concrete class implementing the integer array abstraction.
// ID objects are Vectors of integers which only need a few
// operators defined on them.
//
// What: "@(#) ID.h, revA"


#ifndef ID_h
#define ID_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include <vector>
#include <boost/python/list.hpp>

namespace XC {
//! @ingroup Utils
//
//! @defgroup Matrix Vector, matrix and tensors classes.
//
//! @ingroup Matrix
//
//! @brief Vector of integers.
class ID: public EntCmd, public std::vector<int>
  {
  public:
    typedef std::vector<int> v_int;
  private:
    static int ID_NOT_VALID_ENTRY;
  public:
    // constructors and destructor
    ID(void);
    explicit ID(const int &);
    explicit ID(const v_int &);
    ID(const boost::python::list &);
    explicit ID(const std::set<int> &);
    template <class InputIterator>
    inline ID(InputIterator first, InputIterator last)
      : EntCmd(), std::vector<int>(first,last) {}
    inline virtual ~ID(){}
 
    // utility methods
    //! @brief Returns the vector size.
    inline int Size(void) const
      { return size(); }
    void Zero(void);
    //! @brief Returns a const pointer to the vector data.
    inline const int *getDataPtr(void) const
      { return &(*this)[0]; }
    //! @brief Returns a const pointer to the vector data.
    inline int *getDataPtr(void)
      { return &(*this)[0]; }
    //! @brief Returns true if the vector is empty.
    inline bool Nulo(void) const
      { return empty(); }
    int resize(const int &newSize);
    const int &max(void) const;
    const int &min(void) const;

    bool checkRange(const int &) const;
    int &operator()(const int &);
    const int &operator()(const int &) const;
    //! @brief Returns a reference to the element at position i in the container (does range checking => slower than () operator).
    inline int &operator[](const int &i)
      { return this->at(i); }
    //! @brief Returns a reference to the element at position i in the container (does range checking => slower than () operator).
    inline const int &operator[](const int &i) const
      { return this->at(i); }
  

    int getLocation(const int &) const;
    int getLocationOrdered(const int &) const; // for when insert was used to add elements
    int removeValue(const int &);

    friend std::ostream &operator<<(std::ostream &, const ID &);
    //    friend istream &operator>>(istream &s, ID &V);    


    friend class UDP_Socket;
    friend class TCP_Socket;
    friend class TCP_SocketNoDelay;
    friend class MPI_Channel;
  };

ID getIDFromIntPtr(const int *, const int &);

std::ostream &operator<<(std::ostream &, const ID &);

//! @brief check if argument is inside range [0,sz-1]
inline bool ID::checkRange(const int &i) const
  {
    const int sz= Size();
    if((i < 0) || (i >= sz)) //Range checking.
      {
        std::cerr << "ID::(loc) - loc "
		  << i << " outside range 0 - " <<  sz-1 << std::endl;
        return false;
      }
    else
      return true;
  }
 
//! @brief Returns a reference to the element at position i in the container (does not range checking => faster than [] operator).
inline int &ID::operator()(const int &i) 
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(!checkRange(i))
      return ID_NOT_VALID_ENTRY;
#endif
    return (*this)[i];
  }

//! @brief Returns a const reference to the element at position i in the container (does not range checking => faster than [] operator).
inline const int &ID::operator()(const int &i) const 
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(!checkRange(i))
      return ID_NOT_VALID_ENTRY;
#endif

    return (*this)[i];
  }

} // end of XC namespace

#endif



