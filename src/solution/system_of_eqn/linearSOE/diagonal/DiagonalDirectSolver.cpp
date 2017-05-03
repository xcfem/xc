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
// $Date: 2005/02/09 19:58:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/diagonal/DiagonalDirectSolver.cpp,v $

// Written: fmk 
// Created: Jan 2005
// Revision: A
//
// Description: This file contains the implementation for ProfileSPDLinSOESolver
// Description: This file contains the class definition for 
// DiagonalDirectSolver. DiagonalDirectSolver is a subclass 
// of LinearSOESOlver. It solves a XC::DiagonalSOE object directly!

// What: "@(#) DiagonalDirectSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/diagonal/DiagonalDirectSolver.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DiagonalSOE.h>
#include "utility/matrix/Vector.h"
#include <cmath>

XC::DiagonalDirectSolver::DiagonalDirectSolver(double tol)
:DiagonalSolver(SOLVER_TAGS_DiagonalDirectSolver), minDiagTol(tol)
  {}

    
int XC::DiagonalDirectSolver::setSize(void)
  {
    if(!theSOE)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__;
        std::cerr << " No system has been set!\n";
        return -1;
      }
    return 0;
  }


int XC::DiagonalDirectSolver::solve(void)
  {

    // check for XC::quick returns
    if(!theSOE)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__;
        std::cerr << " - No ProfileSPDSOE has been assigned\n";
        return -1;
      }
    
    if(theSOE->size == 0)
      return 0;
  
    // set some pointers
    double *Aptr = theSOE->A.getDataPtr();
    double *Bptr = theSOE->getPtrB();
    double *Xptr = theSOE->getPtrX();
    int size = theSOE->size;

    if(theSOE->factored == false)
      {
    
    // FACTOR & SOLVE
    double invD;
    for (int i=0; i<size; i++) {
      
      double aii = *Aptr;

      // check that the diag > the tolerance specified
      if (aii == 0.0) {
	std::cerr << nombre_clase() << "::" << __FUNCTION__ << "; ";
	std::cerr << " aii = 0 (i, aii): (" << i << ", " << aii << ")\n"; 
	return(-2);
      }
      if (fabs(aii) <= minDiagTol) {
	std::cerr << nombre_clase() << "::" << __FUNCTION__ << "; ";
	std::cerr << " aii < minDiagTol (i, aii): (" << i;
	std::cerr << ", " << aii << ")\n"; 
	return(-2);
      }		

      // store the inverse 1/Aii in A; and solve for Xi
      invD = 1.0/aii; 
      *Xptr++ = invD * *Bptr++;
      *Aptr++ = invD;
    }

    theSOE->factored = true;

  } else {

    // JUST SOLVE
    for (int i=0; i<size; i++) {
      *Xptr++ = *Aptr++ * *Bptr++;
    }
  }	
    
  return 0;
}

double XC::DiagonalDirectSolver::getDeterminant(void) 
{
  double determinant = 0.0;
  return determinant;
}

int XC::DiagonalDirectSolver::setDiagonalSOE(DiagonalSOE &theNewSOE)
{
  if (theSOE != 0) {
    std::cerr << nombre_clase() << "::" << __FUNCTION__ << "; ";
    std::cerr << " has already been called \n";	
    return -1;
  }
  
  theSOE = &theNewSOE;
  return 0;
}


int XC::DiagonalDirectSolver::sendSelf(CommParameters &cp)
  { return cp.sendDouble(minDiagTol,getDbTagData(),CommMetaData(0)); }


int XC::DiagonalDirectSolver::recvSelf(const CommParameters &cp)
  { return cp.receiveDouble(minDiagTol,getDbTagData(),CommMetaData(0)); }



