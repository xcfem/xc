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

// $Revision: 1.2 $
// $Date: 2003/02/14 23:02:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver.cpp,v $


// File: ~/system_of_eqn/linearSOE/ProfileSPD/ProfileSPDLinSubstrSolver.C
//
// Written: fmk
// Created: February 1997
// Revision: A
//
// Description: This file contains the implementation for XC::ProfileSPDLinSubstrSolver


#include "solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver.h"
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <cmath>

XC::ProfileSPDLinSubstrSolver::ProfileSPDLinSubstrSolver(double tol)
  :ProfileSPDLinDirectSolver(tol),
   DomainSolver(SOLVER_TAGS_ProfileSPDLinSubstrSolver), dSize(0) {}


int XC::ProfileSPDLinSubstrSolver::solve()
  { return this->XC::ProfileSPDLinDirectSolver::solve(); }

int XC::ProfileSPDLinSubstrSolver::setSize(void)
  { return this->XC::ProfileSPDLinDirectSolver::setSize(); }


//! Causes the condenser to form
//! \f$A_{ee}^*= A_{ee} -A_{ei} A_{ii}^{-1} A_{ie}\f$, where
//! \f$A_{ii}\f$ is the first \p numInt rows of the \f$A\f$ matrix.  The
//! original \f$A\f$ is changed as a result. \f$A_{ee}^*\f$ is to be
//! stored in \f$A_{ee}\f$.
//!
//! purpose: A procedure which takes the stifness matrix A= | A11 A12 |
//!							    | A21 A22 |
//!	    and does the following:
//!
//!		1) replaces A11 with D1 & U11, where A11= U11'*D1*U11
//!
//!		2) replaces A12 with M, where M= inv(U11')*A12
//!
//!		3) replaces A22 with Kdash, where Kdash= A22 - A21*inv(A11)*A12
//!							= A22 - M'*inv(D1)*M
//!
//! inputs: K[]          - the nxn matrix symmetric matrix
//!         Profile[]    - a vector outlining the profile of K
//!         n         	- the size of the system
//!	   in		- the no of internal dof (<n)
//!
//! outputs: int    0 if no error
//!                -1 if diag term < MINDIAG
//!                -2 if diag tem becomes <= 0
//!
int XC::ProfileSPDLinSubstrSolver::condenseA(int numInt)
  {
      /*
    for(int iii=0; iii<theSOE->size; iii++) {
      int rowiiitop= RowTop[iii];
      double *ajiptr= topRowPtr[iii];
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "\n COLUMN " << iii << " TopRow " << rowiiitop << " -> ";
      for(int jjj= rowiiitop; jjj <=iii; jjj++)
	std::cerr << *ajiptr++ << " ";
    }
    std::cerr << std::endl;
    */

    // check for quick return
    if(theSOE == nullptr)
      return -1;

    if(numInt == 0)
      {
	theSOE->numInt= numInt;	
	return 0;
      }

    if(dSize != size)
      {
        DU= Vector(numInt);
	dSize= numInt;
      }
		
    //
    //  form D1 & U11, store in A11
    //    - where A11= U11'*D1*U11
    //    - done using Crout decomposition
    //


    this->factor(numInt);

    /*
     *  form M, leave in A12
     *   - where M= inv(U11')*A12
     */

    for(int i=numInt; i<size; i++)
      {
	const int rowitop= RowTop[i];
	double *ajiPtr= topRowPtr[i];

	int jstrt= rowitop;
	if(rowitop == 0)
          {
	    jstrt= 1;
	    ajiPtr++;
	  }
        else
          { jstrt= rowitop; }
	
	for(int j=jstrt; j<numInt; j++)
          {
	    double tmp= *ajiPtr;
	    int rowjtop= RowTop[j];
	    double *akiPtr, *akjPtr;
	    if(rowitop > rowjtop)
              {
		akiPtr= topRowPtr[i];
		akjPtr= topRowPtr[j] + (rowitop-rowjtop);
		for(int k=rowitop; k<j; k++)
		  tmp -= *akjPtr++ * *akiPtr++ ;
	
	      }
            else
              {
		akiPtr= topRowPtr[i] + (rowjtop-rowitop);
		akjPtr= topRowPtr[j];
	
		for(int k=rowjtop; k<j; k++)
		  tmp -= *akjPtr++ * *akiPtr++ ;	
	      }
	    *ajiPtr++= tmp;
          }
      }


    /*
     * Now form K*, leave in A22
     *  - where K*= A22 - M'*inv(D11)*M
     */

    for(int i=numInt; i<size; i++)
      {
	const int rowitop= RowTop[i];
	double *ajiPtr=  topRowPtr[i];;

	int jstrt= rowitop;

	if(rowitop < numInt)
          {
	    ajiPtr+= (numInt-rowitop);
	    jstrt= numInt;
	  }

	double *DUPtr= DU.getDataPtr();
	double *akiPtr=  topRowPtr[i];

	for(int k=rowitop; k<numInt; k++)
	  *DUPtr++= *akiPtr++ * invD[k];


	for(int j=jstrt; j<=i; j++)
          {
	    double tmp= *ajiPtr;
	    const int rowjtop= RowTop[j];
	    double *akiPtr, *akjPtr;
	
	    if(rowitop > rowjtop)
              {
	        akiPtr= DU.getDataPtr();
		akjPtr= topRowPtr[j] + (rowitop-rowjtop);
		
		for(int k=rowitop; k<numInt; k++)
		  tmp -= *akjPtr++ * *akiPtr++;

	      }
            else
              {
		akiPtr= &DU[rowjtop-rowitop];
		akjPtr= topRowPtr[j];
		
		for(int k=rowjtop; k<numInt; k++)
		    tmp -= *akjPtr++ * *akiPtr++;
	      }
	    *ajiPtr++= tmp;
          }
      }

    theSOE->isAcondensed= true;
    theSOE->numInt= numInt;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " numDOF: " << size << "  numInt: " << numInt
	      << " numExt: " << size-numInt << std::endl;
    return 0;
  }




//! @brief Causes the condenser to form
//! \f$B_e^*= B_e - A_{ei} A_{ii}^{-1} B_i\f$, where \f$A_{ii}\f$ 
//! is the first \p numInt rows of \f$A\f$. The original \f$B\f$ is changed
//! as a result. \f$B_e^*\f$ is to be stored in \f$B_e\f$.
//!
//! purpose: A procedure which takes the stifness matrix A= | D1\U11   M  |
//!							    | A21    A22* |
//!	
//!	    and load vector B= | B1 |
//!				| B2 |
//!	    does the following:
//!
//!		1) replaces R1 with R1*, where R1*= inv(D11)inv(U11')R1
//!
//!		2) replaces R2 with R2*, where R2*= R2 - A21*inv(A11)*R1
//!						  = R2 - M'*R1*
//!
//! inputs: A[nxn] - the nxn matrix symmetric matrix  STORED AS ROW VECTOR IN COL MAJOR ORDER
//!	   R[n]   - the nX1 load vector
//!         n         	- the size of the system
//!	   in		- the no of internal dof (<n)
//!
//! outputs: int    0 if no error
//!                -1 if diag term < MINDIAG
//!                -2 if diag tem becomes <= 0
//!
int XC::ProfileSPDLinSubstrSolver::condenseRHS(int numInt, Vector *v)
  {

    // check for quick return
    if(theSOE == nullptr)
	return -1;

    if(numInt == 0)
      {
	theSOE->numInt= numInt;
	return 0;
      }

    // check A has been condensed & numInt was numInt given
    if(theSOE->isAcondensed != true)
      {
	int ok= this->condenseA(numInt);
	if(ok < 0)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to condenseA\n";
	    return ok;
	  }
      }

    if(theSOE->numInt != numInt)
      {	
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; numInt " << numInt << "does not agree with condensedA"
		  << " numInt " << theSOE->numInt << std::endl;
	return -1;
      }


    // set some pointers
    double *B= theSOE->getPtrB();

    //
    // form Y1*, leaving in Y1
    // - simply a triangular solve
    //

    // do forward substitution
    for(int i=1; i<numInt; i++)
      {
	const int rowitop= RowTop[i];	
	double *ajiPtr= topRowPtr[i];
	double *bjPtr = &B[rowitop];
	double tmp= 0;	
	
	for(int j=rowitop; j<i; j++)
	    tmp -= *ajiPtr++ * *bjPtr++;
	
	B[i]+= tmp;
      }

    // divide by diag term
    double *bjPtr= B;
    double *aiiPtr= invD.getDataPtr();
    for(int j=0; j<numInt; j++)
	*bjPtr++= *aiiPtr++ * B[j];

    //
    // Now form Y2*, leaving in Y2
    //

    for(int ii=numInt; ii<size; ii++)
      {
	const int rowitop= RowTop[ii];	
	double *ajiPtr= topRowPtr[ii];
	double *bjPtr = &B[rowitop];
	double tmp= 0;	
	
	for(int j=rowitop; j<numInt; j++)	
	    tmp -= *ajiPtr++ * *bjPtr++;

	B[ii]+= tmp;
      }
    /*
    for(int iii=0; iii<theSOE->size; iii++)
      std::cerr << "COLUMN " << iii << " Biii -> " << B[iii] << std::endl;
    std::cerr << std::endl;
    */

    return 0;
  }


//! @brief Causes the condenser to form \f$A_{ee} u\f$.
int XC::ProfileSPDLinSubstrSolver::computeCondensedMatVect(int numInt, const XC::Vector &theVect)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet\n";
    return -1;
  }

//! @brief Returns the contents of \f$A_{ee}\f$ as a matrix.
const XC::Matrix &XC::ProfileSPDLinSubstrSolver::getCondensedA(void)
  {
    const int numInt= theSOE->numInt;
    const int matSize= size - numInt;

    Aext= Matrix(matSize,matSize);

    // set the components of Aee to be the matrix
    Aext.Zero();
    for(int i=numInt; i<size; i++)
      {
	const int col= i - numInt;
	const int rowitop= RowTop[i];
	double *akiPtr= topRowPtr[i];
	
	int row= 0;
	if(rowitop < numInt)
	  akiPtr+= (numInt - rowitop);
	else
	  row= rowitop - numInt; // numInt FORTRAN index, rowitop C index
	
	while(row < col)
          {
	    double aki= *akiPtr++;
	    Aext(row,col)= aki;
	    Aext(col,row)= aki;	
	    row++;
	  }
	Aext(col,row)= *akiPtr;
      }
    return Aext;
  }

//! @brief Return the contents of \f$B_e\f$ as a Vector.
const XC::Vector &XC::ProfileSPDLinSubstrSolver::getCondensedRHS(void)
  {
    int numInt= theSOE->numInt;
    int matSize= size - numInt;

    double *Y= &(theSOE->getPtrB()[numInt]);
    Yext= Vector(Y,matSize);	
    return Yext;
  }

//! @brief Return the contents of the last call to {\em
//! computeCondensedMatVect()}.
const XC::Vector &XC::ProfileSPDLinSubstrSolver::getCondensedMatVect(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet\n";
    exit(-1);

    // needed for a strict compiler, program will exit before this
    Vector *errVect= nullptr;
    return *errVect;
  }


//! Sets the computed value of the unknowns in \f$X_e\f$ corresponding to the
//! external equations to \p u. The number of external equations is
//! given by the size of vector \f$u\f$.
int XC::ProfileSPDLinSubstrSolver::setComputedXext(const XC::Vector &xExt)
  {
    int retval= 0;
    if(xExt.Size() != (size - theSOE->numInt))
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; size mismatch " << xExt.Size() << " and "
		  << size-theSOE->numInt << std::endl;
	retval= -1;
      }
    else
      {
        double *xPtr= &(theSOE->getPtrX())[theSOE->numInt];
        for(int i=0; i<xExt.Size(); i++)
	    *xPtr++= xExt(i);
      }
    return retval;
  }

//! @brief To compute the internal equation numbers \f$X_i\f$ given the value
//! set for the external equations in the last call to setComputedXext().
//!
//! purpose: A procedure which takes the stifness matrix K= | D1\U11   M  |
//! 	                                                     | K21    K22* |
//!	
//!	    and load/displ vector R= | R1* |
//!				      | r2  |
//!	    does the following:
//!
//!		1) replaces R1* with r1, where r1= inv(K11){R1 - K12*r2}
//!						 = inv(D11)inv(U11'){(D1)R1* - Mr2}
//!
//! inputs: K[nxn] - the nxn matrix symmetric matrix  STORED AS ROW VECTOR
//! IN COL MAJ ORD
//!	   R[n]   - the nX1 load vector
//!         n         	- the size of the system
//!	   in		- the no of internal dof (<n)
//!
int XC::ProfileSPDLinSubstrSolver::solveXint(void)
  {

    /*
     * form X1= (D1)Y1* - M *X2, store in X1*
     */

    const int numInt= theSOE->numInt;
    double *X= theSOE->getPtrX();
    double *B= theSOE->getPtrB();

    for(int j=0; j<numInt; j++)
	X[j]= B[j]/invD[j];

    for(int i=numInt; i<size; i++)
      {
	const int rowitop= RowTop[i];
	double *ajiPtr= topRowPtr[i];
	double *XjPtr = &X[rowitop];
	double Xi= X[i];

	for(int j=rowitop; j<numInt; j++)
	    *XjPtr++ -= *ajiPtr++ * Xi;
      }

    for(int l=0; l<numInt; l++)
       X[l]= X[l]*invD[l];

  /*
   * form inv(U11)*A, store in A
   * - simply a triangular solve (back sub)
   */
    for(int k=(numInt-1); k>0; k--)
      {
	int rowktop= RowTop[k];
	double Xk= X[k];
	double *ajiPtr= topRowPtr[k]; 		

	for(int j=rowktop; j<k; j++)
	    X[j] -= *ajiPtr++ * Xk;
      }
    return 0;
  }

int XC::ProfileSPDLinSubstrSolver::getClassTag(void) const
  { return SOLVER_TAGS_ProfileSPDLinSubstrSolver; }

int XC::ProfileSPDLinSubstrSolver::sendSelf(CommParameters &cp)
  {
    if(size != 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; does not send itself YET.\n";
    return 0;
  }


int XC::ProfileSPDLinSubstrSolver::recvSelf(const CommParameters &cp)
  { return 0; }
