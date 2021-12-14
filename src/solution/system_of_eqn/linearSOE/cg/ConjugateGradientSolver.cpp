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
// $Date: 2003/02/14 23:02:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/cg/ConjugateGradientSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/cg/ConjugateGradientSolver.C
//
// Written: fmk 
// Created: 06/00
// Revision: A

#include "solution/system_of_eqn/linearSOE/cg/ConjugateGradientSolver.h"
#include <utility/matrix/Vector.h>

#include <solution/system_of_eqn/linearSOE/LinearSOE.h>

XC::ConjugateGradientSolver::ConjugateGradientSolver(int classtag, 
						 LinearSOE *theSOE,
						 double tol)
:LinearSOESolver(classtag),
 theLinearSOE(theSOE), 
 tolerance(tol)
  {}

int XC::ConjugateGradientSolver::setSize(void)
  {
    int n = theLinearSOE->getNumEqn();
    if(n <= 0)
      {
	std::cerr << "ConjugateGradientSolver::setSize() - n < 0 \n";
	return -1;
      }

    if(r.Size() != n)
      {
        r.resize(n);
        p.resize(n);
        Ap.resize(n);
        x.resize(n);	
      }
    return 0;
  }



int XC::ConjugateGradientSolver::solve(void)
  {
    // initialize
    x.Zero();    
    r= theLinearSOE->getB();
    p= r;
    double rdotr= r^r;
    
    // lopp till convergence
    while(r.Norm() > tolerance)
      {
	this->formAp(p, Ap);

	double alpha = rdotr/(p ^ Ap);

	// x+= p * alpha;
	x.addVector(1.0, p, alpha);

	// *r -= *Ap * alpha;
	r.addVector(1.0, Ap, -alpha);

	double oldrdotr = rdotr;

	rdotr = r ^ r;

	double beta = rdotr / oldrdotr;

	// p = r + p * beta;
	p.addVector(beta, r, 1.0);
    }
    return 0;
  }











