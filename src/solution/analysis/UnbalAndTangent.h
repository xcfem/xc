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
//UnbalAndTangent.h
                                                                        
                                                                        
#ifndef UnbalAndTangent_h
#define UnbalAndTangent_h

#include "solution/analysis/UnbalAndTangentStorage.h"

namespace XC {

//! @ingroup Analisis
//
//! @brief Vector de fuerza desequilibrada y matriz de rigidez tangente.
class UnbalAndTangent
  {
  private:
    size_t nDOF;
    Vector *theResidual;
    Matrix *theTangent;
    UnbalAndTangentStorage &unbalAndTangentArray; //!< Reference to array of class wide vectors and matrices
    bool libera(void);
    void alloc(void);
    void copia(const UnbalAndTangent &otro);

  public:
    UnbalAndTangent(const size_t &,UnbalAndTangentStorage &);
    UnbalAndTangent(const UnbalAndTangent &otro);
    UnbalAndTangent &operator=(const UnbalAndTangent &otro);
    virtual ~UnbalAndTangent(void);

    inline const size_t &getNumDOF(void) const
      { return nDOF; }

    const Matrix &getTangent(void) const;
    Matrix &getTangent(void);
    const Vector &getResidual(void) const;
    Vector &getResidual(void);
  };
} // end of XC namespace

#endif


