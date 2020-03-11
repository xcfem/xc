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
//AuxMatrix.h
                                                                        
                                                                        
#ifndef AuxMatrix_h
#define AuxMatrix_h

#include "Vector.h"
#include "ID.h"

namespace XC {

#define MATRIX_VERY_LARGE_VALUE 1.0e213

//! @ingroup Matrix
//
//! Double and integer arrays used for calling
//! LAPACK routines.
class AuxMatrix: public CommandEntity
  {
  private:
    size_t sizeDoubleWork;
    size_t sizeIntWork;
    Vector matrixWork;
    ID intWork;

    AuxMatrix(const AuxMatrix &);
    AuxMatrix &operator=(const AuxMatrix &);
  public:
    // constructors and destructor
    AuxMatrix(const size_t &,const size_t &);
 
    void resize(const size_t &,const size_t &);

    const size_t &getSizeDoubleWork(void) const;
    const size_t &getIntDoubleWork(void) const;
    const double *getMatrixWork(void) const;
    double *getMatrixWork(void);
    const int *getIntWork(void) const;
    int *getIntWork(void);
  };

} // end of XC namespace

#endif




