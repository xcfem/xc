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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1 $
// $Date: 2001/11/19 22:44:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/eigenSOE/SymBandEigenSolver.h,v $

// Written: MHS
// Created: Oct 2001
//
// Description: This file contains the class definition for
// SymBandEigenSolver, which computes the eigenvalues and eigenvectors
// of a symmetric banded matrix using the LAPACK subroutine dsbevx.

#ifndef SymBandEigenSolver_h
#define SymBandEigenSolver_h

#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>
#include "utility/matrix/Vector.h"

namespace XC {
class Vector;
class SymBandEigenSOE;

//! @ingroup EigenSolver
//
//! @brief Base class for symmetric band matrix
//! eigenvalue SOE solvers.
class SymBandEigenSolver : public EigenSolver
  {
  private:
    SymBandEigenSOE *theSOE;

    Vector eigenvalue;
    Vector eigenvector;
    mutable Vector eigenV;
    Vector work;

    friend class EigenSOE;
    SymBandEigenSolver(void);    
    virtual EigenSolver *getCopy(void) const;
    bool setEigenSOE(EigenSOE *theSOE);
  public:  
    virtual int solve(void);
    virtual int solve(int nModes);
    virtual int setSize(void);
    const int &getSize(void) const;
    virtual bool setEigenSOE(SymBandEigenSOE &theSOE);
  
    virtual const Vector &getEigenvector(int mode) const;
    virtual const double &getEigenvalue(int mode) const;
  
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline EigenSolver *SymBandEigenSolver::getCopy(void) const
   { return new SymBandEigenSolver(*this); }
} // end of XC namespace

#endif
