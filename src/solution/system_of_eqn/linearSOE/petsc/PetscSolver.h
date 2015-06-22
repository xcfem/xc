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
// $Date: 2006/01/13 00:02:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/PetscSolver.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// PetscSolver. It solves the FullGenLinSOE object by calling
// Lapack routines.
//
// What: "@(#) PetscSolver.h, revA"

#ifndef PetscSolver_h
#define PetscSolver_h

#include <petscksp.h>
#include "solution/system_of_eqn/linearSOE/LinearSOESolver.h"

namespace XC {
class PetscSOE;

//! @ingroup Solver
//
//! @brief Objeto encargado de la solución del
//! sistema de ecuaciones basado en la biblioteca
//! PETSC (http://www.mcs.anl.gov/petsc/petsc-as).
class PetscSolver : public LinearSOESolver
  {
  private:
    KSP ksp;                   
    PC pc;
    int its;
    std::string method;
    std::string preconditioner;
    double rTol;
    double aTol;
    double dTol; 
    int maxIts;

    static int numSolver;
  protected:
    PetscSOE *theSOE;

    PetscSolver();    
    PetscSolver(KSPType mth, PCType precond);    
    PetscSolver(KSPType mth, PCType precond, double rTol, double aTol, double dTol, int maxIts);    
    virtual LinearSOESolver *getCopy(void) const;
    bool setLinearSOE(LinearSOE *theSOE);
  public:
    ~PetscSolver(void);

    int solve(void);
    int setSize(void);
    virtual bool setLinearSOE(PetscSOE &theSOE);
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);    

    friend class ActorPetscSOE;
    friend class ShadowPetscSOE;
  };

inline LinearSOESolver *PetscSolver::getCopy(void) const
   { return new PetscSolver(*this); }
} // fin namespace XC

#endif

