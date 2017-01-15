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
                                                                        
// $Revision: 1.3 $
// $Date: 2006/01/13 00:02:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/PetscSolver.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// PetscSolver. It solves the FullGenLinSOE object by calling
// Lapack routines.
//
// What: "@(#) PetscSolver.h, revA"

#ifndef PetscSolver_h
#define PetscSolver_h

#include <petscksp.h>
#include "solution/system_of_eqn/linearSOE/LinearSOESolver.h"

namespace XC {
class PetscSOE;

//! @ingroup Solver
//
//! @brief Objeto encargado de la solución del
//! sistema de ecuaciones basado en la biblioteca
//! PETSC (http://www.mcs.anl.gov/petsc/petsc-as).
class PetscSolver : public LinearSOESolver
  {
  private:
    KSP ksp;                   
    PC pc;
    int its;
    std::string method;
    std::string preconditioner;
    double rTol;
    double aTol;
    double dTol; 
    int maxIts;

    static int numSolver;
  protected:
    PetscSOE *theSOE;

    PetscSolver();    
    PetscSolver(KSPType mth, PCType precond);    
    PetscSolver(KSPType mth, PCType precond, double rTol, double aTol, double dTol, int maxIts);    
    virtual LinearSOESolver *getCopy(void) const;
    bool setLinearSOE(LinearSOE *theSOE);
  public:
    ~PetscSolver(void);

    int solve(void);
    int setSize(void);
    virtual bool setLinearSOE(PetscSOE &theSOE);
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);    

    friend class ActorPetscSOE;
    friend class ShadowPetscSOE;
  };

inline LinearSOESolver *PetscSolver::getCopy(void) const
   { return new PetscSolver(*this); }
} // end of XC namespace

#endif

