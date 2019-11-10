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

// $Revision: 1.2 $
// $Date: 2007/12/04 22:29:05 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/eigenSOE/FullGenEigenSolver.h,v $


#ifndef FullGenEigenSolver_h
#define FullGenEigenSolver_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 11/07
// Revision: A
//
// Description: This file contains the class definition for 
// FullGenEigenSolver. It computes the generalized eigenvalues
// and eigenvectors of a pair of real nonsymmetric matrices using
// the LAPACK subroutine DGGEV.

#include "EigenSolver.h"
#include "utility/matrix/Vector.h"

namespace XC {
class FullGenEigenSOE;

//! @ingroup EigenSolver
//
//! @brief base class for full (dense) matrix eigenvalue SOE solvers.
class FullGenEigenSolver: public EigenSolver
  {
  private:
    void sort(int length, Vector &x, ID &id);

    FullGenEigenSOE *theSOE;
    int numEigen;

    Vector eigenvalue;
    Vector eigenvector;
    ID sortingID;
    mutable Vector eigenV;

    friend class EigenSOE;
    FullGenEigenSolver();
    virtual EigenSolver *getCopy(void) const;
    bool setEigenSOE(EigenSOE *theSOE);
  public:

    virtual int solve(void);
    virtual int setSize(void);
    const int &getSize(void) const;
    virtual bool setEigenSOE(FullGenEigenSOE &theSOE);

    virtual const Vector &getEigenvector(int mode) const;
    virtual const double &getEigenvalue(int mode) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline EigenSolver *FullGenEigenSolver::getCopy(void) const
   { return new FullGenEigenSolver(*this); }
} // end of XC namespace

#endif
