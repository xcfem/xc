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
** file 'COPYRIGHT' in main directory for information on usage and    **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.6 $
// $Date: 2005/04/01 19:56:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/SuperLU.cpp,v $
                                                                        
                                                                        
//
// Written: fmk 
// Created: Tue 11/96
// Revision: A
//
// Description: This file contains the implementation of XC::SuperLU
//
// What: "@(#) SuperLU.h, revA"

#include <solution/system_of_eqn/linearSOE/sparseGEN/SuperLU.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.h>
#include <cmath>


void XC::SuperLU::free_matricesLU(void)
  {
    if(L.ncol!=0)
      {
        Destroy_SuperNode_Matrix(&L);
        L.ncol= 0;
      }
    if(U.ncol!=0)
      {
        Destroy_CompCol_Matrix(&U);
        U.ncol= 0;
      }
  }

void XC::SuperLU::free_matricesABAC(void)
  {
    if(AC.ncol != 0)
      {
        NCPformat *ACstore = static_cast<NCPformat *>(AC.Store);
        if(ACstore)
          {
            SUPERLU_FREE(ACstore->colbeg);
            SUPERLU_FREE(ACstore->colend);
            SUPERLU_FREE(ACstore);
            AC.ncol= 0;
          }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; ERROR when releasing AC matrix."
		    << std::endl;
      }
    if(A.ncol != 0)
      {
        SUPERLU_FREE(A.Store);
        A.ncol= 0;
      }
    if(B.ncol != 0)
      {
        SUPERLU_FREE(B.Store);
        B.ncol= 0;
      }
  }

void XC::SuperLU::free_matrices(void)
  {
    free_matricesLU();
    free_matricesABAC();
  }

void XC::SuperLU::free_mem(void)
  {
    free_matrices();
  }

void XC::SuperLU::inic_permutation_vectors(const size_t &n)
  {
    // create space for the permutation vectors 
    // and the elimination tree
    const size_t sizePerm= perm_r.Size();
    if(sizePerm<n)
      {
        perm_r.resize(n);		
	perm_c.resize(n);		
	etree.resize(n);		
        for(size_t k= 0;k<n;k++)
          {
            perm_c(k)= k;
            perm_r(k)= k;
            etree(k)= 0;
          }
      }

  }

void XC::SuperLU::alloc_matrices(const size_t &n)
  {
    free_matrices();
    // create the SuperMatrix A	
    dCreate_CompCol_Matrix(&A, n, n, theSOE->nnz, theSOE->A.getDataPtr(), theSOE->rowA.getDataPtr(), theSOE->colStartA.getDataPtr(), SLU_NC, SLU_D, SLU_GE);

    // obtain and apply column permutation to give SuperMatrix AC
    get_perm_c(permSpec, &A, perm_c.getDataPtr());

    sp_preorder(&options, &A, perm_c.getDataPtr(), etree.getDataPtr(), &AC);

    // create the rhs SuperMatrix B 
    dCreate_Dense_Matrix(&B, n, 1, theSOE->getPtrX(), n, SLU_DN, SLU_D, SLU_GE);
  }
void XC::SuperLU::alloc(const size_t &n)
  {
    if(n>0)
      {
        inic_permutation_vectors(n);
        alloc_matrices(n);
      }
  }

//! @brief Copy constructor.
XC::SuperLU::SuperLU(const SuperLU &other)
  : SparseGenColLinSolver(other)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; ERROR copy constructor not implemented."
              << std::endl;
  }

//! @brief Assignment operator.
XC::SuperLU &XC::SuperLU::operator=(const SuperLU &other)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; ERROR assignment operator not implemented."
              << std::endl;
    return *this;
  }

//! @brief Virtual constructor.
XC::LinearSOESolver *XC::SuperLU::getCopy(void) const
   { return new SuperLU(*this); }

//! @brief Constructor.
//!
//! Saves the values for the arguments
//! \p permSpec, \p panelSize, \p relax and \p thresh that
//! will be used when calling the SuperLU routines in solve() and
//! setSize().
//!
//! \p permSpec defines the ordering routine used in defining the
//! column permutations \p permC: \f$0\f$ uses the original ordering
//! supplied, \f$1\f$ defines a min-degree ordering based on \f$A^TA\f$
//! and \f$2\f$ a min-degree ordering based on \f$A^T + A\f$. \p relax defines
//! the min number of columns in a subtree for the subtree to be considered a
//! single supernode. \p thresh defines the pivoting threshold: at
//! step j of the Gaussian elimination if (abs\f$(A_{jj}) \ge\f$ \p thresh
//! (max\f$ i \ge j\f$ abs(\f$A_{ij}\f$)). A value for \p thresh of \f$0.0\f$
//! definines no pivoting, a value of \f$1.0\f$ classical partial pivoting.
//! \p panelSize defines the number of consecutive columns used as a
//! panel in the elimination. For more information on these values see the
//! SuperLU manual.
XC::SuperLU::SuperLU(int perm, double drop_tolerance, int panel, int relx, char symm)
  :SparseGenColLinSolver(SOLVER_TAGS_SuperLU), relax(relx), permSpec(perm), panelSize(panel), drop_tol(drop_tolerance), symmetric(symm)
  {
    // set_default_options(&options);
    options.Fact = DOFACT;
    options.Equil = YES;
    options.ColPerm = COLAMD;
    options.DiagPivotThresh = 1.0;
    options.Trans = NOTRANS;
    options.IterRefine = NOREFINE;
    options.SymmetricMode = NO;
    options.PivotGrowth = NO;
    options.ConditionNumber = NO;
    options.PrintStat = YES;

    if(symmetric == 'Y')
      options.SymmetricMode = YES;
    L.ncol= 0;
    U.ncol= 0;
    A.ncol= 0;
    B.ncol= 0;
    AC.ncol= 0; //Added by LCPT.
  }


//! @brief Destructor.
XC::SuperLU::~SuperLU(void)
  { free_mem(); }

//! @brief Compute the LU factorization of A.
int XC::SuperLU::factorize(void)
  {
    int retval= 0;
    if(theSOE->factored == false)
      {
        // factor the matrix
        free_matricesLU();
        int info= 0;
        SuperLUStat_t slu_stat;
	GlobalLU_t global_lu_t; //Ubuntu 18 
        StatInit(&slu_stat);


        //Prior to Ubuntu 16: dgstrf(&options, &AC, drop_tol, relax, &panelSize,etree.getDataPtr(), 0, perm_c.getDataPtr(), perm_r.getDataPtr(), &L, &U, &slu_stat, &info);
	//it seems that argument 'drop_tol' is deprecated (or it was an error?)
        //Prior to Ubuntu 18: dgstrf(&options, &AC, relax, panelSize,etree.getDataPtr(), nullptr, 0, perm_c.getDataPtr(), perm_r.getDataPtr(), &L, &U, &slu_stat, &info);
        dgstrf(&options, &AC, relax, panelSize,etree.getDataPtr(), nullptr, 0, perm_c.getDataPtr(), perm_r.getDataPtr(), &L, &U, &global_lu_t, &slu_stat, &info);	
        if(info != 0)
          {        
             std::cerr << getClassName() << "::" << __FUNCTION__
		       << "; WARNING - error " << info
		       << " returned in factorization dgstrf()\n";
             retval= -info;
          }
        StatFree(&slu_stat);

        if(symmetric == 'Y')
          options.Fact= SamePattern_SameRowPerm;
        else
          options.Fact = SamePattern;
        theSOE->factored = true;
      }
    return retval;
  }

//! @brief Solve the system.
//!
//! First copies \f$B\f$ into \f$X\f$ and then solves the FullGenLinSOE system 
//! it is associated with (pointer kept by parent class) by calling the SeuperLU
//! routine dgstrf(), if the system is marked as not having been factored,
//! or dgstrs(), if system is marked as having been factored. If the
//! solution is successfully obtained, i.e. the SuperLU routines return \f$0\f$
//! in the INFO argument, it marks the system has having been
//! factored and returns \f$0\f$, otherwise it prints a warning message and
//! returns INFO. The solve process changes \f$A\f$ and \f$X\f$ and sets the
//! char \p rafact to \p Y.
int XC::SuperLU::solve(void)
  {
    int retval= 0;
    if(!theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - no LinearSOE object has been set\n";
        retval= -1;
      }
    else
      {
        size_t n = theSOE->size;
    
        // check for quick return
        if(n>0)
          {
            const size_t sizePerm= perm_r.Size();
            if(sizePerm != n)
              {
	        std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; WARNING - size for row and col permutations"
		          << " are 0 - has setSize() been called?\n";
	        retval= -1;
              }
            else
              {
                // first copy B into X
                double *Xptr= theSOE->getPtrX();
                double *Bptr= theSOE->getPtrB();
                for(size_t i=0; i<n; i++)
                  *(Xptr++)= *(Bptr++);

                const int ok= factorize();
                if(ok==0)
                  {
                    // do forward and backward substitution
                    trans_t trans= NOTRANS;
                    int info= 0;
                    SuperLUStat_t slu_stat;
                    StatInit(&slu_stat);
                    dgstrs(trans, &L, &U, perm_c.getDataPtr(), perm_r.getDataPtr(), &B, &slu_stat, &info);    
                    if(info != 0)
                      {        
                        std::cerr << getClassName() << "::" << __FUNCTION__
				  << "; WARNING - "
				  << " error " << info << " returned in substitution dgstrs()\n";
                        retval= -info;
                      }
                    else
                      StatFree(&slu_stat);
                  }
                else
                  retval= ok;
              }
          }
      }
    return retval;
  }


//! @brief Set the system size.
//! 
//! Obtains the size of the system from it's associaed SparseGenColLinSOE
//! object. With this information it creates space for the integer arrays
//! \p permR, \p permC and \p etree. It then creates the
//! a SuperMatrix for A by calling the SuperLU routine {\em
//! dCreate\_CompCol\_Matrix()}, sets the column permutation \p permR
//! by calling the SuperLU routine {\em get\_perm\_c(permSpec, A, permC)},
//! applies this permutation and determines the elimination tree {\em
//! etree} by calling the SuperLU routine sp\_preorder(). It then
//! creates a SuperMatrix for X by calling the SuperLU routine 
//! dCreate\_Dense\_Matrix().
//! Returns \f$0\f$ if successful, prints a warning message and returns
//! a \f$-1\f$ if not enough memory is available for the arrays.
int XC::SuperLU::setSize(void)
  {
    const size_t n = theSOE->size;
    if(n>0)
      {
        const size_t sizePerm= perm_r.Size();
        if((sizePerm>0) && (sizePerm<n))
          std::clog << getClassName() << "::" << __FUNCTION__
		    << "SuperLU, sometimes, fails when dimension"
	            << " of the system is changed." << std::endl;
        alloc(n);
        
        // set the refact variable to 'N' after first factorization with new_ size 
        // can set to 'Y'.
        //options.Fact = DOFACT;
        set_default_options(&options);

        if(symmetric == 'Y') options.SymmetricMode=YES;
      }
    else
      if(n == 0)
        return 0;
      else
        {
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "WARNING; order of system <  0\n";
          return -1;        
        }
    return 0;
  }

int XC::SuperLU::sendSelf(CommParameters &cp)
  {
    // nothing to do
    return 0;
  }

int XC::SuperLU::recvSelf(const CommParameters &cp)
  {
    // nothing to do
    return 0;
  }

void XC::SuperLU::Print(std::ostream &os) const
  {
    os << "A.ncol= " << A.ncol << std::endl;
    os << "A.nrow= " << A.nrow << std::endl;
    os << "B.ncol= " << B.ncol << std::endl;
    os << "B.nrow= " << B.nrow << std::endl;
    os << "AC.ncol= " << AC.ncol << std::endl;
    os << "AC.nrow= " << AC.nrow << std::endl;
    os << "perm_r= " << perm_r << std::endl;
    os << "perm_c= " << perm_c << std::endl;
    os << "etree= " << etree << std::endl;
  }















