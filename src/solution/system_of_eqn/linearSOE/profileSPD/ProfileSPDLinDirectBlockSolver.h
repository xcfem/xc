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
// $Date: 2000/09/15 08:23:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectBlockSolver.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectBlockSolver.h
//
// Written: fmk 
// Created: Febuary 1997
// Revision: A
//
// Description: This file contains the class definition for 
// ProfileSPDLinDirectBlockSolver. ProfileSPDLinDirectBlockSolver is a subclass 
// of LinearSOESOlver. It solves a ProfileSPDLinSOE object using
// the LDL^t factorization.

// What: "@(#) ProfileSPDLinDirectBlockSolver.h, revA"

#ifndef ProfileSPDLinDirectBlockSolver_h
#define ProfileSPDLinDirectBlockSolver_h

#include "ProfileSPDLinDirectBase.h"

namespace XC {
class ProfileSPDLinSOE;

//! @ingroup Solver
//
//! @brief solves a ProfileSPDLinSOE object using
//! the LDL^t factorization.
class ProfileSPDLinDirectBlockSolver: public ProfileSPDLinDirectBase
  {
  protected:
    int blockSize;
    int maxColHeight;

    friend class LinearSOE;
    ProfileSPDLinDirectBlockSolver(double tol=1.0e-12, int blockSize = 4);    
    virtual LinearSOESolver *getCopy(void) const;
  public:
    virtual int solve(void);        
    virtual int setSize(void);    

    virtual int setProfileSOE(ProfileSPDLinSOE &theSOE);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline LinearSOESolver *ProfileSPDLinDirectBlockSolver::getCopy(void) const
   { return new ProfileSPDLinDirectBlockSolver(*this); }
} // end of XC namespace


#endif

