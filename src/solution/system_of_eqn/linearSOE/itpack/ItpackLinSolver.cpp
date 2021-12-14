//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:02:02 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/itpack/ItpackLinSolver.cpp,v $
                                                                        
// Written: MHS
// Created: Sept 2001
//
// Description: This file contains the class definition for ItpackLinSolver.
// ItpackLinSolver is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.  Instances of XC::ItpackLinSolver 
// are used to solve a system of equations of type XC::ItpackLinSOE.

#include <solution/system_of_eqn/linearSOE/itpack/ItpackLinSolver.h>
#include <solution/system_of_eqn/linearSOE/itpack/ItpackLinSOE.h>
#include <utility/matrix/ID.h>

XC::ItpackLinSolver::ItpackLinSolver(int meth, int iter, double om)
  :LinearSOESolver(SOLVER_TAGS_Itpack),
   theSOE(0), n(0), nwksp(0), maxIter(iter),
   method(meth), omega(om)
  {}    

XC::ItpackLinSolver::ItpackLinSolver(void)
  :LinearSOESolver(SOLVER_TAGS_Itpack),
   theSOE(0), n(0), nwksp(0), maxIter(0),
   method(0), omega(0.0)
  {}    

bool XC::ItpackLinSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    ItpackLinSOE *tmp= dynamic_cast<ItpackLinSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << getClassName() << "::setLinearSOE: not a suitable system of equations" << std::endl;
    return retval;
  }

bool XC::ItpackLinSolver::setLinearSOE(ItpackLinSOE &theItpackSOE)
  { return setLinearSOE(&theItpackSOE); }

int XC::ItpackLinSolver::setSize(void)
  {
  // Get number of equations from SOE
  n = theSOE->size;

  if(n > 0)
    {
      iwksp.resize(3*n);
    }

  // Should be 2*maxIter for symmetric storage, 4*maxIter for nonsymmetric
  int ncg = 4*maxIter;
  
  // Order of the black subsystem
  int nb = theSOE->nnz; // I think this is the most it could be

  switch(method) {
  case ItpackJCG:
    nwksp = 4*n + ncg;
    break;
  case ItpackJSI: case ItpackJ:
    nwksp = 2*n;
    break;
  case ItpackSOR: case ItpackGS: case ItpackSORFixed:
    nwksp = n;
    break;
  case ItpackSSORCG:
    nwksp = 6*n + ncg;
    break;
  case ItpackSSORSI: case ItpackSSORFixed:
    nwksp = 5*n;
    break;
  case ItpackRSCG:
    nwksp = n + 3*nb + ncg;
    break;
  case ItpackRSSI: case ItpackRS:
    nwksp = n + nb;
    break;
  default:
    nwksp = 6*n + ncg;
    break;
  }

  if(nwksp > 0)
    {
      wksp.resize(nwksp);
    }

  // Get number of nonzeros from the SOE
  int nnz = theSOE->nnz;
  
  if(nnz > 0)
    {
      JA.resize(nnz);
    }

  int *jaPtr = theSOE->colA.getDataPtr();
  int i;
  // Add one for FORTRAN indexing
  for (i = 0; i < nnz; i++)
    JA[i] = jaPtr[i] + 1;

  if(n > 0)
    {
      IA.resize(n+1);
    }
  
  int *iaPtr = theSOE->rowStartA.getDataPtr();
  // Add one for FORTRAN indexing
  for (i = 0; i <= n; i++) 
    IA[i] = iaPtr[i] + 1;

  return 0;
  }

int XC::ItpackLinSolver::sendSelf(Communicator &comm)
  { return -1; }

int XC::ItpackLinSolver::recvSelf(const Communicator &comm)
  { return -1; }

extern "C" int dfault_(int *iparm, double *rparm);

extern "C" int vfill_(int *n, double *u, double *val);

extern "C" int jcg_(int *n, int *ia, int *ja, double *a, double *rhs,
		    double *u, int *iwksp, int *nw, double *wksp,
		    int *iparm, double *rparm, int *ier);

extern "C" int jsi_(int *n, int *ia, int *ja, double *a, double *rhs,
		    double *u, int *iwksp, int *nw, double *wksp,
		    int *iparm, double *rparm, int *ier);

extern "C" int sor_(int *n, int *ia, int *ja, double *a, double *rhs,
		    double *u, int *iwksp, int *nw, double *wksp,
		    int *iparm, double *rparm, int *ier);

extern "C" int ssorcg_(int *n, int *ia, int *ja, double *a, double *rhs,
		       double *u, int *iwksp, int *nw, double *wksp,
		       int *iparm, double *rparm, int *ier);

extern "C" int ssorsi_(int *n, int *ia, int *ja, double *a, double *rhs,
		       double *u, int *iwksp, int *nw, double *wksp,
		       int *iparm, double *rparm, int *ier);

extern "C" int rscg_(int *n, int *ia, int *ja, double *a, double *rhs,
		     double *u, int *iwksp, int *nw, double *wksp,
		     int *iparm, double *rparm, int *ier);

extern "C" int rssi_(int *n, int *ia, int *ja, double *a, double *rhs,
		     double *u, int *iwksp, int *nw, double *wksp,
		     int *iparm, double *rparm, int *ier);

int XC::ItpackLinSolver::solve(void)
  {
    // Let ITPACK fill in default parameter values
    dfault_(iparm, rparm);

    // Override defaults for "textbook" methods
    switch(method)
      {
      case ItpackJ:
        iparm[5] = 0; iparm[6] = 2; break;
      case ItpackGS:
        iparm[5] = 0; break;
      case ItpackSORFixed:
        iparm[5] = 0; rparm[4] = omega; break;
      case ItpackSSORFixed:
        iparm[5] = 0; rparm[4] = omega; break;
      case ItpackRS:
        iparm[5] = 0; break;
      default:
        break;
      }

  // Overwrite default max number of iterations
  iparm[0] = maxIter;

  // Sparse matrix storage scheme (0 = symmetric, 1 = nonsymmetric)
  iparm[4] = 1;

  double *aPtr = theSOE->A.getDataPtr();
  double *xPtr = theSOE->getPtrX();
  double *bPtr = theSOE->getPtrB();

  int *iaPtr = IA.getDataPtr();
  int *jaPtr = JA.getDataPtr();

  // Number of non-zero entries in matrix
  int nnz = iaPtr[n]-1;

  // Copy original ordering of column indices from the SOE
  // because ITPACK will reorder the sparse matrix representation
  if (theSOE->Aformed == false) {
    int *soeColA = theSOE->colA.getDataPtr();
    for (int i = 0; i < nnz; i++) {
      jaPtr[i] = soeColA[i] + 1;  // Add one for FORTRAN indexing
    }
  }

  int ier = 0;

  // Fill the x vector with zeros as initial guess to solution of Ax=b
  //double val = 0.0;
  //vfill_(&n, xPtr, &val);

  switch (method) {
  case ItpackJCG:
    jcg_(&n, iaPtr, jaPtr, aPtr, bPtr, xPtr,
	 iwksp.getDataPtr(), &nwksp, wksp.getDataPtr(), iparm, rparm, &ier);
    break;
  case ItpackJSI: case ItpackJ:
    jsi_(&n, iaPtr, jaPtr, aPtr, bPtr, xPtr,
	 iwksp.getDataPtr(), &nwksp, wksp.getDataPtr(), iparm, rparm, &ier);
    break;
  case ItpackSOR: case ItpackGS: case ItpackSORFixed:
    sor_(&n, iaPtr, jaPtr, aPtr, bPtr, xPtr,
	 iwksp.getDataPtr(), &nwksp, wksp.getDataPtr(), iparm, rparm, &ier);
    break;
  case ItpackSSORCG:
    ssorcg_(&n, iaPtr, jaPtr, aPtr, bPtr, xPtr,
	    iwksp.getDataPtr(), &nwksp, wksp.getDataPtr(), iparm, rparm, &ier);
    break;
  case ItpackSSORSI: case ItpackSSORFixed:
    ssorsi_(&n, iaPtr, jaPtr, aPtr, bPtr, xPtr,
	    iwksp.getDataPtr(), &nwksp, wksp.getDataPtr(), iparm, rparm, &ier);
    break;
  case ItpackRSCG:
    rscg_(&n, iaPtr, jaPtr, aPtr, bPtr, xPtr,
	  iwksp.getDataPtr(), &nwksp, wksp.getDataPtr(), iparm, rparm, &ier);
    break;
  case ItpackRSSI: case ItpackRS:
    rssi_(&n, iaPtr, jaPtr, aPtr, bPtr, xPtr,
	  iwksp.getDataPtr(), &nwksp, wksp.getDataPtr(), iparm, rparm, &ier);
    break;
  default:
    std::cerr << method  << " -- unknown method type %d"
	      << "XC::ItpackLinSolver::solve()" << std::endl;
    break;    
  }

  // Tell the SOE that solve() has been called
  theSOE->Aformed = true;

    if(ier > 0)
      {
        std::cerr << "XC::ItpackLinSolver::solve() -- returned ier = " << ier << std::endl;
        return -ier;
      }
    else
      return 0;
  }
