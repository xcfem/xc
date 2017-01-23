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
// File: ~/system_of_eqn/eigenSOE/BandArpackSOE.h
//
// Written: Jun Peng 
// Created: Febuary 1999
// Revision: A
//



#ifndef BandArpackSOE_h
#define BandArpackSOE_h

#include <solution/system_of_eqn/eigenSOE/ArpackSOE.h>
#include "utility/matrix/Vector.h"

namespace XC {

class BandArpackSolver;

//! @ingroup EigenSOE
//
//! @brief Sistema de ecuaciones correspondiente
//! a un problema de eigenvalues con matriz en banda
//! en cuya solución se empleará la biblioteca
//! ArPack (http://www.caam.rice.edu/software/ARPACK)
//! Description: This file contains the class definition for BandArpackSOE
//! BandArpackSOE is a subclass of ArpackSOE. It uses the LAPACK storage
//! scheme to store the components of the K, M matrix, which is a full matrix.
//! It uses the ARPACK to do eigenvalue analysis.
//! ARPACK is an eigen analysis package which was developed by 
//! R.B.Lehoucq, D.C.Sorensen and C.Yang at Rice University. ARPACK is a
//! collection of FORTRAN77 subroutines designed to solve large scale eigen
//! problems. ARPACK is capable of solving large scale non-Hermitian standard 
//! and generalized eigen problems. When the matrix <B>K</B> is symmetric, 
//! the method is a variant of the Lanczos process called Implicitly Restarted
//! Lanczos Method (IRLM).
class BandArpackSOE: public ArpackSOE
  {
  private:
    int numSuperD; //Numero de diagonales por encima de la principal.
    int numSubD; //Numero de diagonales por debajo de la principal.
    Vector A;
  protected:
    bool setSolver(EigenSolver *);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    BandArpackSOE(SoluMethod *, double shift = 0.0);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);
    
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    virtual int addM(const Matrix &, const ID &, double fact = 1.0);
   
    virtual void zeroA(void);
    virtual void zeroM(void);
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    

    friend class BandArpackSolver;
  };
inline SystemOfEqn *BandArpackSOE::getCopy(void) const
  { return new BandArpackSOE(*this); }
} // end of XC namespace


#endif



