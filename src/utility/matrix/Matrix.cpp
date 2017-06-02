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
                                                                        
// $Revision: 1.12 $
// $Date: 2003/04/02 22:02:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/matrix/Matrix.cpp,v $
                                                                        
                                                                        
// File: ~/matrix/Matrix.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class implementation for XC::Matrix.
//
// What: "@(#) Matrix.h, revA"

#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/ID.h"
#include "utility/matrix/nDarray/Tensor.h"
#include <cstdlib>
#include <iostream>
#include "xc_basic/src/matrices/m_double.h"

//#include <boost/any.hpp>

#include "AuxMatrix.h"

#define MATRIX_WORK_AREA 400
#define INT_WORK_AREA 20

XC::AuxMatrix XC::Matrix::auxMatrix(MATRIX_WORK_AREA,INT_WORK_AREA);
double XC::Matrix::MATRIX_NOT_VALID_ENTRY =0.0;

//! @brief Constructor.
XC::Matrix::Matrix(void)
  :numRows(0), numCols(0) {}


//! @brief Constructor.
XC::Matrix::Matrix(int nRows,int nCols)
  :numRows(nRows), numCols(nCols), data(numRows*numCols)
  {
#ifdef _G3DEBUG
    if(nRows < 0)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; WARNING: tried to init matrix ";
        std::cerr << "with num rows: " << nRows << " <0\n";
        numRows= 0; numCols =0;
      }
    if(nCols < 0)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; WARNING: tried to init matrix";
        std::cerr << "with num cols: " << nCols << " <0\n";
        numRows= 0; numCols =0;
      }
#endif
    data.Zero();
  }

XC::Matrix::Matrix(double *theData, int row, int col) 
  :numRows(row),numCols(col),data(theData,row*col)
  {
#ifdef _G3DEBUG
    if(row < 0)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; WARNING: tried to init matrix with numRows: ";
        std::cerr << row << " <0\n";
        numRows= 0; numCols =0;
      }
    if(col < 0)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; WARNING: tried to init matrix with numCols: ";
        std::cerr << col << " <0\n";
        numRows= 0; numCols =0;
      }    
#endif
  }

//! @brief Constructor (interfaz Python).
XC::Matrix::Matrix(const boost::python::list &l)
  :numRows(len(l)), numCols(0)
  {
    boost::python::list row0= boost::python::extract<boost::python::list>(l[0]);
    numCols= len(row0);
    // copy the components
    resize(numRows,numCols);
    for(int i=0; i<numRows; i++)
      {
        boost::python::list rowI= boost::python::extract<boost::python::list>(l[i]);
        for(int j= 0; j<numCols;j++)
          (*this)(i,j)= boost::python::extract<double>(rowI[j]);
      }
  }

//
// METHODS - Zero, Assemble, Solve
//

int XC::Matrix::setData(double *theData, int row, int col) 
  {

    numRows= row;
    numCols= col;
    return data.setData(theData,row*col);
  }

void XC::Matrix::Zero(void)
  { data.Zero(); }

void XC::Matrix::Identity(void)
  {
    Zero();
    if(noRows()!=noCols())
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; not an square matrix." << std::endl;
    const int n= std::min(noRows(),noCols());
    for(int i=0;i<n;i++)
      (*this)(i,i)= 1.0;
  }

int XC::Matrix::resize(int rows, int cols)
  {

    const int newSize= rows*cols;

    if(newSize<0)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; rows " << rows << " or cols " << cols
		  << " specified <= 0\n";
        return -1;
      }
    else if(newSize == 0)
      {
        data.resize(0);
        numRows= rows;
        numCols= cols;
      }
    else if(newSize > data.Size())
      {
        data.resize(newSize);
        numRows= rows;
        numCols= cols;
      }
    else
      {
        // just reset the cols and rows - save two memory calls at expense of holding 
        // onto extra memory
        numRows= rows;
        numCols= cols;
      }
    return 0;
  }


int XC::Matrix::Assemble(const Matrix &V, const ID &rows, const ID &cols, double fact) 
  {
    int pos_Rows, pos_Cols;
    int res= 0;

    for(int i=0; i<cols.Size(); i++)
      {
        pos_Cols= cols(i);
        for(int j=0; j<rows.Size(); j++)
          {
            pos_Rows= rows(j);
      
            if((pos_Cols >= 0) && (pos_Rows >= 0) && (pos_Rows < numRows) &&
               (pos_Cols < numCols) && (i < V.numCols) && (j < V.numRows))
	      (*this)(pos_Rows,pos_Cols) += V(j,i)*fact;
            else
              {
                std::cerr << nombre_clase() << "::" << __FUNCTION__
			  << "; WARNING: position (" << pos_Rows
			  << "," << pos_Cols << ") outside bounds \n";
	        res= -1;
              }
          }
      }
    return res;
  }

#ifdef _WIN32
extern "C" int  DGESV(int *N, int *NRHS, double *A, int *LDA, 
			      int *iPiv, double *B, int *LDB, int *INFO);

extern "C" int  DGETRF(int *M, int *N, double *A, int *LDA, 
			      int *iPiv, int *INFO);

extern "C" int  DGETRS(char *TRANS, unsigned int sizeT,
			       int *N, int *NRHS, double *A, int *LDA, 
			       int *iPiv, double *B, int *LDB, int *INFO);

extern "C" int  DGETRI(int *N, double *A, int *LDA, 
			      int *iPiv, double *Work, int *WORKL, int *INFO);

#else
extern "C" int dgesv_(int *N, int *NRHS, double *A, int *LDA, int *iPiv, 
		      double *B, int *LDB, int *INFO);

extern "C" int dgetrs_(char *TRANS, int *N, int *NRHS, double *A, int *LDA, 
		       int *iPiv, double *B, int *LDB, int *INFO);		       

extern "C" int dgetrf_(int *M, int *N, double *A, int *LDA, 
		       int *iPiv, int *INFO);

extern "C" int dgetri_(int *N, double *A, int *LDA, 
		       int *iPiv, double *Work, int *WORKL, int *INFO);
extern "C" int dgerfs_(char *TRANS, int *N, int *NRHS, double *A, int *LDA, 
		       double *AF, int *LDAF, int *iPiv, double *B, int *LDB, 
		       double *X, int *LDX, double *FERR, double *BERR, 
		       double *WORK, int *IWORK, int *INFO);

#endif

int XC::Matrix::Solve(const Vector &b, Vector &x) const
{

    int n= numRows;

#ifdef _G3DEBUG    
    if(numRows != numCols) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; the matrix of dimensions " 
	        << numRows << ", " << numCols
		<< " is not square " << std::endl;
      return -1;
    }

    if(n != x.Size()) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; dimension of x, " << numRows
		<< "is not same as matrix " <<  x.Size() << std::endl;
      return -2;
    }

    if(n != b.Size()) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< ";- dimension of x, " << numRows
		<< "is not same as matrix " <<  b.Size() << std::endl;
      return -2;
    }
#endif
    
    // check work area can hold all the data
    const int dataSize= data.Size();
    auxMatrix.resize(dataSize,n);

    double *matrixWork= auxMatrix.getMatrixWork();
    for(int i=0; i<dataSize; i++)
      matrixWork[i]= data(i);

    // set x equal to b
    x= b;

    int nrhs= 1;
    int ldA= n;
    int ldB= n;
    int info;
    double *Aptr= matrixWork;
    double *Xptr= x.theData;
    int *iPIV= auxMatrix.getIntWork();
    

#ifdef _WIN32
    DGESV(&n,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);
#else
    dgesv_(&n,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);
#endif

    

    return 0;
}


int XC::Matrix::Solve(const Matrix &b, Matrix &x) const
  {

    int n= numRows;
    int nrhs= x.numCols;

#ifdef _G3DEBUG    
    if(numRows != numCols) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; the matrix of dimensions ["
		<< numRows << " " <<  numCols << "] is not square\n";
      return -1;
    }

    if(n != x.numRows) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; #rows of X, " << x.numRows
		<< " is not same as the matrices: " << numRows << std::endl;
      return -2;
    }

    if(n != b.numRows) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; #rows of B, " << b.numRows
		<< " is not same as the matrices: " << numRows << std::endl;
      return -2;
    }

    if(x.numCols != b.numCols) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; #cols of B, " << b.numCols
		<< " , is not same as that of X, b " <<  x.numCols << std::endl;
      return -3;
    }
#endif

    // check work area can hold all the data
    const int dataSize= data.Size();
    auxMatrix.resize(dataSize,n);
    
    x= b;

    // copy the data
    double *matrixWork= auxMatrix.getMatrixWork();
    for(int i=0; i<dataSize; i++)
      matrixWork[i]= data(i);


    int ldA= n;
    int ldB= n;
    int info;
    double *Aptr= matrixWork;
    double *Xptr= x.getDataPtr();
    
    int *iPIV= auxMatrix.getIntWork();
    

#ifdef _WIN32
    DGESV(&n,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);
#else
    dgesv_(&n,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);

    /*
    // further correction if required
    double Bptr[n*n];
    for(int i=0; i<n*n; i++) Bptr[i]= b.data(i);
    double *origData= getDataPtr();
    double Ferr[n];
    double Berr[n];
    double newWork[3*n];
    int newIwork[n];
    
    dgerfs_("N",&n,&n,origData,&ldA,Aptr,&n,iPIV,Bptr,&ldB,Xptr,&ldB,
	    Ferr, Berr, newWork, newIwork, &info);
    */
#endif

    return info;
}


int XC::Matrix::Invert(Matrix &theInverse) const
{

    int n= numRows;
    //int nrhs= theInverse.numCols;

#ifdef _G3DEBUG    
    if(numRows != numCols) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; the matrix of dimensions ["
		<< numRows << "," << numCols << "] is not square\n";
      return -1;
    }

    if(n != theInverse.numRows) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; #rows of X, " << numRows
		<< ", is not same as matrix " << theInverse.numRows
		<< std::endl;
      return -2;
    }
#endif
    const int dataSize= data.Size();
    auxMatrix.resize(dataSize,n);
    
    // copy the data
    theInverse= *this;
    
    double *matrixWork= auxMatrix.getMatrixWork();
    for(int i=0; i<dataSize; i++)
      matrixWork[i]= data(i);

    int ldA= n;
    //int ldB= n;
    int info;
    double *Wptr= matrixWork;
    double *Aptr= theInverse.getDataPtr();
    int workSize= auxMatrix.getSizeDoubleWork();
    
    int *iPIV= auxMatrix.getIntWork();
    

    dgetrf_(&n,&n,Aptr,&ldA,iPIV,&info);
    if(info != 0) 
      return info;
    
    dgetri_(&n,Aptr,&ldA,iPIV,Wptr,&workSize,&info);
    
    return info;
}
    
		    

int XC::Matrix::addMatrix(double factThis, const Matrix &other, double factOther)
  {
    if(factThis == 1.0 && factOther == 0.0)
      return 0;

#ifdef _G3DEBUG
    if((other.numRows != numRows) || (other.numCols != numCols)) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; incompatable matrices\n";
      return -1;
    }
#endif
    const int dataSize= data.Size();

    if(factThis == 1.0) {

      // want: this += other * factOther
      double *dblDataPtr= getDataPtr();
      if(factOther == 1.0) {
	const double *otherDataPtr= other.getDataPtr();		    
	for(int i=0; i<dataSize; i++)
	  *dblDataPtr++ += *otherDataPtr++;
      } else {
	const double *otherDataPtr= other.getDataPtr();
	for(int i=0; i<dataSize; i++)
	  *dblDataPtr++ += *otherDataPtr++ * factOther;
      }
    } 

    else if(factThis == 0.0) {

      // want: this= other * factOther
      double *dblDataPtr= getDataPtr();
      if(factOther == 1.0) {
	const double *otherDataPtr= other.getDataPtr();		    
	for(int i=0; i<dataSize; i++)
	  *dblDataPtr++= *otherDataPtr++;
      } else {
	const double *otherDataPtr= other.getDataPtr();		    
	for(int i=0; i<dataSize; i++)
	  *dblDataPtr++= *otherDataPtr++ * factOther;
      }
    } 

    else {

      // want: this= this * thisFact + other * factOther
      double *dblDataPtr= getDataPtr();
      if(factOther == 1.0) {
	const double *otherDataPtr= getDataPtr();
	for(int i=0; i<dataSize; i++)
          {
	    const double value= *dblDataPtr * factThis + *otherDataPtr++;
	    *dblDataPtr++= value;
	  }
      } else {
	const double *otherDataPtr= other.getDataPtr();		    
	for(int i=0; i<dataSize; i++) {
	  double value= *dblDataPtr * factThis + *otherDataPtr++ * factOther;
	  *dblDataPtr++= value;
	}
      }
    } 

    // successfull
    return 0;
}



int XC::Matrix::addMatrixProduct(double thisFact, const Matrix &B, const Matrix &C, double otherFact)
  {
    if(thisFact == 1.0 && otherFact == 0.0)
      return 0;
#ifdef _G3DEBUG
    if((B.numRows != numRows) || (C.numCols != numCols) || (B.numCols != C.numRows)) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; incompatable matrices, this\n";
      return -1;
    }
#endif
    const int dataSize= data.Size();
    // NOTE: looping as per blas3 dgemm_: j,k,i
    if(thisFact == 1.0) {

      // want: this += B * C  otherFact
      const int numColB= B.numCols;
      const double *ckjPtr = &(C.data)[0];
      for(int j=0; j<numCols; j++)
        {
	  double *aijPtrA= &data(j*numRows);
	  for(int k=0; k<numColB; k++)
            {
	      const double tmp= *ckjPtr++ * otherFact;
	      double *aijPtr= aijPtrA;
	      const double *bikPtr= &(B.data)(k*numRows);
	      for(int i=0; i<numRows; i++)
	        *aijPtr++ += *bikPtr++ * tmp;
	    }
        }
    }

    else if(thisFact == 0.0) {

      // want: this= B * C  otherFact
      double *dblDataPtr= data.getDataPtr();
      for(int i=0; i<dataSize; i++)
	  *dblDataPtr++= 0.0;
      const int numColB= B.numCols;
      const double *ckjPtr = &(C.data)(0);
      for(int j=0; j<numCols; j++) {
	double *aijPtrA= &data[j*numRows];
	for(int k=0; k<numColB; k++) {
	  double tmp= *ckjPtr++ * otherFact;
	  double *aijPtr= aijPtrA;
	  const double *bikPtr= &(B.data)(k*numRows);
	  for(int i=0; i<numRows; i++)
	    *aijPtr++ += *bikPtr++ * tmp;
	}
      }
    } 

    else {
      // want: this= B * C  otherFact
      double *dblDataPtr= getDataPtr();
      for(int i=0; i<dataSize; i++)
	*dblDataPtr++ *= thisFact;
      int numColB= B.numCols;
      const double *ckjPtr = &(C.data)(0);
      for(int j=0; j<numCols; j++) {
	double *aijPtrA= &data[j*numRows];
	for(int k=0; k<numColB; k++) {
	  double tmp= *ckjPtr++ * otherFact;
	  double *aijPtr= aijPtrA;
	  const double *bikPtr= &(B.data)(k*numRows);
	  for(int i=0; i<numRows; i++)
	    *aijPtr++ += *bikPtr++ * tmp;
	}
      }
    } 

    return 0;
}


// to perform this += T' * B * T
int XC::Matrix::addMatrixTripleProduct(double thisFact, 
			       const XC::Matrix &T, 
			       const XC::Matrix &B, 
			       double otherFact)
{
    if(thisFact == 1.0 && otherFact == 0.0)
      return 0;
#ifdef _G3DEBUG
    if((numCols != numRows) || (B.numCols != B.numRows) || (T.numCols != numRows) ||
	(T.numRows != B.numCols)) {
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; incompatable matrices\n";
      return -1;
    }
#endif

    // cheack work area can hold the temporary matrix
    int dimB= B.numCols;
    const size_t sizeWork= dimB * numCols;

    if(sizeWork > auxMatrix.getSizeDoubleWork())
      {
        this->addMatrix(thisFact, T^B*T, otherFact);
        return 0;
      }

    // zero out the work area
    double *matrixWorkPtr= auxMatrix.getMatrixWork();
    for(size_t l=0; l<sizeWork; l++)
      *matrixWorkPtr++= 0.0;

    // now form B * T * fact store in matrixWork == A area
    // NOTE: looping as per blas3 dgemm_: j,k,i

    double *matrixWork= auxMatrix.getMatrixWork();
    const double *tkjPtr = &(T.data)(0);
    for(int j=0; j<numCols; j++)
      {
        double *aijPtrA= &matrixWork[j*dimB];
        for(int k=0; k<dimB; k++)
          {
	    double tmp= *tkjPtr++ * otherFact;
	    double *aijPtr= aijPtrA;
	    const double *bikPtr= &(B.data)(k*dimB);
	    for(int i=0; i<dimB; i++) 
	      *aijPtr++ += *bikPtr++ * tmp;
          }
      }

    // now form T' * matrixWork
    // NOTE: looping as per blas3 dgemm_: j,i,k
    if(thisFact == 1.0)
      {
        double *dblDataPtr= &data(0);
        for(int j=0; j< numCols; j++)
          {
	    const double *workkjPtrA= &matrixWork[j*dimB];
            for(int i=0; i<numRows; i++)
              {
	        const double *ckiPtr= &(T.data)(i*dimB);
	        const double *workkjPtr= workkjPtrA;
	        double aij= 0.0;
	        for(int k=0; k< dimB; k++)
	          aij += *ckiPtr++ * *workkjPtr++;
	        *dblDataPtr++ += aij;
	      }
          }
      }
    else if(thisFact == 0.0)
      {
        double *dblDataPtr= &data[0];
        for(int j=0; j< numCols; j++)
          {
	    const double *workkjPtrA= &matrixWork[j*dimB];
	    for(int i=0; i<numRows; i++)
              {
	        const double *ckiPtr= &(T.data)(i*dimB);
	        const double *workkjPtr= workkjPtrA;
	        double aij= 0.0;
	        for(int k=0; k< dimB; k++)
	          aij += *ckiPtr++ * *workkjPtr++;
	        *dblDataPtr++= aij;
	      }
          }
      }
    else
      {
        double *dblDataPtr= &data(0);
        for(int j=0; j< numCols; j++)
          {
	    const double *workkjPtrA= &matrixWork[j*dimB];
	    for(int i=0; i<numRows; i++)
              {
                const double *ckiPtr= &(T.data)(i*dimB);
	        const double *workkjPtr= workkjPtrA;
	        double aij= 0.0;
	        for(int k=0; k< dimB; k++)
	          aij+= *ckiPtr++ * *workkjPtr++;
	        const double value= *dblDataPtr * thisFact + aij;
	        *dblDataPtr++= value;
	      }
          }
      }
    return 0;
  }



//
// OVERLOADED OPERATOR () to CONSTRUCT A NEW_ MATRIX
//

XC::Matrix XC::Matrix::operator()(const ID &rows, const ID & cols) const
  {
    const int nRows= rows.Size();
    const int nCols= cols.Size();
    Matrix result(nRows,nCols);
    double *dblDataPtr= result.getDataPtr();
    for(int i=0; i<nCols; i++)
      for(int j=0; j<nRows; j++)
	*dblDataPtr++= (*this)(rows(j),cols(i));
    return result;
  }

//! @brief Return the fila which index being passed as parameter.
XC::Vector XC::Matrix::getRow(int row) const
  {
    Vector retval(numCols);
    for(int j=0; j<numCols; j++)
      retval(j)= (*this)(row,j);
    return retval;
  }

//! @brief Return the columna which index being passed as parameter.
XC::Vector XC::Matrix::getCol(int col) const
  {
    Vector retval(numRows);
    for(int i=0; i<numRows; i++)
      retval(i)= (*this)(i,col);
    return retval;
  }

		
// Matrix &operator=(const Matrix  &V):
//      the assignment operator, This is assigned to be a copy of V. if sizes
//      are not compatable this.data [] is deleted. The data pointers will not
//      point to the same area in mem after the assignment.
//






// virtual XC::Matrix &operator+=(double fact);
// virtual XC::Matrix &operator-=(double fact);
// virtual XC::Matrix &operator*=(double fact);
// virtual XC::Matrix &operator/=(double fact); 
//	The above methods all modify the current matrix. If in
//	derived matrices data kept in data and of sizeData no redef necessary.

XC::Matrix &XC::Matrix::operator+=(double fact)
  {
    if(fact != 0.0)
      {
        double *dblDataPtr= data.getDataPtr();
        const int dataSize= data.Size();
        for(int i=0; i<dataSize; i++)
          *dblDataPtr++ += fact;
      }
    return *this;
  }




XC::Matrix &XC::Matrix::operator-=(double fact)
  {

    if(fact!= 0.0)
      {
        double *dblDataPtr= data.getDataPtr();
        const int dataSize= data.Size();
        for(int i=0; i<dataSize; i++)
          *dblDataPtr++ -= fact;
      }
    return *this;
  }


XC::Matrix &XC::Matrix::operator*=(double fact)
  {
    if(fact!=1.0)
      {
        double *dblDataPtr= data.getDataPtr();
        const int dataSize= data.Size();
        for(int i=0; i<dataSize; i++)
          *dblDataPtr++ *= fact;
      }
    return *this;
  }

XC::Matrix &XC::Matrix::operator/=(double fact)
  {
    if(fact!=1.0)
      {
        double *dblDataPtr= data.getDataPtr();
        const int dataSize= data.Size();
        if(fact!=0.0)
          {
            const double val= 1.0/fact;
            for(int i=0; i<dataSize; i++)
	      *dblDataPtr++*= val;
          }
        else
          {
            // print out the warining message
            std::cerr << nombre_clase() << "::" << __FUNCTION__
		      << "; WARNING: 0 factor specified all values in Matrix set to ";
            std::cerr << MATRIX_VERY_LARGE_VALUE << std::endl;

            for(int i=0; i<dataSize; i++)
	      *dblDataPtr++= MATRIX_VERY_LARGE_VALUE;
          }
      }
    return *this;
  }


//    virtual Matrix operator+(double fact);
//    virtual Matrix operator-(double fact);
//    virtual Matrix operator*(double fact);
//    virtual Matrix operator/(double fact);
//	The above methods all return a new_ full general matrix.

XC::Matrix XC::Matrix::operator+(double fact) const
  {
    Matrix result(*this);
    result+= fact;
    return result;
  }

XC::Matrix XC::Matrix::operator-(double fact) const
{
    Matrix result(*this);
    result-= fact;
    return result;
}

XC::Matrix XC::Matrix::operator*(double fact) const
  {
    Matrix result(*this);
    result *= fact;
    return result;
  }

XC::Matrix XC::Matrix::operator/(double fact) const
{
    if(fact == 0.0) {
	std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; ERROR divide-by-zero\n";
	exit(0);
    }
    Matrix result(*this);
    result /= fact;
    return result;
}


//
// MATRIX_VECTOR OPERATIONS
//

XC::Vector XC::Matrix::operator*(const Vector &V) const
  {
    Vector result(numRows);
    
    if(V.Size() != numCols)
      {
	std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; incompatable sizes\n";
	return result;
      } 
    
    const double *dblDataPtr= getDataPtr();
    for(int i=0; i<numCols; i++)
      for(int j=0; j<numRows; j++)
	result(j) += *dblDataPtr++ * V(i);

    /*
    std::cerr << "HELLO: " << V;
    for(int i=0; i<numRows; i++) {
	double sum= 0.0;
	for(int j=0; j<numCols; j++) {
	    sum += (*this)(i,j) * V(j);
	    if(i == 9) std::cerr << "sum: " << sum << " " << (*this)(i,j)*V(j) << " " << V(j) << 
;
	}
	result(i) += sum;
    }
    std::cerr << *this;
    std::cerr << "HELLO result: " << result;    
    */

    return result;
  }

XC::Vector XC::Matrix::operator^(const Vector &V) const
  {
    Vector result(numCols);
    
    if(V.Size() != numRows)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; incompatable sizes\n";
        return result;
      } 

    const double *dblDataPtr= getDataPtr();
    for(int i=0; i<numCols; i++)
      for(int j=0; j<numRows; j++)
	result(i) += *dblDataPtr++ * V(j);

    return result;
  }

//! @brief Unary minus operator.
XC::Matrix XC::Matrix::operator-(void) const
  {
    Matrix retval(*this);
    retval.data.operator-();
    return retval;
  }

//
// MATRIX - MATRIX OPERATIONS
//
	    

XC::Matrix XC::Matrix::operator+(const Matrix &M) const
  {
    Matrix result(*this);
    result.addMatrix(1.0,M,1.0);    
    return result;
  }
	    
XC::Matrix XC::Matrix::operator-(const Matrix &M) const
  {
    Matrix result(*this);
    result.addMatrix(1.0,M,-1.0);    
    return result;
  }
	    
    
XC::Matrix XC::Matrix::operator*(const Matrix &M) const
  {
    Matrix result(numRows,M.numCols);
    
    if(numCols != M.numRows || result.numRows != numRows)
      {
	std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; incompatable sizes\n";
	return result;
      } 

    result.addMatrixProduct(0.0, *this, M, 1.0);
    
    /****************************************************
    double *resDataPtr= result.data;	    

    int innerDim= numCols;
    int nCols= result.numCols;
    for(int i=0; i<nCols; i++) {
      double *aStartRowDataPtr= data;
      double *bStartColDataPtr= &(M.data[i*innerDim]);
      for(int j=0; j<numRows; j++) {
	double *bDataPtr= bStartColDataPtr;
	double *aDataPtr= aStartRowDataPtr +j;	    
	double sum= 0.0;
	for(int k=0; k<innerDim; k++) {
	  sum += *aDataPtr * *bDataPtr++;
	  aDataPtr += numRows;
	}
	*resDataPtr++= sum;
      }
    }
    ******************************************************/
    return result;
  }



// Matrix operator^(const Matrix &M) const
//	We overload the * operator to perform matrix^t-matrix multiplication.
//	results= (*this)transposed * M.

XC::Matrix XC::Matrix::operator^(const Matrix &M) const
  {
    Matrix result(numCols,M.numCols);
  
    if(numRows != M.numRows || result.numRows != numCols)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; incompatable sizes\n";
        return result;
      } 

    double *resDataPtr= result.getDataPtr();	    

    int innerDim= numRows;
    int nCols= result.numCols;
    for(int i=0; i<nCols; i++)
      {
        const double *aDataPtr= getDataPtr();
        const double *bStartColDataPtr= &(M.data(i*innerDim));
        for(int j=0; j<numCols; j++)
          {
	    const double *bDataPtr= bStartColDataPtr;
	    double sum= 0.0;
	    for(int k=0; k<innerDim; k++)
              { sum += *aDataPtr++ * *bDataPtr++; }
            *resDataPtr++= sum;
          }
      }
    return result;
  }
    



XC::Matrix &XC::Matrix::operator+=(const Matrix &M)
  {
#ifdef _G3DEBUG
  if(numRows != M.numRows || numCols != M.numCols) {
    std::cerr << nombre_clase() << "::" << __FUNCTION__
	      << "; matrices incompatable\n";
    return *this;
  }
#endif

    double *dblDataPtr= getDataPtr();
    const double *otherData= M.getDataPtr();
    const int dataSize= data.Size();
    for(int i=0; i<dataSize; i++)
      *dblDataPtr++ += *otherData++;
    return *this;
  }

XC::Matrix &XC::Matrix::operator-=(const Matrix &M)
  {
#ifdef _G3DEBUG
  if(numRows != M.numRows || numCols != M.numCols) {
    std::cerr << nombre_clase() << "::" << __FUNCTION__
	      << ";  matrices incompatable [" << numRows << " "
	      << numCols << "]" << "[" << M.numRows << "]"
	      << M.numCols << "]\n";

    return *this;
  }
#endif

    double *dblDataPtr= getDataPtr();
    const double *otherData= M.getDataPtr();
    const int dataSize= data.Size();
    for(int i=0; i<dataSize; i++)
      *dblDataPtr++ -= *otherData++;
    return *this;
  }


//! @brief Output method.
void XC::Matrix::Output(std::ostream &s) const
  {
    const int nr= noRows();
    if(nr>0)
      {
        const int nc= noCols();
        if(nc>0)
          {
            for(int j=0; j<nc; j++)
              s <<  (*this)(0,j) << " ";
            for(int i=1; i<nr; i++)
              {
                s << std::endl;
	        for(int j=0; j<nc; j++)
                  s <<  (*this)(i,j) << " ";
              }
          }
      }
  }

//! @brief Convierte una matriz de tipo m_double otra de tipo Matrix.
XC::Matrix XC::m_double_to_matrix(const m_double &m)
  {
    const size_t fls= m.getNumFilas();
    const size_t cls= m.getNumCols();
    Matrix retval(fls,cls);
    for(register size_t i=1;i<=fls;i++)
      for(register size_t j=1;j<=cls;j++)
        retval(i-1,j-1)= m(i,j);
    return retval;
  }

//! @brief Convierte a una matriz de tipo m_double.
m_double XC::matrix_to_m_double(const Matrix &m)
  {
    const size_t fls= m.noRows();
    const size_t cls= m.noCols();
    m_double retval(fls,cls,0.0);
    for(register size_t i=0;i<fls;i++)
      for(register size_t j=0;j<cls;j++)
        retval(i+1,j+1)= m(i,j);
    return retval;
  }

//! @brief Lectura desde cadena de caracteres.
void XC::Matrix::Input(const std::string &s)
  {
    m_double tmp(1,1);
    tmp.Input(s);
    (*this)= m_double_to_matrix(tmp);
  }


//! @brief Escribe la matriz en un archivo binario.
void XC::Matrix::write(std::ofstream &os)
  {
    os.write((char *) &numRows,sizeof numRows);
    os.write((char *) &numCols,sizeof numCols); 
    data.write(os);
  }

//! @brief Lee la matriz de un archivo binario.
void XC::Matrix::read(std::ifstream &is)
  {
    is.read((char *) &numRows,sizeof numRows);
    is.read((char *) &numCols,sizeof numCols); 
    data.read(is);
  }

//! @brief Returns a string that represents the matrix.
std::string to_string(const XC::Matrix &V)
  {
    //Doing this way clients will be able to manage the formatting
    //with things like 'std::scientific << std::setprecision(10)' 
    std::ostringstream ss; 
    ss << V;
    return ss.str();
  }

//! @brief Operador salida.
std::ostream &XC::operator<<(std::ostream &s, const Matrix &m)
  {
    m.Output(s);
    return s;
  }

	
/****************	
istream &operator>>(istream &s, Matrix &V)
{
    V.Input(s);
    return s;
}
****************/

 //! @brief Assemble
int XC::Matrix::Assemble(const Matrix &V, int init_row, int init_col, double fact) 
{
  int pos_Rows, pos_Cols;
  int res= 0;
  
  int VnumRows= V.numRows;
  int VnumCols= V.numCols;
  
  int final_row= init_row + VnumRows - 1;
  int final_col= init_col + VnumCols - 1;
  
  if((init_row >= 0) && (final_row < numRows) && (init_col >= 0) && (final_col < numCols))
  {
     for(int i=0; i<VnumCols; i++) 
     {
        pos_Cols= init_col + i;
        for(int j=0; j<VnumRows; j++) 
        {
           pos_Rows= init_row + j;
      
	   (*this)(pos_Rows,pos_Cols) += V(j,i)*fact;
        }
     }
  }  
  else 
  {
     std::cerr << nombre_clase() << "::" << __FUNCTION__
	       << "; WARNING: position outside bounds \n";
     res= -1;
  }

  return res;
}

//! @brief Return the transpuesta.
XC::Matrix XC::Matrix::getTrn(void) const
  {
    Matrix retval(numCols,numRows);
    for(register int i=0;i<numRows;i++)
      for(register int j=0;j<numCols;j++)
        retval(j,i)= (*this)(i,j);
    return retval;
  }

int XC::Matrix::AssembleTranspose(const Matrix &V, int init_row, int init_col, double fact) 
{
  int pos_Rows, pos_Cols;
  int res= 0;
  
  int VnumRows= V.numRows;
  int VnumCols= V.numCols;
  
  int final_row= init_row + VnumCols - 1;
  int final_col= init_col + VnumRows - 1;
  
  if((init_row >= 0) && (final_row < numRows) && (init_col >= 0) && (final_col < numCols))
  {
     for(int i=0; i<VnumRows; i++) 
     {
        pos_Cols= init_col + i;
        for(int j=0; j<VnumCols; j++) 
        {
           pos_Rows= init_row + j;
      
	   (*this)(pos_Rows,pos_Cols) += V(i,j)*fact;
        }
     }
  }  
  else 
  {
     std::cerr << nombre_clase() << "::" << __FUNCTION__
	       << "; WARNING: position outside bounds \n";
     res= -1;
  }

  return res;
}




int XC::Matrix::Extract(const Matrix &V, int init_row, int init_col, double fact) 
  {
  int pos_Rows, pos_Cols;
  int res= 0;
  
  int VnumRows= V.numRows;
  int VnumCols= V.numCols;
  
  int final_row= init_row + numRows - 1;
  int final_col= init_col + numCols - 1;
  
  if((init_row >= 0) && (final_row < VnumRows) && (init_col >= 0) && (final_col < VnumCols))
  {
     for(int i=0; i<numCols; i++) 
     {
        pos_Cols= init_col + i;
        for(int j=0; j<numRows; j++) 
        {
           pos_Rows= init_row + j;
      
	   (*this)(j,i)= V(pos_Rows,pos_Cols)*fact;
        }
     }
  }  
  else 
  {
     std::cerr << nombre_clase() << "::" << __FUNCTION__
	       << "; WARNING: position outside bounds \n";
     res= -1;
  }

  return res;
}


XC::Matrix XC::operator*(double a, const Matrix &V)
  { return V * a; }

// //! @brief Convierte en matriz la cadena de caracteres being passed as parameter.
// void XC::Matrix::from_string(const std::string &str)
//   {
//     MatrizAny tmp= interpretaMatrizAny(str);
//     if(!tmp.empty())
//       {
//         const size_t nfilas= tmp.getNumFilas(); //Número de filas.
//         const size_t ncols= tmp.getNumCols(); //Número de columnas.
//         resize(nfilas,ncols);
//         for(size_t i= 0;i<nfilas;i++)
//           for(size_t j= 0;j<ncols;j++)
//             XC::Matrix::operator()(i,j)= convert_to_double(tmp(i+1,j+1));
//       }
//   }

// //! @brief Return the matriz que resulta de interpretar la cadena de caracteres being passed as parameter.
// XC::Matrix XC::interpreta_xc_matriz(const std::string &str)
//   {
//     Matrix retval(1,1);
//     retval.from_string(str);
//     return retval;
//   }

XC::Matrix XC::identity(const Matrix &m)
  {
    Matrix retval(m);
    retval.Identity();
    return retval; 
  }

//! @brief Returns the sum of the i-th row.
double XC::Matrix::rowSum(int i) const
  {
    double sumaf= (*this)(i,0);
    for(register int j=1;j<this->noCols();j++)
      sumaf+= (*this)(i,j);
    return sumaf;
  }

//! @brief Returns the sum of the j-th row.
double XC::Matrix::columnSum(int j) const
  {
    double sumac= (*this)(0,j);
    for(register int i=1;i<this->noRows();i++)
      sumac+= (*this)(i,j);
    return sumac;
  }

//! @brief Returns the squared modulus (euclidean norm) of the matrix.
double XC::Matrix::Norm2(void) const
  {
    double r= 0.0;
    int ncols= this->noCols();
    int nrows= this->noRows();
    if(ncols==nrows)
      {
        const Matrix trn= transposed(*this);
        const Matrix prod= (*this)*trn;
        nrows= prod.noRows();
        for(int i=0;i<nrows;i++)
          r+= prod(i,i);
      }
    else
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; non-square matrix." << std::endl;
    return r;
  }



//! @brief Returns the modulus (euclidean norm) of the matrix.
double XC::Matrix::Norm(void) const
  { return sqrt(Norm2()); }

//! @brief Returns the maximum value of the elements of the vector
//! that results that contains the row sums.
double XC::Matrix::rowNorm(void) const
  {
    double maximo= rowSum(0);
    for(register int i=1;i<this->noRows();i++) maximo= std::max(maximo,rowSum(i));
    return maximo;
  }

//! @brief Returns the maximum value of the elements of the vector
//! that results that contains the column sums.
double XC::Matrix::columnNorm(void) const
  {
    double maximo= columnSum(0);
    for(register int j=1;j<this->noCols();j++) maximo= std::max(maximo,columnSum(j));
    return maximo;
  }
