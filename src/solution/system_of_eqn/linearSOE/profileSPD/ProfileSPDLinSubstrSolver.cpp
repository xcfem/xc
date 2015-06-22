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
// $Date: 2003/02/14 23:02:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/ProfileSPD/ProfileSPDLinSubstrSolver.C
//
// Written: fmk 
// Created: Febuary 1997
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
 DomainSolver(SOLVER_TAGS_ProfileSPDLinSubstrSolver),
 dSize(0) {}

    
int XC::ProfileSPDLinSubstrSolver::solve()
  { return this->XC::ProfileSPDLinDirectSolver::solve(); }

int XC::ProfileSPDLinSubstrSolver::setSize(void)
  { return this->XC::ProfileSPDLinDirectSolver::setSize(); }    

/* XC::ProfileSPDLinSubstrSolver::condenseA(int numInt)
**
** purpose: A procedure which takes the stifness matrix A = | A11 A12 |
**							    | A21 A22 |
**	    and does the following:
**
**		1) replaces A11 with D1 & U11, where A11 = U11'*D1*U11
**
**		2) replaces A12 with M, where M = inv(U11')*A12
**
**		3) replaces A22 with Kdash, where Kdash = A22 - A21*inv(A11)*A12
**							= A22 - M'*inv(D1)*M
**
** inputs: K[]          - the nxn matrix symmetric matrix  
**         Profile[]    - a vector outlining the profile of K
**         n         	- the size of the system
**	   in		- the no of internal dof (<n)
**
** outputs: int    0 if no error
**                -1 if diag term < MINDIAG
**                -2 if diag tem becomes <= 0
**
*/


int XC::ProfileSPDLinSubstrSolver::condenseA(int numInt)
  {
  /*
for (int iii=0; iii<theSOE->size; iii++) {
  int rowiiitop = RowTop[iii];
  double *ajiptr = topRowPtr[iii];
  std::cerr << "\n COLUMN " << iii << " TopRow " << rowiiitop << " -> ";
  for (int jjj = rowiiitop; jjj <=iii; jjj++)
    std::cerr << *ajiptr++ << " ";
}
std::cerr << std::endl;
*/
    
    // check for quick return
    if(theSOE == 0)
      return -1;

    if(numInt == 0)
      {
	theSOE->numInt = numInt;	
	return 0;
      }

    if(dSize != size)
      {
        DU= Vector(numInt);
	dSize = numInt;
      }
		
    //
    //  form D1 & U11, store in A11
    //    - where A11 = U11'*D1*U11
    //    - done using Crout decomposition
    //


    this->factor(numInt);
    
    /*
     *  form M, leave in A12
     *   - where M = inv(U11')*A12
     */
    int i;

    for(i=numInt; i<size; i++)
      {
	int rowitop = RowTop[i];
	double *ajiPtr = topRowPtr[i];

	int jstrt = rowitop;
	if(rowitop == 0)
          {
	    jstrt = 1;
	    ajiPtr++;
	  }
        else
          { jstrt = rowitop; }

	
	for(int j=jstrt; j<numInt; j++)
          {
	    double tmp = *ajiPtr;
	    int rowjtop = RowTop[j];
	    double *akiPtr, *akjPtr;
	    if(rowitop > rowjtop)
              {
		akiPtr = topRowPtr[i];
		akjPtr = topRowPtr[j] + (rowitop-rowjtop);
		for(int k=rowitop; k<j; k++)
		  tmp -= *akjPtr++ * *akiPtr++ ;
	      
	      }
            else
              {
		akiPtr = topRowPtr[i] + (rowjtop-rowitop);
		akjPtr = topRowPtr[j];
	       
		for(int k=rowjtop; k<j; k++) 
		  tmp -= *akjPtr++ * *akiPtr++ ;	    
	      }
	    *ajiPtr++ = tmp;
          }
      }


    /*
     * Now form K*, leave in A22
     *  - where K* = A22 - M'*inv(D11)*M
     */

    for(i=numInt; i<size; i++)
      {
	int rowitop = RowTop[i];
	double *ajiPtr =  topRowPtr[i];;

	int jstrt;

	if(rowitop < numInt)
          {
	    ajiPtr += (numInt-rowitop);
	    jstrt = numInt;
	  }
	else
	  jstrt = rowitop;

	double *DUPtr = DU.getDataPtr(); 
	double *akiPtr =  topRowPtr[i];

	for(int k=rowitop; k<numInt; k++)
	  *DUPtr++ = *akiPtr++ * invD[k];

       
	for(int j=jstrt; j<=i; j++)
          {
	    double tmp = *ajiPtr;
	    int rowjtop = RowTop[j];
	    double *akiPtr, *akjPtr;
	    
	    if(rowitop > rowjtop)
              {
	        akiPtr = DU.getDataPtr();
		akjPtr = topRowPtr[j] + (rowitop-rowjtop);
		
		for(int k=rowitop; k<numInt; k++) 
		  tmp -= *akjPtr++ * *akiPtr++;

	      }
            else
              {
		akiPtr = &DU[rowjtop-rowitop];
		akjPtr = topRowPtr[j];
		
		for(int k=rowjtop; k<numInt; k++) 
		    tmp -= *akjPtr++ * *akiPtr++;
	      }
	    *ajiPtr++ = tmp;
          }
      }      

    theSOE->isAcondensed = true;
    theSOE->numInt = numInt;
    std::cerr << "XC::ProfileSPDLinSubstrSolver::condenseA  numDOF: " << size << "  numInt: " << numInt << "  numExt: " << size-numInt << std::endl;
    return 0;
  }




/* name: condenseRHS
**
** purpose: A procedure which takes the stifness matrix A = | D1\U11   M  | 
**							    | A21    A22* |
**	    
**	    and load vector B = | B1 |
**				| B2 |
**	    does the following:
**
**		1) replaces R1 with R1*, where R1* = inv(D11)inv(U11')R1
**
**		2) replaces R2 with R2*, where R2* = R2 - A21*inv(A11)*R1
**						   = R2 - M'*R1*
**
** inputs: A[nxn] - the nxn matrix symmetric matrix  STORED AS ROW VECTOR IN COL MAJOR ORDER
**	   R[n]   - the nX1 load vector
**         n         	- the size of the system
**	   in		- the no of internal dof (<n)
**
** outputs: int    0 if no error
**                -1 if diag term < MINDIAG
**                -2 if diag tem becomes <= 0
**
*/

int 
XC::ProfileSPDLinSubstrSolver::condenseRHS(int numInt, Vector *v)
{

    // check for XC::quick return
    if (theSOE == 0)
	return -1;

    if (numInt == 0) {
	theSOE->numInt = numInt;
	return 0;
    }

    // check A has been condensed & numInt was numInt given
    if (theSOE->isAcondensed != true) {
	int ok = this->condenseA(numInt);
	if (ok < 0) {
	    std::cerr << "XC::ProfileSPDLinSubstrSolver::condenseRHS()";
	    std::cerr << " - failed to condenseA\n";
	    return ok;
	}
    }

    if (theSOE->numInt != numInt) {	
	std::cerr << "XC::ProfileSPDLinSubstrSolver::condenseRHS()";
	std::cerr << " - numInt " << numInt << "does not agree with condensedA";
	std::cerr << " numInt " << theSOE->numInt << std::endl;
	return -1;
    }


    // set some pointers
    double *B = theSOE->getPtrB();

    //
    // form Y1*, leaving in Y1
    // - simply a triangular solve
    //

    // do forward substitution 
    for (int i=1; i<numInt; i++) {
	
	int rowitop = RowTop[i];	    
	double *ajiPtr = topRowPtr[i];
	double *bjPtr  = &B[rowitop];  
	double tmp = 0;	    
	
	for (int j=rowitop; j<i; j++) 
	    tmp -= *ajiPtr++ * *bjPtr++; 
	
	B[i] += tmp;
    }

    // divide by diag term 
    double *bjPtr = B; 
    double *aiiPtr = invD.getDataPtr();
    for (int j=0; j<numInt; j++) 
	*bjPtr++ = *aiiPtr++ * B[j];

    //
    // Now form Y2*, leaving in Y2
    //

    for (int ii=numInt; ii<size; ii++) {
	int rowitop = RowTop[ii];	    
	double *ajiPtr = topRowPtr[ii];
	double *bjPtr  = &B[rowitop];  
	double tmp = 0;	    
	
	for (int j=rowitop; j<numInt; j++)	
	    tmp -= *ajiPtr++ * *bjPtr++;

	B[ii] += tmp;
    }
    /*
      for (int iii=0; iii<theSOE->size; iii++) {
      std::cerr << "COLUMN " << iii << " Biii -> " << B[iii] << std::endl;
      }
      std::cerr << std::endl;
      */

    return 0;
}


int XC::ProfileSPDLinSubstrSolver::computeCondensedMatVect(int numInt, const XC::Vector &theVect) 
  {
    std::cerr << "XC::ProfileSPDLinSubstrSolver::computeCondensedMatVect() -";
    std::cerr << " not implemented yet\n";
    return -1;
  }


const XC::Matrix &XC::ProfileSPDLinSubstrSolver::getCondensedA(void)
  {
    int numInt = theSOE->numInt;
    int matSize = size - numInt;

    Aext= Matrix(matSize,matSize);

    // set the components of Aee to be the matrix
    Aext.Zero();
    for(int i=numInt; i<size; i++)
      {
	int col = i - numInt;

	int rowitop = RowTop[i];
	double *akiPtr = topRowPtr[i];
	
	int row =0;
	if(rowitop < numInt) 
	  akiPtr += (numInt - rowitop);
	else
	  row = rowitop - numInt; // numInt FORTRAN index, rowitop C index
	    
	while(row < col)
          {
	    double aki = *akiPtr++;
	    Aext(row,col) = aki;
	    Aext(col,row) = aki;	    
	    row++;
	  }
	Aext(col,row) = *akiPtr;
      }
    return Aext;
  }


const XC::Vector &XC::ProfileSPDLinSubstrSolver::getCondensedRHS(void)
  {
    int numInt = theSOE->numInt;
    int matSize = size - numInt;

    double *Y = &(theSOE->getPtrB()[numInt]);
    Yext= Vector(Y,matSize);	
    return Yext;
  }

const XC::Vector &XC::ProfileSPDLinSubstrSolver::getCondensedMatVect(void)
  {
    std::cerr << "XC::ProfileSPDLinSubstrSolver::computeCondensedMatVect() -";
    std::cerr << " not implemented yet\n";
    exit(-1);

    // needed for a strict compiler, program will exit before this
    Vector *errVect= nullptr; 
    return *errVect;
      
  }


int XC::ProfileSPDLinSubstrSolver::setComputedXext(const XC::Vector &xExt)
  {
    if (xExt.Size() != (size - theSOE->numInt)) {
	std::cerr << "XC::ProfileSPDLinSubstrSolver::setComputedxExt() :";
	std::cerr << " - size mismatch " << xExt.Size() << " and ";
	std::cerr << size-theSOE->numInt << std::endl;
	return -1;
    }

    double *xPtr = &(theSOE->getPtrX())[theSOE->numInt];
    for (int i=0; i<xExt.Size(); i++)
	*xPtr++ = xExt(i);

    return 0;
  }    


/* name: S O L V r I
**
** purpose: A procedure which takes the stifness matrix K = | D1\U11   M  | 
**							    | K21    K22* |
**	    
**	    and load/displ vector R = | R1* |
**				      | r2  |
**	    does the following:
**
**		1) replaces R1* with r1, where r1 = inv(K11){R1 - K12*r2}
**						  = inv(D11)inv(U11'){(D1)R1* - Mr2}
**
** inputs: K[nxn] - the nxn matrix symmetric matrix  STORED AS ROW VECTOR IN COL MAJ ORD
**	   R[n]   - the nX1 load vector
**         n         	- the size of the system
**	   in		- the no of internal dof (<n)
**
**
*/
int XC::ProfileSPDLinSubstrSolver::solveXint(void)
  {

  /*
   * form X1 = (D1)Y1* - M *X2, store in X1*
   */

    int numInt = theSOE->numInt;
    double *X = theSOE->getPtrX();
    double *B = theSOE->getPtrB();
    
    for (int j=0; j<numInt; j++) 
	X[j] = B[j]/invD[j];

    for (int i=numInt; i<size; i++) {
	
	int rowitop = RowTop[i];
	double *ajiPtr = topRowPtr[i];
	double *XjPtr  = &X[rowitop];
	double Xi = X[i];

	for (int j=rowitop; j<numInt; j++)
	    *XjPtr++ -= *ajiPtr++ * Xi;
    }

    for (int l=0; l<numInt; l++) 
       X[l] = X[l]*invD[l];

  /*
   * form inv(U11)*A, store in A
   * - simply a triangular solve (back sub)
   */
    for (int k=(numInt-1); k>0; k--) {

	int rowktop = RowTop[k];
	double Xk = X[k];
	double *ajiPtr = topRowPtr[k]; 		

	for (int j=rowktop; j<k; j++) 
	    X[j] -= *ajiPtr++ * Xk;
    }   	     
    return 0;
  }

int XC::ProfileSPDLinSubstrSolver::getClassTag(void) const
  { return SOLVER_TAGS_ProfileSPDLinSubstrSolver; }

int XC::ProfileSPDLinSubstrSolver::sendSelf(CommParameters &cp)
  {
    if(size != 0)
      std::cerr << "XC::ProfileSPDLinSubstrSolver::sendSelf - does not send itself YET\n"; 
    return 0;
  }


int XC::ProfileSPDLinSubstrSolver::recvSelf(const CommParameters &cp)
  { return 0; }
