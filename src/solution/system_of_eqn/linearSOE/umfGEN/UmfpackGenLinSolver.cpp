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
// $Date: 2003/04/02 22:02:53 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.C
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for 
// UmfpackGenLinSolver. It solves the UmfpackGenLinSOEobject by calling
// UMFPACK2.2.1 routines.
//
// What: "@(#) UmfpackGenLinSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.h>
#include <f2c.h>

extern "C" int umd21i_(int *keep, double *cntl, int *icntl);


XC::UmfpackGenLinSolver::UmfpackGenLinSolver()
:LinearSOESolver(SOLVER_TAGS_UmfpackGenLinSolver),
 copyIndex(0), lIndex(0), work(0), theSOE(nullptr)
  {
    // perform the initialisation needed in UMFpack
    umd21i_(keep, cntl, icntl);
  }


extern "C" int umd2fa_(int *n, int *ne, int *job, logical *transa,
		       int *lvalue, int *lindex, double *value,
		       int *index, int *keep, double *cntl, int *icntl,
		       int *info, double *rinfo);

extern "C" int umd2so_(int *n, int *job, logical *transa,
		       int *lvalue, int *lindex, double *value,
		       int *index, int *keep, double *b, double *x, 
		       double *w, double *cntl, int *icntl,
		       int *info, double *rinfo);

int XC::UmfpackGenLinSolver::solve(void)
  {
    if(theSOE == 0)
      {
	std::cerr << "WARNING XC::UmfpackGenLinSolver::solve(void)- ";
	std::cerr << " No XC::LinearSOE object has been set\n";
	return -1;
      }
    
    int n = theSOE->size;
    int ne = theSOE->nnz;
    int lValue = theSOE->lValue;

    // check for XC::quick return
    if (n == 0)
	return 0;

    // first copy B into X
    double *Xptr = theSOE->getPtrX();
    double *Bptr = theSOE->getPtrB();
    double *Aptr = theSOE->A.getDataPtr();

    int job =0; // set to 1 if wish to do iterative refinment
    logical trans = FALSE_;

    if(theSOE->factored == false)
      {
        // make a copy of index
        for(int i=0; i<2*ne; i++)
          { copyIndex[i] = theSOE->index[i]; }

      // factor the matrix
      umd2fa_(&n, &ne, &job, &trans, &lValue, &lIndex, Aptr,
	      copyIndex.getDataPtr(), keep, cntl, icntl, info, rinfo);
      
      if (info[0] != 0) {	
	std::cerr << "WARNING UmfpackGenLinSolver::solve(void)- ";
	std::cerr << info[0] << " returned in factorization UMD2FA()\n";
	return -info[0];
      }
      theSOE->factored = true;
    }	

    // do forward and backward substitution
    umd2so_(&n, &job, &trans, &lValue, &lIndex, Aptr, copyIndex.getDataPtr(), 
	    keep, Bptr, Xptr, work.getDataPtr(), cntl, icntl, info, rinfo);

    if (info[0] != 0) {	
       std::cerr << "WARNING UmfpackGenLinSolver::solve(void)- ";
       std::cerr << info[0] << " returned in substitution dgstrs()\n";
       return -info[0];
    }

    return 0;
}


int XC::UmfpackGenLinSolver::setSize()
  {
    int n = theSOE->size;
    int ne = theSOE->nnz;
    if(n > 0)
      {
        work.resize(4*n);

        lIndex = 37*n + 4*ne + 10;
        copyIndex= ID(lIndex);
      }	
    return 0;
  }

//! @brief Asigna el sistema de ecuaciones a resolver.
bool XC::UmfpackGenLinSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    UmfpackGenLinSOE *tmp= dynamic_cast<UmfpackGenLinSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << nombre_clase() << "::setLinearSOE: el sistema de ecuaciones no es del tipo adecuado para este solver." << std::endl;
    return retval;
  }

bool XC::UmfpackGenLinSolver::setLinearSOE(UmfpackGenLinSOE &theLinearSOE)
  { return setLinearSOE(&theLinearSOE); }

int XC::UmfpackGenLinSolver::sendSelf(CommParameters &cp)
  {
    // nothing to do
    return 0;
  }

int XC::UmfpackGenLinSolver::recvSelf(const CommParameters &cp)
  {
    // nothing to do
    return 0;
  }

