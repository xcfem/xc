//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
                                                                        
// $Revision: 1.5 $
// $Date: 2006/01/13 00:02:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/PetscSolver.cpp,v $
                                                                        
// Written: fmk & om
// Created: 7/98
// Revision: A
//
// Description: This file contains the class implementation for 
// FullGenLinLapackSolver. It solves the XC::FullGenLinSOE object by calling
// Lapack routines.
//
// What: "@(#) FullGenLinLapackSolver.h, revA"

#include <solution/system_of_eqn/linearSOE/petsc/PetscSolver.h>
#include <solution/system_of_eqn/linearSOE/petsc/PetscSOE.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/Timer.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>

int XC::PetscSolver::numSolver=0;

XC::PetscSolver::PetscSolver()
  :LinearSOESolver(SOLVER_TAGS_PetscSolver), 
   rTol(PETSC_DEFAULT), aTol(PETSC_DEFAULT), dTol(PETSC_DEFAULT), maxIts(PETSC_DEFAULT)
  {
    numSolver++;
  }

XC::PetscSolver::PetscSolver(KSPType meth, PCType pre)
  :LinearSOESolver(SOLVER_TAGS_PetscSolver), method(meth), preconditioner(pre),
   rTol(PETSC_DEFAULT), aTol(PETSC_DEFAULT), dTol(PETSC_DEFAULT), maxIts(PETSC_DEFAULT)
  {
    numSolver++;
  }

XC::PetscSolver::PetscSolver(KSPType meth, PCType pre, double relTol, double absTol, double divTol, int maxIterations)
  :LinearSOESolver(SOLVER_TAGS_PetscSolver), method(meth), preconditioner(pre),
   rTol(relTol), aTol(absTol), dTol(divTol), maxIts(maxIterations)
  {
    numSolver++;
  }

XC::PetscSolver::~PetscSolver(void)
  {
#if (PETSC_VERSION_MAJOR == 3) && (PETSC_VERSION_MINOR >= 2)
    KSPDestroy( &ksp );
#else
    KSPDestroy( ksp );
#endif
    numSolver--;
    /*
    if(numSolver == 0)
      PetscFinalize();
    */
  }



int XC::PetscSolver::solve(void)
  {
    int size = theSOE->size;
    
    int numProcesses = theSOE->numProcesses;
    int processID = theSOE->processID;
    
    int ierr;
    ierr = MatAssemblyBegin(theSOE->A, MAT_FINAL_ASSEMBLY); CHKERRQ(ierr); 
    ierr = MatAssemblyEnd(theSOE->A, MAT_FINAL_ASSEMBLY); CHKERRQ(ierr); 

    //
    // if parallel, we must zero X & form the total B: each processor has own contributions
    //
    
    static Vector recvVector(1);

    if(numProcesses > 1)
      {
        Vector &vectX= theSOE->getX();
        Vector &vectB= theSOE->getB();

        // zero X
        vectX.Zero();

        //
        // form B on each
        //

        ChannelQueue &theChannels= theSOE->getChannels();
        const size_t numChannels= theChannels.size();
        if(processID != 0)
          {
            CommParameters cp(0,*theChannels[0]);
            cp.sendVector(vectB,getDbTagData(),CommMetaData(0));
            cp.receiveVector(vectB,getDbTagData(),CommMetaData(0));
          }
        else
          {
            if(recvVector.Size() != size) 
              recvVector.resize(size);
            for(size_t j=0; j<numChannels; j++)
              {
                CommParameters cp(0,*theChannels[j]);
                cp.receiveVector(recvVector,getDbTagData(),CommMetaData(1));
                vectB += recvVector;
              }
            for(size_t j=0; j<numChannels; j++)
              {
                CommParameters cp(0,*theChannels[0]);
                cp.sendVector(vectB,getDbTagData(),CommMetaData(0));
              }
          }
      }

    //
    // solve and mark as having been solved
    //

    ierr = KSPSolve(ksp, theSOE->b, theSOE->x); CHKERRQ(ierr); 
    theSOE->factored = 1;

    //
    // if parallel, we must form the total X: each processor has startRow through endRow-1
    //

    if(numProcesses > 1)
      {
        Vector &vectX= theSOE->getX();

        ChannelQueue &theChannels= theSOE->getChannels();
        const size_t numChannels= theChannels.size();
      
        if(processID != 0)
          {
            CommParameters cp(0,*theChannels[0]);
            cp.sendVector(vectX,getDbTagData(),CommMetaData(2));
            cp.receiveVector(vectX,getDbTagData(),CommMetaData(2));
          }
        else
          {
            if(recvVector.Size() != size) 
              recvVector.resize(size);

            for(size_t j=0; j<numChannels; j++)
              {
                CommParameters cp(0,*theChannels[j]);
                cp.receiveVector(recvVector,getDbTagData(),CommMetaData(3));
                vectX += recvVector;
              }
            for(size_t j=0; j<numChannels; j++)
              {
                Channel *theChannel = theChannels[j];
                theChannel->sendVector(0, 0, vectX);
              }
          }
      }
    return ierr;
  } 


int XC::PetscSolver::setSize()
  {
    /* 
     * Create linear solver context
     */
  
    KSPCreate(PETSC_COMM_WORLD, &ksp);

     /* 
      *  Set operators. NOTE: matrix that defines the linear system
      *  also serves as the preconditioning matrix.
      */

    KSPSetOperators(ksp, theSOE->A, theSOE->A);//, DIFFERENT_NONZERO_PATTERN);

   /* 
    *  Set solution scheme & tolerances
    */
   
   int ierr;
   ierr = KSPSetType(ksp, method.c_str()); CHKERRQ(ierr); 
   ierr = KSPSetTolerances(ksp, rTol, aTol, dTol, maxIts); 

   /* 
    *  Set preconditioning scheme
    */

   KSPGetPC(ksp, &pc);
   ierr = PCSetType(pc,  preconditioner.c_str()); CHKERRQ(ierr); 

   /* 
    *  Finally mark so that uses last solution as initial guess in each solution
    *    NOTE: maybe change this as another user supplied option
    */

   //   ierr = KSPSetInitialGuessNonzero(ksp, PETSC_TRUE); CHKERRQ(ierr); 
   return ierr;
}

//! @brief Asigna el sistema de ecuaciones a resolver.
bool XC::PetscSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    PetscSOE *tmp= dynamic_cast<PetscSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << nombre_clase() << "::setLinearSOE: el sistema de ecuaciones no es del tipo adecuado para este solver." << std::endl;
    return retval;
  }

//! @brief Asigna el sistema de ecuaciones a resolver.
bool XC::PetscSolver::setLinearSOE(PetscSOE &theSys)
  { return setLinearSOE(&theSys); }


int XC::PetscSolver::sendSelf(CommParameters &cp)
  {
    static ID idData(3);
    idData(0) = maxIts;
    if(method==KSPCG) 
      idData(1) = 0;
    else if(method==KSPBICG)
      idData(1) = 1;
    else if(method==KSPRICHARDSON)
      idData(1) = 2;
    else if(method==KSPCHEBYSHEV)
      idData(1) = 3;
    else if(method==KSPGMRES) 
      idData(1) = 4;
    else
      {
        std::cerr << "PetscSolver::sendSelf() - unknown method set\n";
        return -1;
      }

    if(preconditioner==PCJACOBI)
      idData(2) = 0;
    else if(preconditioner==PCILU)
      idData(2) = 1;
    else if(preconditioner==PCICC)
      idData(2) = 2;
    else if(preconditioner==PCBJACOBI)
      idData(2) = 3;
    else if(preconditioner==PCNONE)
      idData(2) = 4;
    else
      {
        std::cerr << "PetscSolver::sendSelf() - unknown preconditioner set\n";
        return -1;
       }

    cp.sendID(idData,getDbTagData(),CommMetaData(0));
    cp.sendDoubles(rTol,aTol,dTol,getDbTagData(),CommMetaData(1));
    return 0;
  }

int XC::PetscSolver::recvSelf(const CommParameters &cp)
  {
    static ID idData(3);
    cp.receiveID(idData,getDbTagData(),CommMetaData(0));
    maxIts = idData(0);
    if(idData(1) == 0) 
      method= KSPCG;
    else if(idData(1) == 1)
      method= KSPBICG;
    else if(idData(1) == 2) 
      method= KSPRICHARDSON;
    else if(idData(1) == 3)
      method= KSPCHEBYSHEV;
    else if(idData(1) == 4)
      method= KSPGMRES;
    else
      {
        std::cerr << "PetscSolver::recvSelf() - unknown method recvd\n";
        return -1;
      }

    if(idData(2) == 0)
      preconditioner= PCJACOBI;
    else if(idData(2) == 1)
      preconditioner= PCILU;
    else if(idData(2) == 2)
      preconditioner= PCICC;
    else if(idData(2) == 3)
      preconditioner= PCBJACOBI;
    else if(idData(2) == 4)
      preconditioner= PCNONE;
    else
      {
        std::cerr << "PetscSolver::sendSelf() - unknown preconditioner set\n";
        return -1;
      }
    cp.receiveDoubles(rTol,aTol,dTol,getDbTagData(),CommMetaData(1));
    return 0;
  }



