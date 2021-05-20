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
//FT_matrix.h

#ifndef FT_MATRIX_H
#define FT_MATRIX_H

#include "utility/matrices/ZMatrix.h"
#include "cgal_types.h"



//Elementos neutros para la suma y el producto.
GEOM_FT neutro_suma(const GEOM_FT &);
GEOM_FT neutro_producto(const GEOM_FT &);


//! @ingroup GEOM
//
//! @brief Matrix which components are GEOM_FT numbers.
class FT_matrix: public ZMatrix<GEOM_FT>
  {
    FT_matrix(const FT_matrix &orig,size_t f1, size_t c1, size_t f2, size_t c2);
  public:
    FT_matrix(void);
    FT_matrix(size_type rows,size_type cols);
    FT_matrix(size_type rows,size_type cols,GEOM_FT val);
    FT_matrix getBox(size_t f1, size_t c1, size_t f2, size_t c2) const;
    FT_matrix getRow(size_t iRow) const;
    FT_matrix getColumn(size_t col) const;
    FT_matrix GetTrn(void) const;
    friend FT_matrix operator+(const FT_matrix &a,const FT_matrix &b);
    friend FT_matrix operator-(const FT_matrix &a,const FT_matrix &b);
    friend FT_matrix operator*(const FT_matrix &a,const FT_matrix &b);
    friend FT_matrix operator*(const GEOM_FT &d,const FT_matrix &a);
    friend FT_matrix operator*(const FT_matrix &a,const GEOM_FT &d);
    friend FT_matrix operator^(const FT_matrix &,const FT_matrix &v2);
  };

FT_matrix identity(const size_t &f);
FT_matrix identity(const FT_matrix &m);
FT_matrix traspuesta(const FT_matrix &m);


//Producto tensorial de dos tensores de primer orden.
FT_matrix prod_tensor(const FT_matrix &u,const FT_matrix &v);

//Operador producto tensorial.
inline FT_matrix operator&(const FT_matrix &u,const FT_matrix &v)
  { return prod_tensor(u,v); }

void normalize(FT_matrix &m);
void NormalizeRows(FT_matrix &m);
FT_matrix normalize(const FT_matrix &m);

FT_matrix operator-(const FT_matrix &m);

ZMatrix<double> to_double(const FT_matrix &m);
FT_matrix from_double(const ZMatrix<double> &m);

namespace boost
  {
    class any;
  }

#endif
