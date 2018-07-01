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
//FiberSet.cc

#include "FiberSet.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

//! @brief Constructor.
XC::FiberSet::FiberSet(const size_t &num)
  : FiberPtrDeque(num)
  {}

//! @brief Copy constructor.
XC::FiberSet::FiberSet(const FiberSet &otro)
  : FiberPtrDeque(otro)
  {}

//! @brief Assignment operator.
XC::FiberSet &XC::FiberSet::operator=(const FiberSet &otro)
  {
    FiberPtrDeque::operator=(otro);
    return *this;
  }

//! @brief Extend this container with the pointers from the container
//! being passed as parameter.
void XC::FiberSet::extend(const FiberSet &other)
  {
    for(const_iterator i= other.begin();i!=other.end();i++)
      push_back(*i);
  }

//! @brief += (union) operator.
XC::FiberSet &XC::FiberSet::operator+=(const FiberSet &other)
  {
    extend(other);
    return *this;
  }

//! @brief Return the union of both containers.
XC::FiberSet XC::operator+(const FiberSet &a,const FiberSet &b)
  {
    FiberSet retval(a);
    retval+=b;
    return retval;
  }

//! @brief Return the fibers in a that are not in b.
XC::FiberSet XC::operator-(const FiberSet &a,const FiberSet &b)
  {
    FiberSet retval;
    for(FiberSet::const_iterator i= a.begin();i!=a.end();i++)
      {
        Fiber *n= (*i);
        assert(n);
	if(!b.in(n)) //If not in b.
	  retval.push_back(n);
      }
    return retval;
  }

//! @brief Return the fibers in a that are also in b.
XC::FiberSet XC::operator*(const FiberSet &a,const FiberSet &b)
  {
    FiberSet retval;
    for(FiberSet::const_iterator i= a.begin();i!=a.end();i++)
      {
        Fiber *n= (*i);
        assert(n);
	if(b.in(n)) //If also in b.
	  retval.push_back(n);
      }
    return retval;    
  }



