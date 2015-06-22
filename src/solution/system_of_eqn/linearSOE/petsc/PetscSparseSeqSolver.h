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
// $Date: 2005/05/18 19:26:59 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/PetscSparseSeqSolver.h,v $
                                                                        
// Written: fmk 
// Created: 04/05

//
// Description: This file contains the class definition for 
// PetscSparseSeqSolver. It solves the SparseGenLinSOE object by calling Petsc routines
//
// What: "@(#) PetscSparseSeqSolver.h, revA"

#ifndef PetscSparseSeqSolver_h
#define PetscSparseSeqSolver_h

#include <petscksp.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSolver.h>

#define PetscMinRelTol 1.0e-6

namespace XC {
class SparseGenRowLinSOE;

//! @ingroup Solver
//
//! @brief Objeto encargado de la solución de
//! sistema de ecuaciones con matriz dispersa
//! basado en la biblioteca
//! PETSC (http://www.mcs.anl.gov/petsc/petsc-as).
class PetscSparseSeqSolver: public SparseGenRowLinSolver
  {
  private:
    KSP ksp;                      // linear solver context
    PC  pc;
    int its;
    KSPType method;
    PCType  preconditioner;
    double rTol;
    double aTol;
    double dTol; 
    int maxIts;

    Mat A;
    Vec x, b;
  protected:
    SparseGenRowLinSOE *theSOE;

    PetscSparseSeqSolver(KSPType method, PCType preconditioner);    
    PetscSparseSeqSolver(KSPType method, PCType preconditioner, double rTol, double aTol, double dTol, int maxIts);    
    virtual LinearSOESolver *getCopy(void) const;
    virtual bool setLinearSOE(LinearSOE *theSOE);
  public:
    ~PetscSparseSeqSolver();

    int solve(void);
    int setSize(void);
    int getNumIterations(void);
    virtual bool setLinearSOE(SparseGenRowLinSOE &theSOE);
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline LinearSOESolver *PetscSparseSeqSolver::getCopy(void) const
   { return new PetscSparseSeqSolver(*this); }
} // fin namespace XC

#endif

