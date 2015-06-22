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
// File: ~/system_of_eqn/linearSOE/symLinSolver/SymSparseLinSolver.C
//
// Written: Jun Peng  (junpeng@stanford.edu)
//          Prof. Kincho H. Law
//          Stanford University
// Created: 12/98
// Revision: A
//
// Description: This file contains the class definition for 
// SymSparseinSolver. It solves the XC::SymSparseLinSOE object by calling
// some "C" functions. The solver used here is generalized sparse
// solver. The user can choose three different ordering schema.
//
// What: "@(#) SymSparseLinSolver.C, revA"


#include "solution/system_of_eqn/linearSOE/sparseSYM/SymSparseLinSOE.h"
#include "solution/system_of_eqn/linearSOE/sparseSYM/SymSparseLinSolver.h"

extern "C" {
  #include "solution/system_of_eqn/linearSOE/sparseSYM/nmat.h"
  #include "solution/system_of_eqn/linearSOE/sparseSYM/FeStructs.h"
}



XC::SymSparseLinSolver::SymSparseLinSolver()
:LinearSOESolver(SOLVER_TAGS_SymSparseLinSolver),
 theSOE(0)
{
    // nothing to do.
}


int XC::SymSparseLinSolver::solve(void)
{ 
    if (theSOE == 0) {
	std::cerr << "WARNING XC::SymSparseLinSolver::solve(void)- ";
	std::cerr << " No XC::LinearSOE object has been set\n";
	return -1;
    }

    int      nblks = theSOE->nblks;
    int      *xblk = theSOE->xblk;
    int      *invp = theSOE->invp;
    double   *diag = theSOE->diag;
    double   **penv = theSOE->penv;
    int      *rowblks = theSOE->rowblks;
    OFFDBLK  **begblk = theSOE->begblk;
    OFFDBLK  *first = theSOE->first;

    int neq = theSOE->size;

    // check for XC::quick return
    if (neq == 0)
	return 0;

    // first copy B into X

    for(int i=0; i<neq; i++)
      theSOE->getX(i)= theSOE->getB(i);
    double *Xptr = theSOE->getPtrX();

    if(theSOE->factored == false)
      {
        //factor the matrix
        //call the "C" function to do the numerical factorization.
        int factor;
	factor = pfsfct(neq, diag, penv, nblks, xblk, begblk, first, rowblks);
	if(factor > 0)
          {
	    std::cerr << "In XC::SymSparseLinSolver: error in factorization.\n";
	    return -1;
	  }
	theSOE->factored = true;
      }

    // do forward and backward substitution.
    // call the "C" function.

    pfsslv(neq, diag, penv, nblks, xblk, Xptr, begblk);

    // Since the X we get by solving AX=B is P*X, we need to reordering
    // the Xptr to ge the wanted X.

    std::vector<double> tempX(neq);

    for(int m=0; m<neq; m++)
      { tempX[m] = Xptr[invp[m]]; }
	
    for(int k=0; k<neq; k++)
      { Xptr[k] = tempX[k]; }
    return 0;
  }


int XC::SymSparseLinSolver::setSize(void)
  {
    // nothing to do
    return 0;
  }

//! @brief Asigna el sistema de ecuaciones a resolver.
bool XC::SymSparseLinSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    SymSparseLinSOE *tmp= dynamic_cast<SymSparseLinSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << nombre_clase() << "::setLinearSOE: el sistema de ecuaciones no es del tipo adecuado para este solver." << std::endl;
    return retval;
  }

bool XC::SymSparseLinSolver::setLinearSOE(SymSparseLinSOE &theLinearSOE)
  { return setLinearSOE(&theLinearSOE); }


int XC::SymSparseLinSolver::sendSelf(CommParameters &cp)
  {
    // doing nothing
    return 0;
  }


int XC::SymSparseLinSolver::recvSelf(const CommParameters &cp)
  {
    // nothing to do
    return 0;
  }




