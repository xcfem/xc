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
// $Date: 2003/02/25 23:34:45 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSkypackSolver.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSkypackSolver.h
//
// Written: fmk 
// Created: 03/98
// Revision: A
//
// Description: This file contains the class definition for 
// ProfileSPDLinDirectSkypackSolver. ProfileSPDLinDirectSkypackSolver 
// is a subclass of LinearSOESOlver. It solves a ProfileSPDLinSOE object using
// the Skypack library developed by Osni Marques, software available at
//   http://www.nersc.gov/research/SCG/Osni/marques_software.html

// What: "@(#) ProfileSPDLinDirectSkypackSolver.h, revA"

#ifndef ProfileSPDLinDirectSkypackSolver_h
#define ProfileSPDLinDirectSkypackSolver_h

#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSolver.h>
#include "utility/matrix/Vector.h"

namespace XC {
class ProfileSPDLinSOE;

//! @ingroup LinearSolver
//
//! @brief Objeto encargado de obtener la solución
//! de un sistemas de ecuaciones lineal con matriz de perfil.
class ProfileSPDLinDirectSkypackSolver : public ProfileSPDLinSolver
  {
  private:
    int mCols, mRows;
    Vector rw; // work array of dimension mRows*mCols
    Vector tw; // work array of dimension mRows*mRows
    ID index; // integer array of dimension max(mRows,mCols)		     
    int size;
    
    Vector invD;
    int block[3];

    ProfileSPDLinDirectSkypackSolver();    
    ProfileSPDLinDirectSkypackSolver(int mCols, int mRows);    
    virtual LinearSOESolver *getCopy(void) const;
  public:
    virtual int solve(void);        
    virtual int setSize(void);    

    virtual int setProfileSOE(ProfileSPDLinSOE &theSOE);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline LinearSOESolver *ProfileSPDLinDirectSkypackSolver::getCopy(void) const
   { return new ProfileSPDLinDirectSkypackSolver(*this); }
} // end of XC namespace


#endif

