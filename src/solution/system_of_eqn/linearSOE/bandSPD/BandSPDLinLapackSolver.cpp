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
// $Date: 2003/04/02 22:02:52 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandSPD/BandSPDLinLapackSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/bandSPD/BandSPDLinLapackSolver.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the implementation of XC::BandSPDLinLapackSolver.
//
// What: "@(#) BandSPDLinLapackSolver.h, revA"

#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinLapackSolver.h>
#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.h>

XC::BandSPDLinLapackSolver::BandSPDLinLapackSolver()
  :BandSPDLinSolver(SOLVER_TAGS_BandSPDLinLapackSolver)
  {}

extern "C" int dpbsv_(char *UPLO, int *N, int *KD, int *NRHS, 
		      double *A, int *LDA, double *B, int *LDB, 
		      int *INFO);

extern "C" int dpbtrs_(char *UPLO, int *N, int *KD, int *NRHS, 
		       double *A, int *LDA, double *B, int *LDB, 
		       int *INFO);

int XC::BandSPDLinLapackSolver::solve(void)
  {
    if(!theSOE)
      {
	std::cerr << "WARNING XC::BandSPDLinLapackSolver::solve(void)- ";
	std::cerr << " No XC::LinearSOE object has been set\n";
	return -1;
      }

    int n = theSOE->size;
    int kd = theSOE->half_band -1;
    int ldA = kd +1;
    int nrhs = 1;
    int ldB = n;
    int info;
    double *Aptr = theSOE->A.getDataPtr();
    double *Xptr = theSOE->getPtrX();
    double *Bptr = theSOE->getPtrB();

    // first copy B into X
    for(int i=0; i<n; i++)
      *(Xptr++) = *(Bptr++);
    Xptr= theSOE->getPtrX();

    char strU[]= "U";
    // now solve AX = Y
    { if (theSOE->factored == false)          
	dpbsv_(strU,&n,&kd,&nrhs,Aptr,&ldA,Xptr,&ldB,&info);
      else
	dpbtrs_(strU,&n,&kd,&nrhs,Aptr,&ldA,Xptr,&ldB,&info);
    }

    // check if successfull
    if(info != 0)
      {
	std::cerr << "WARNING XC::BandSPDLinLapackSolver::solve() - the LAPACK";
	std::cerr << " routines returned " << info << std::endl;
	return -info;
      }
    theSOE->factored = true;
    return 0;
  }
    


int XC::BandSPDLinLapackSolver::setSize()
  {
    // nothing to do    
    return 0;
  }

int XC::BandSPDLinLapackSolver::sendSelf(CommParameters &cp)
  {
    // nothing to do
    return 0;
  }

int XC::BandSPDLinLapackSolver::recvSelf(const CommParameters &cp)
  {
    // nothing to do
    return 0;
  }



