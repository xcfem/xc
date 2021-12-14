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
//IndicesMatrix.cc

#include "IndicesMatrix.h"

IndicesMatrix::IndicesMatrix(const IndicesMatrix &mp, size_t &f1, size_t &c1, size_t &f2, size_t &c2)
 : m_indices(mp,f1,c1,f2,c2) {}

IndicesMatrix::IndicesMatrix(const size_t &f,const size_t &c,const VIndices &p)
  : m_indices(f,c,p) {}

//! @brief Constructor.
//! @param posiciones: matrix in which each row is a vector [j,k] that points to
//! the position (row= j, column= k) of the vector that occupies the i row
//! in indices, stated otherwise
//! (*this)(j,k)= indices[i].
//! @param indices: matrix wich rows are the indexes vectors the are disposed
//! in this matrix.
IndicesMatrix::IndicesMatrix(const m_int &posiciones,const std::vector<VIndices> &indices)
  : m_indices(posiciones.column_maximum(1),posiciones.column_maximum(2)) 
  {
    const size_t sz= indices.size();
    const size_t nf= posiciones.column_maximum(1);
    const size_t nc= posiciones.column_maximum(2);
    if(sz>=nf*nc)
      {
        for(size_t i=1;i<sz;i++)
          {
            const size_t j= posiciones(i,1);
            const size_t k= posiciones(i,2);
            (*this)(j,k)= indices[i-1];
          }
      }
    else
      std::cerr << "IndicesMatrix; " << __FUNCTION__
	        << " argument matrix has a "
                << " number of rows (" << sz << ") too small"
		<< " (<" << nf*nc << ").\n";
  }
//! @brief Return the row cuyos índices is being passed as parameter.
IndicesMatrix IndicesMatrix::getBox(size_t f1, size_t c1, size_t f2, size_t c2) const
  { return IndicesMatrix(*this,f1,c1,f2,c2); }

//! @brief Return the iRow-th row.
IndicesMatrix IndicesMatrix::getRow(size_t iRow) const
  { return getBox(iRow,1,iRow,this->n_columns); }

//! @brief Retunr the iCol-th column.
IndicesMatrix IndicesMatrix::getColumn(size_t iColumn) const
  { return getBox(1,iColumn,this->n_rows,iColumn); }

//! @brief Agrega a todos los miembros el valor
//! que is being passed as parameter.
void IndicesMatrix::Offset(const VIndices &vi)
  {
    const size_t n_rows= getNumberOfRows();
    const size_t nCols= getNumberOfColumns();
    for(size_t i= 1;i<= n_rows;i++)
      for(size_t j= 1;j<= nCols;j++)
	(*this)(i,j)+= vi;
  }
