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
#include "ASDEICR.h"


//! @brief Sets the input matrix to the zero matrix of requested size. Resize is done if necessary
//! @param n the number of rows
//! @param n the number of columns
//! @param I the input/output matrix
void XC::EICR::SetZero(size_t n, size_t m, MatrixType& I)
  {
    if ((I.noRows() != n) || (I.noCols() != m))
	    I.resize(n, m);
    I.Zero();
  }

//! @brief computes the outer product A x B, in C.
//! Note: no size check is made!
//! @param A the first vector
//! @param B the second vector
//! @param C the output matrix
void XC::EICR::OuterProd(const VectorType& A, const VectorType& B, MatrixType& C)
  {
    for(size_t i = 0; i < A.Size(); ++i)
      for(size_t j = 0; j < B.Size(); ++j)
	C(i, j) = A(i) * B(j);
  }
