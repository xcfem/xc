// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//Normalize.h

#ifndef NORMALIZE_H
#define NORMALIZE_H


#include "ZMatrix.h"

template<class treal>
//! @brief Divide the components of the matrix by its norm.
inline void normalize(ZMatrix<treal> &m)
  {
    const treal vabs= Abs(m);
    if(vabs>0) m*= 1.0/vabs;
  }

template<class treal>
//! @brief Divide the components of the matrix by its norm.
inline ZMatrix<treal> normalize(const ZMatrix<treal> &m)
  {
    ZMatrix<treal> retval(m);
    normalize(retval);
    return retval;
  }

#endif
