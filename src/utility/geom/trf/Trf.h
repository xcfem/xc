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
//Trf.h
//Geometric transformation.

#ifndef TRF_H
#define TRF_H

#include "../cgal_types.h"
#include "utility/kernel/CommandEntity.h"

class FT_matrix;


//! @ingroup GEOM
//
//! @brief Base class for coordinate transformation.
class Trf: public CommandEntity
  {
  public:
    inline virtual ~Trf(void) {}
    //! @brief Return the (i,j) component of the transformation matrix
    //! expressed in cartesian coordinates.
    //       -              -
    //       | m11  m12 m13 |
    //       | m21  m22 m23 |
    //       |   0    0  1  |
    //       -              -
    virtual GEOM_FT Cartesianas(const size_t &i,const size_t &j) const= 0;
    
    //! @brief Return the (i,j) component of the transformation matrix
    //! expressed in homogeneous coordinates.
    //       -              -
    //       | m11  m12 m13 |
    //       | m21  m22 m23 |
    //       |   0    0  hw |
    //       -              -
    virtual GEOM_FT Homogeneas(const size_t &i,const size_t &j) const= 0;
    //!@brief Return the tranformation matrix expressed in
    //! cartesian coordinates.
    virtual FT_matrix Cartesianas(void) const= 0;
    //!@brief Return the tranformation matrix expressed in
    //! homogenous coordinates.
    virtual FT_matrix Homogeneas(void) const= 0;
  };

#endif
