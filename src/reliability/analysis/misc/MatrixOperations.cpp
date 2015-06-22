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
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.5 $
// $Date: 2003/03/04 00:39:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/misc/MatrixOperations.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/misc/MatrixOperations.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <cmath>
#include <string.h>


XC::MatrixOperations::MatrixOperations(Matrix passedMatrix)
{
	int rows = passedMatrix.noRows();
	int cols = passedMatrix.noCols();

	theMatrix = new XC::Matrix(rows,cols);
	(*theMatrix) = passedMatrix;

	theLowerCholesky = new XC::Matrix(rows,cols);
	theInverseLowerCholesky = new XC::Matrix(rows,cols);
	theInverse = new XC::Matrix(rows,cols);
	theTranspose = new XC::Matrix(rows,cols);
	theSquareRoot = new XC::Matrix(rows,cols);

	theMatrixNorm = 0;
	theTrace = 0; 

}


XC::MatrixOperations::~MatrixOperations()
{
	delete theMatrix;
	delete theLowerCholesky;
	delete theInverseLowerCholesky;
	delete theInverse;
	delete theTranspose;
	delete theSquareRoot;
}



int
XC::MatrixOperations::setMatrix(Matrix passedMatrix)
{
	int rows = passedMatrix.noRows();
	int cols = passedMatrix.noCols();

	// delete
	delete theMatrix;
	delete theLowerCholesky;
	delete theInverseLowerCholesky;
	delete theInverse;
	delete theTranspose;
	delete theSquareRoot;

	// reallocate
	theMatrix = new XC::Matrix(rows,cols);
	(*theMatrix) = passedMatrix;
	theLowerCholesky = new XC::Matrix(rows,cols);
	theInverseLowerCholesky = new XC::Matrix(rows,cols);
	theInverse = new XC::Matrix(rows,cols);
	theTranspose = new XC::Matrix(rows,cols);
	theSquareRoot = new XC::Matrix(rows,cols);

	return 0;
}


 XC::Matrix 
XC::MatrixOperations::getMatrix()
{
	return (*theMatrix);
}


 XC::Matrix 
XC::MatrixOperations::getLowerCholesky()
{
	if (theLowerCholesky == 0) {
		std::cerr << "XC::MatrixOperations::getLowerCholesky() - this" << std::endl
			<< " matrix has not been computed." << std::endl;
		return (*theMatrix);
	}

	return (*theLowerCholesky);
}


 XC::Matrix 
XC::MatrixOperations::getInverseLowerCholesky()
{
	if (theInverseLowerCholesky == 0) {
		std::cerr << "XC::MatrixOperations::getInverseLowerCholesky() - this" << std::endl
			<< " matrix has not been computed." << std::endl;
		return (*theMatrix);
	}

	return (*theInverseLowerCholesky);
}


 XC::Matrix 
XC::MatrixOperations::getInverse()
{
	if (theInverse == 0) {
		std::cerr << "XC::MatrixOperations::getInverse() - this" << std::endl
			<< " matrix has not been computed." << std::endl;
		return (*theMatrix);
	}
	
	return (*theInverse);
}


 XC::Matrix 
XC::MatrixOperations::getTranspose()
{
	if (theTranspose == 0) {
		std::cerr << "XC::MatrixOperations::getTranspose() - this" << std::endl
			<< " matrix has not been computed." << std::endl;
		return (*theMatrix);
	}
	
	return (*theTranspose);
}


 XC::Matrix 
XC::MatrixOperations::getSquareRoot()
{
	if (theSquareRoot == 0) {
		std::cerr << "XC::MatrixOperations::getSquareRoot() - this" << std::endl
			<< " matrix has not been computed." << std::endl;
		return (*theMatrix);
	}
	
	return (*theSquareRoot);
}


double
XC::MatrixOperations::getMatrixNorm()
{
	return theMatrixNorm;
}


double
XC::MatrixOperations::getTrace()
{
	return theTrace;
}


int
XC::MatrixOperations::computeLowerCholesky()
{
	Matrix passedMatrix = (*theMatrix);

	// This algorithm is more or less like given in Professor Der Kiureghians
	// handout/brief in CE193 on Cholesky decomposition.

	// Should first check that the matrix is quadratic, etc. 

	int sizeOfPassedMatrix = passedMatrix.noCols();
	int i, j, k;
	double sumOfLambda_i_k_squared = 0;
	double sumOfLambda_i_kLambda_j_k = 0;
	Matrix lambda( sizeOfPassedMatrix , sizeOfPassedMatrix );

	for ( i=0 ; i<sizeOfPassedMatrix ; i++ ) 
	{
		for ( j=0 ; j<sizeOfPassedMatrix ; j++ )
		{
			sumOfLambda_i_k_squared = 0;
			sumOfLambda_i_kLambda_j_k = 0;
			lambda(i,j) = 0.0;
			for ( k=0 ; k<i ; k++ )
			{
				sumOfLambda_i_k_squared += pow ( lambda(i,k) , 2 );
			}
			for ( k=0 ; k<j ; k++ )
			{
				sumOfLambda_i_kLambda_j_k += lambda(i,k) * lambda(j,k);
			}
			if ( i == j )
			{
				if ( ( passedMatrix(i,j) - sumOfLambda_i_k_squared ) < 1.0e-8) {
					std::cerr << "WARNING: XC::MatrixOperations::computeLowerCholesky()" << std::endl
						<< " ... matrix may be close to singular. " << std::endl;
				}
				lambda(i,j) = sqrt ( passedMatrix(i,j) - sumOfLambda_i_k_squared );
			}
			if ( i > j )
			{
				if ( fabs(lambda(j,j)) < 1.0e-8) {
					std::cerr << "WARNING: XC::MatrixOperations::computeLowerCholesky()" << std::endl
						<< " ... matrix may be close to singular. " << std::endl;
				}
				lambda(i,j) = ( passedMatrix(i,j) - sumOfLambda_i_kLambda_j_k ) / lambda(j,j);
			}
			if ( i < j )
			{
				lambda(i,j) = 0.0;
			}
		}
	}

	(*theLowerCholesky) = lambda;

	return 0;
}




int
XC::MatrixOperations::computeInverseLowerCholesky()
{
	Matrix passedMatrix = (*theMatrix);

	// This algorithm is more or less like given in Professor Der Kiureghians
	// handout/brief in CE193 on Cholesky decomposition.

	// Should first check that the matrix is quadratic, etc. 

	int sizeOfPassedMatrix = passedMatrix.noCols();
	
	this->computeLowerCholesky();
	Matrix lambda = this->getLowerCholesky();

	Matrix gamma( sizeOfPassedMatrix , sizeOfPassedMatrix );
	int i, j, k;
	double sumOfLambda_i_kGamma_k_j = 0;
	for ( i=0 ; i<sizeOfPassedMatrix ; i++ ) 
	{
		for ( j=0 ; j<sizeOfPassedMatrix ; j++ )
		{
			sumOfLambda_i_kGamma_k_j = 0;
			gamma(i,j) = 0.0;
			for ( k=j ; k<i ; k++ )
			{
				sumOfLambda_i_kGamma_k_j += lambda(i,k) * gamma(k,j);
			}
			if ( i == j )
			{
				gamma(i,j) = 1 / lambda(i,i);
			}
			if ( i > j )
			{
				if ( fabs(lambda(i,i)) < 1.0e-8) {
					std::cerr << "WARNING: XC::MatrixOperations::computeInverseLowerCholesky()" << std::endl
						<< " ... matrix may be close to singular. " << std::endl;
				}
				gamma(i,j) = - sumOfLambda_i_kGamma_k_j / lambda(i,i);
			}
			if ( i < j )
			{
				gamma(i,j) = 0.0;
			}
		}
	}

	(*theInverseLowerCholesky) = gamma;

	return 0;
}


int
XC::MatrixOperations::computeCholeskyAndItsInverse()
{
	Matrix &passedMatrix = (*theMatrix);

	// This algorithm is more or less like given in Professor Der Kiureghians
	// handout/brief in CE193 on Cholesky decomposition.

	// Should first check that the matrix is quadratic, etc. 

	int sizeOfPassedMatrix = passedMatrix.noCols();
	double sumOfLambda_i_k_squared = 0.0;
	double sumOfLambda_i_kLambda_j_k = 0.0;
	double sumOfLambda_i_kGamma_k_j = 0.0;
	Matrix lambda( sizeOfPassedMatrix , sizeOfPassedMatrix );
	Matrix gamma( sizeOfPassedMatrix , sizeOfPassedMatrix );
	int i, j, k;

	// Lower Cholesky
	for ( i=0 ; i<sizeOfPassedMatrix ; i++ ) 
	{
		for ( j=0 ; j<sizeOfPassedMatrix ; j++ )
		{
			sumOfLambda_i_k_squared = 0.0;
			sumOfLambda_i_kLambda_j_k = 0.0;
			lambda(i,j) = 0.0;
			for ( k=0 ; k<i ; k++ )
			{
				sumOfLambda_i_k_squared += pow ( lambda(i,k) , 2.0 );
			}
			for ( k=0 ; k<j ; k++ )
			{
				sumOfLambda_i_kLambda_j_k += lambda(i,k) * lambda(j,k);
			}
			if ( i == j )
			{
				if ( ( passedMatrix(i,j) - sumOfLambda_i_k_squared ) < 1.0e-8) {
					std::cerr << "WARNING: XC::MatrixOperations::computeCholeskyAndItsInverse()" << std::endl
						<< " ... matrix may be close to singular. " << std::endl;
				}
				lambda(i,j) = sqrt ( passedMatrix(i,j) - sumOfLambda_i_k_squared );
			}
			if ( i > j )
			{
				if ( fabs(lambda(j,j)) < 1.0e-8) {
					std::cerr << "WARNING: XC::MatrixOperations::computeCholeskyAndItsInverse()" << std::endl
						<< " ... matrix may be close to singular. " << std::endl;
				}
				lambda(i,j) = ( passedMatrix(i,j) - sumOfLambda_i_kLambda_j_k ) / lambda(j,j);
			} 
			if ( i < j )
			{
				lambda(i,j) = 0.0;
			}
		}
	}

	// Inverse lower Cholesky
	for ( i=0 ; i<sizeOfPassedMatrix ; i++ ) 
	{
		for ( j=0 ; j<sizeOfPassedMatrix ; j++ )
		{
			sumOfLambda_i_kGamma_k_j = 0;
			gamma(i,j) = 0.0;
			for ( k=j ; k<i ; k++ )
			{
				sumOfLambda_i_kGamma_k_j += lambda(i,k) * gamma(k,j);
			}
			if ( i == j )
			{
				gamma(i,j) = 1 / lambda(i,i);
			}
			if ( i > j )
			{
				if ( fabs(lambda(i,i)) < 1.0e-8) {
					std::cerr << "WARNING: XC::MatrixOperations::computeCholeskyAndItsInverse()" << std::endl
						<< " ... matrix may be close to singular. " << std::endl;
				}
				gamma(i,j) = - sumOfLambda_i_kGamma_k_j / lambda(i,i);
			}
			if ( i < j )
			{
				gamma(i,j) = 0.0;
			}
		}
	}

	(*theLowerCholesky) = lambda;
	(*theInverseLowerCholesky) = gamma;

	return 0;
}






int
XC::MatrixOperations::computeTranspose()
{
	Matrix &A = (*theMatrix);

	int sizeOfA = A.noCols();

	Matrix B(sizeOfA,sizeOfA);

	for (int i=0; i<sizeOfA; i++) {
		for (int j=0; j<sizeOfA; j++) {
			B(i,j) = A(j,i);
		}
	}

	(*theTranspose) = B;

	return 0;
}





int
XC::MatrixOperations::computeSquareRoot()
{
	Matrix &A = (*theMatrix);

	int sizeOfA = A.noCols();

	Matrix B(sizeOfA,sizeOfA);

	for (int i=0; i<sizeOfA; i++) {
		for (int j=0; j<sizeOfA; j++) {
			B(i,j) = sqrt( A(i,j) );
		}
	}

	(*theSquareRoot) = B;

	return 0;
}



int
XC::MatrixOperations::computeInverse()
{

	Matrix &A = (*theMatrix);

	// Return the invers matrix B such that A*B=I
	int sizeOfA = A.noCols();
	Matrix B ( sizeOfA, sizeOfA );
	Matrix AB ( sizeOfA, 2*sizeOfA );
	int i, j, k;

	// Set up the AB matrix
	for ( i=0 ; i<sizeOfA ; i++ )
	{
		for ( j=0 ; j<(sizeOfA*2) ; j++ )
		{
			if ( j < sizeOfA )
			{
				AB(i,j) = A(i,j);
			}
			else
			{
				if ( j == (sizeOfA+i) )
				{
					AB(i,j) = 1.0;
				}
				else
				{
					AB(i,j) = 0.0;
				}
			}
		}
	}

	// The Gauss-Jordan method
	double pivot;
	double ABii;

	for ( k=0 ; k<sizeOfA ; k++ )
	{
		for ( i=k ; i<sizeOfA ; i++ )
		{
			ABii = AB(i,i);
			pivot = AB(i,k);
			for ( j=k ; j<(sizeOfA*2) ; j++ )
			{
				if ( i == k )
				{
					AB(i,j) = AB(i,j) / ABii;
				}
				else
				{
					AB(i,j) = AB(i,j) - pivot * AB(k,j);
				}
			}
		}
	}
	for ( k=1 ; k<sizeOfA ; k++ )
	{
		for ( i=k ; i<sizeOfA ; i++ )
		{
			pivot = AB( (sizeOfA-i-1), (sizeOfA-k) );
			for ( j=(sizeOfA-k) ; j<(sizeOfA*2) ; j++ )
			{
					AB((sizeOfA-i-1),(j)) = 
						AB((sizeOfA-i-1),(j)) - 
						pivot * AB((sizeOfA-k),(j));
			}
		}
	}


	// Collect the B matrix
	for ( i=0 ; i<sizeOfA ; i++ )
	{
		for ( j=sizeOfA ; j<(sizeOfA*2) ; j++ )
		{
			B(i,(j-sizeOfA)) = AB(i,j);
		}
	}

	(*theInverse) = B;
	
	return 0;
}



int
XC::MatrixOperations::computeMatrixNorm()
{
	Matrix &passedMatrix = (*theMatrix);

	int numberOfColumns = passedMatrix.noCols();
	int numberOfRows = passedMatrix.noRows();
	double sum = 0;
	for ( int i=0 ; i<numberOfRows ; i++ )
	{
		for ( int j=0 ; j<numberOfColumns ; j++ )
		{
			sum += passedMatrix(i,j) * passedMatrix(i,j);
		}
	}

	theMatrixNorm = sqrt(sum);

	return 0;
}


int
XC::MatrixOperations::computeTrace()
{
	Matrix &passedMatrix = (*theMatrix);

	int numberOfColumns = passedMatrix.noCols();
	int numberOfRows = passedMatrix.noRows();
	
	if (numberOfColumns != numberOfRows) {
		std::cerr << "XC::MatrixOperations::computeTrace() - can not" << std::endl
			<< " compute the trace of a non-quadratic matrix." << std::endl;
		return -1;
	}

	double product = 1.0;
	for ( int i=0 ; i<numberOfRows ; i++ )
	{
		product = product * passedMatrix(i,i);
	}

	theTrace = product;

	return 0;
}





/*
double
XC::MatrixOperations::vectorDotProduct(Vector vector1, Vector vector2)
{
	int sizeOfVector1 = vector1.Size();
	int sizeOfVector2 = vector2.Size();
	double sum = 0;

	if ( sizeOfVector1 != sizeOfVector2 )
	{
		std::cerr << "vectorDotProduct can not be performed" << std::endl;
	}
	else
	{
		for ( int i=0 ; i<sizeOfVector1 ; i++ )
		{
			sum += vector1(i) * vector2(i);
		}
	}

	return sum;
}
*/


/*
 XC::Vector 
XC::MatrixOperations::matrixTimesVector(Matrix passedMatrix, Vector passedVector)
{
	int numberOfMatrixRows = passedMatrix.noRows();
	int sizeOfPassedVector = passedVector.Size();
	Vector result(numberOfMatrixRows);
	int i, j;
	double sum;

	for ( i=0 ; i<numberOfMatrixRows ; i++ )
	{
		sum = 0.0;
		for ( j=0 ; j<sizeOfPassedVector ; j++ )
		{
			sum += passedMatrix(i,j) * passedVector(j);
		}
		result(i) = sum;
	}

	return result;
}
*/
