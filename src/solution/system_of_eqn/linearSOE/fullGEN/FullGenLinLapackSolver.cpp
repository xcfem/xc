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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/04/02 22:02:52 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/fullGEN/FullGenLinLapackSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/FullGEN/FullGenLinLapackSolver.h
//
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// FullGenLinLapackSolver. It solves the XC::FullGenLinSOE object by calling
// Lapack routines.
//
// What: "@(#) FullGenLinLapackSolver.h, revA"

#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinLapackSolver.h>
#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.h>

XC::FullGenLinLapackSolver::FullGenLinLapackSolver()
  : FullGenLinSolver(SOLVER_TAGS_FullGenLinLapackSolver) {}


extern "C" int dgesv_(int *N, int *NRHS, double *A, int *LDA, int *iPiv, 
		      double *B, int *LDB, int *INFO);

extern "C" int dgetrs_(char *TRANS, int *N, int *NRHS, double *A, int *LDA, 
		       int *iPiv, double *B, int *LDB, int *INFO);		       
int XC::FullGenLinLapackSolver::solve(void)
  {
    if(!theSOE)
      {
	std::cerr << "WARNING XC::FullGenLinLapackSolver::solve(void)- ";
	std::cerr << " No XC::LinearSOE object has been set\n";
	return -1;
      }
    
    int n= theSOE->size;
    
    // check for XC::quick return
    if(n == 0)
      return 0;
    
    // check iPiv is large enough
    if(iPiv.Size() < n)
      {
	std::cerr << "WARNING XC::FullGenLinLapackSolver::solve(void)- ";
	std::cerr << " iPiv not large enough - has setSize() been called?\n";
	return -1;
      }	
	
    int ldA= n;
    int nrhs= 1;
    int ldB= n;
    int info;
    double *Aptr = theSOE->A.getDataPtr();
    double *Xptr = theSOE->getPtrX();
    double *Bptr = theSOE->getPtrB();
    int *iPIV= iPiv.getDataPtr();
    
    // first copy B into X
    for(int i=0; i<n; i++)
      *(Xptr++) = *(Bptr++);
    Xptr= theSOE->getPtrX();

    // now solve AX = Y

    char strN[]= "N";
    {if (theSOE->factored == false)      
	dgesv_(&n,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);
     else
	dgetrs_(strN, &n,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);
    }
    
    // check if successfull
    if (info != 0) {
	std::cerr << "WARNING XC::FullGenLinLapackSolver::solve()";
	std::cerr << " - lapack solver failed - " << info << " returned\n";
	return -info;
    }

    
    theSOE->factored = true;
    return 0;
}


int XC::FullGenLinLapackSolver::setSize()
  {
    const int n = theSOE->size;
    if(n > 0)
      {
	if(iPiv.Size() < n)
          iPiv.resize(n);
      }
    else if(n == 0)
      return 0;
    else
      {
	std::cerr << "WARNING XC::FullGenLinLapackSolver::setSize()"
	          << " - ran out of memory\n";
	return -1;	
      }
    return 0;
  }

int XC::FullGenLinLapackSolver::sendSelf(CommParameters &cp)

  {
    // nothing to do
    return 0;
  }

int XC::FullGenLinLapackSolver::recvSelf(const CommParameters &cp)
  {
    // nothing to do
    return 0;
  }



