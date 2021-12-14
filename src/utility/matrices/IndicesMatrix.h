//----------------------------------------------------------------------------
//  biblioteca bibXCLcmd; bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//IndicesMatrix.h
//Matrix of indices

#ifndef INDICESMATRIX_H
#define INDICESMATRIX_H

#include "utility/matrices/TMatrix.h"
#include "utility/matrices/vectorZ.h"
#include "m_int.h"

typedef vectorZ<int> VIndices;

//! @ingroup Matrices
//
//! @brief Matrix of indices. I'ts used to express a
//! set of positions by its indices, i. e. :
//! [[1,2][2,2][3,2]
//!  [1,1][2,1][2,3]]
class IndicesMatrix: public TMatrix<VIndices,std::vector<VIndices > >
  {
  public:
    typedef TMatrix<VIndices,std::vector<VIndices > > m_indices;
  protected:
    IndicesMatrix(const IndicesMatrix &mp, size_t &f1, size_t &c1, size_t &f2, size_t &c2);
  public:
    IndicesMatrix(const size_t &f=1,const size_t &c=1,const VIndices &p= VIndices());
    IndicesMatrix(const m_int &,const std::vector<VIndices > &);

    IndicesMatrix getBox(size_t f1, size_t c1, size_t f2, size_t c2) const;
    IndicesMatrix getRow(size_t ) const;
    IndicesMatrix getColumn(size_t ) const;

    void Offset(const VIndices &);
  };


#endif
