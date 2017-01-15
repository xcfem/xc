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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver.h
//
// Written: fmk 
// Created: Febuary 1997
// Revision: A
//
// Description: This file contains the class definition for 
// ProfileSPDLinSubstrSolver. ProfileSPDLinSubstrSolver is a 
// subclass of DomainSolver and ProfileSPDlinDirectSolver.
// It perform the operations needed of a substr domain decomp method
// on a ProfileSPDlinSOE object. 

// What: "@(#) ProfileSPDLinSubstrSolver.h, revA"

#ifndef ProfileSPDLinSubstrSolver_h
#define ProfileSPDLinSubstrSolver_h

#include <solution/system_of_eqn/linearSOE/DomainSolver.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver.h>
#include "utility/matrix/Matrix.h"

namespace XC {
class ProfileSPDLinSOE;

//! @ingroup LinearSolver
//
//! @brief Objetos que se encargan de obtener la solución
//! de sistemas de ecuaciones lineales con matriz de perfil.
class ProfileSPDLinSubstrSolver : public ProfileSPDLinDirectSolver,
                                  public DomainSolver
  {
  private:
    int dSize;
    Vector DU;
    Matrix Aext;
    Vector Yext;

  protected:
    friend class FEM_ObjectBroker;
    ProfileSPDLinSubstrSolver(double tol=1.0e-12);    
    virtual LinearSOESolver *getCopy(void) const;
  public:

    int solve(void);
    int condenseA(int numInt);
    int condenseRHS(int numInt, Vector *v =0);
    int computeCondensedMatVect(int numInt, const Vector &u);    
    const Matrix &getCondensedA(void);
    const Vector &getCondensedRHS(void);
    const Vector &getCondensedMatVect(void);
    
    int setComputedXext(const Vector &);
    int solveXint(void);

    int setSize(void);
    int getClassTag() const;
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline LinearSOESolver *ProfileSPDLinSubstrSolver::getCopy(void) const
   { return new ProfileSPDLinSubstrSolver(*this); }
} // end of XC namespace


#endif

