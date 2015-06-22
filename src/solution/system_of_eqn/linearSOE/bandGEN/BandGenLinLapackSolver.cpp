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
// $Date: 2003/04/02 22:02:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandGEN/BandGenLinLapackSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/bandGEN/BandGenLinLapackSolver.h
//
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// BandGenLinLapackSolver. It solves the XC::BandGenLinSOE object by calling
// Lapack routines.
//
// What: "@(#) BandGenLinLapackSolver.h, revA"

#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinLapackSolver.h>
#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.h>

XC::BandGenLinLapackSolver::BandGenLinLapackSolver()
  :BandGenLinSolver(SOLVER_TAGS_BandGenLinLapackSolver) {}


extern "C" int dgbsv_(int *N, int *KL, int *KU, int *NRHS, double *A, 
		      int *LDA, int *iPiv, double *B, int *LDB, int *INFO);
		      

extern "C" int dgbtrs_(char *TRANS, int *N, int *KL, int *KU, int *NRHS, 
		       double *A, int *LDA, int *iPiv, double *B, int *LDB, 
		       int *INFO);
int XC::BandGenLinLapackSolver::solve(void)
  {
    if(theSOE == 0)
      {
	std::cerr << "WARNING XC::BandGenLinLapackSolver::solve(void)- ";
	std::cerr << " No XC::LinearSOE object has been set\n";
	return -1;
      }

    int n = theSOE->size;    
    // check iPiv is large enough
    if(iPiv.Size() < n)
      {
	std::cerr << "WARNING XC::BandGenLinLapackSolver::solve(void)- ";
	std::cerr << " iPiv not large enough - has setSize() been called?\n";
	return -1;
      }

    int kl = theSOE->numSubD;
    int ku = theSOE->numSuperD;
    int ldA = 2*kl + ku +1;
    int nrhs = 1;
    int ldB = n;
    int info;
    double *Aptr = theSOE->A.getDataPtr();
    double *Xptr = theSOE->getPtrX();
    double *Bptr = theSOE->getPtrB();
    int    *iPIV = iPiv.getDataPtr();
    
    // first copy B into X
    for(int i=0; i<n; i++)
      {	*(Xptr++) = *(Bptr++); }
    Xptr = theSOE->getPtrX();

    // now solve AX = B

    {
      if(theSOE->factored == false) // factor and solve 	
        dgbsv_(&n,&kl,&ku,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);
      else // solve only using factored matrix
        {
          char ene[]= "N";
          dgbtrs_(ene,&n,&kl,&ku,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);
        }
    }

    // check if successfull
    if(info != 0)
      {
	std::cerr << "WARNING XC::BandGenLinLapackSolver::solve() -";
	std::cerr << "LAPACK routine returned " << info << std::endl;
	return -info;
      }

    theSOE->factored = true;
    return 0;
  }
    


int XC::BandGenLinLapackSolver::setSize()
  {
    // if iPiv not big enough, free it and get one large enough
    if(iPiv.Size() < theSOE->size)
      {
        iPiv.resize(theSOE->size);
      }
    return 0;
  }

int XC::BandGenLinLapackSolver::sendSelf(CommParameters &cp)
  { return 0; }

int XC::BandGenLinLapackSolver::recvSelf(const CommParameters &cp)
  {
    // nothing to do
    return 0;
  }
