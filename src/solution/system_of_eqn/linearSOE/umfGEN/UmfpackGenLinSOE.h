// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
// $Date: 2001/12/07 00:17:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.h
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for 
// UmfpackGenLinSOE. It stores the sparse matrix A in a fashion
// required by the UmfpackLinSolver object.
//
// What: "@(#) UmfpackGenLinSOE.h, revA"


#ifndef UmfpackGenLinSOE_h
#define UmfpackGenLinSOE_h

#include "solution/system_of_eqn/linearSOE/LinearSOEData.h"
#include "utility/matrix/Vector.h"

namespace XC {
class UmfpackGenLinSolver;

//! @brief System of equations that can be used with the UMFPACK routines
//! (Unsymmetric MultiFrontal Method). See <a href="http://faculty.cse.tamu.edu/davis/research.html" target="_new"> SuiteSparse</a>.
//! @ingroup SOE
class UmfpackGenLinSOE: public LinearSOEData
  {
  private:
    std::vector<double> Ax;
    std::vector<int> Ap, Ai;
  protected:
    bool setSolver(LinearSOESolver *);

    friend class SolutionStrategy;
    UmfpackGenLinSOE(SolutionStrategy *);        
    SystemOfEqn *getCopy(void) const;
  public:
    int setSize(Graph &theGraph);
    int addA(const Matrix &, const ID &, double fact = 1.0);
    
    void zeroA(void);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    friend class UmfpackGenLinSolver;

  };
} // end of XC namespace


#endif

