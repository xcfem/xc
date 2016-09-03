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











