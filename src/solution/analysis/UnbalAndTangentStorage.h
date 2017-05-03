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
//UnbalAndTangentStorage.h
                                                                        
                                                                        
#ifndef UnbalAndTangentStorage_h
#define UnbalAndTangentStorage_h

#include <vector>
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

namespace XC {

//! @ingroup Analysis
//
//! @brief Unbalanced force vector and tangent stiffness matrix.
class UnbalAndTangentStorage
  {
  private:
    std::vector<Matrix> theMatrices; //!< array of matrices
    std::vector<Vector> theVectors;  //!< array of vectors

  public:
    UnbalAndTangentStorage(const size_t &);    

    Vector *setUnbalance(const size_t &);
    Matrix *setTangent(const size_t &);

    inline size_t size(void) const
      { return theMatrices.size(); }

    const Matrix &getTangent(const size_t &) const;
    Matrix &getTangent(const size_t &);
    const Vector &getUnbalance(const size_t &) const;
    Vector &getUnbalance(const size_t &);
  };
} // end of XC namespace

#endif

