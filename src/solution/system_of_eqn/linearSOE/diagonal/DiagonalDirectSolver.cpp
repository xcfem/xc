//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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
        std::cerr << "XC::DiagonalDirectSolver::setSize()";
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
        std::cerr << "XC::DiagonalDirectSolver::solve(void): ";
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
	std::cerr << "XC::DiagonalDirectSolver::solve() - ";
	std::cerr << " aii = 0 (i, aii): (" << i << ", " << aii << ")\n"; 
	return(-2);
      }
      if (fabs(aii) <= minDiagTol) {
	std::cerr << "XC::DiagonalDirectSolver::solve() - ";
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
    std::cerr << "XC::DiagonalDirectSolver::setProfileSOE() - ";
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



