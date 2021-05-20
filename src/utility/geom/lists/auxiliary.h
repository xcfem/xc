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
//auxiliary.h
//Auxiliary functions.

#ifndef AUXILIARY_H
#define AUXILIARY_H


//! @ingroup GEOM
//
//! @brief Return the maximum value of i-th coordinate.
template <class InputIterator>
inline GEOM_FT get_max_i( short unsigned int i,
                                 InputIterator first,
                                 InputIterator last)
  {
    if(first==last)
      {
	std::cerr << "get_max_i: La lista está vacía, se devuelve 0."
		  << std::endl;
        return 0.0;
      }
    InputIterator j= first;
    GEOM_FT mx= (*j)(i); j++;
    for(;j!=last;j++)
      mx= std::max((*j)(i),mx);
    return mx;
  }

//! @ingroup GEOM
//
//! @brief Return the maximum value of i-th coordinate.
template <class InputIterator>
inline GEOM_FT get_min_i( short unsigned int i,
                                 InputIterator first,
                                 InputIterator last)
  {
    if(first==last)
      {
        std::cerr << "get_max_i: La lista está vacía, se devuelve 0."
		  << std::endl;
        return 0.0;
      }
    InputIterator j= first;
    GEOM_FT mn= (*j)(i); j++;
    for(;j!=last;j++)
      mn= std::min((*j)(i),mn);
    return mn;
  }

#endif
