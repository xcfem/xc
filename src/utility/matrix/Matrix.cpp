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
#include "xc_utils/src/matrices/m_double.h"

#include "AuxMatrix.h"

#define MATRIX_WORK_AREA 400
#define INT_WORK_AREA 20

XC::AuxMatrix XC::Matrix::auxMatrix(MATRIX_WORK_AREA,INT_WORK_AREA);
double XC::Matrix::MATRIX_NOT_VALID_ENTRY =0.0;
int XC::Matrix::sizeDoubleWork= MATRIX_WORK_AREA;
int XC::Matrix::sizeIntWork= INT_WORK_AREA;
double *XC::Matrix::matrixWork= nullptr;
int *XC::Matrix::intWork= nullptr;

//! @brief Default constructor.
XC::Matrix::Matrix(void)
  :numRows(0), numCols(0) {}


//! @brief Constructor.
//!
//! To construct a Matrix with nRows and numberOfColumns and all coefficients equal
//! to \f$0\f$. Allocates memory for the storage of the data. If numRows
//! \f$<\f$ \f$0\f$, numCols \f$<\f$ \f$0\f$ or not enough memory is available
//! available, an error message is printed and a Matrix with numRows = \f$0\f$
//! and numCols = \f$0\f$ is returned. Before the Matrix is returned, Zero() is
//! called. 
//!
//! @param nRows: number of matrix rows.
//! @param numberOfColumns: number of matrix columns.
XC::Matrix::Matrix(int nRows,int nCols)
  : numRows(nRows), numCols(nCols), data(nRows*nCols)
  {
#ifdef _G3DEBUG
    if(nRows < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: tried to init matrix ";
        std::cerr << "with num rows: " << nRows << " <0\n";
        numRows= 0; numCols =0;
      }
    if(nCols < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: tried to init matrix";
        std::cerr << "with num cols: " << nCols << " <0\n";
        numRows= 0; numCols =0;
      }
#endif
    data.Zero();
  }

//! @brief Constructor. Wrapper for values contained in theData.
//!
//! To construct a Matrix with nRow and nCols. The memory for storage of the
//! data is found at the location pointed to by \p data. Note that this memory
//! must have been previously allocated and it must be of appropriate size,
//! erroneous results and segmentation faults can occur otherwise. No
//! additional memory is allocated for the storage of the data. 
//!
//! @param theData: values.
//! @param nRows: number of matrix rows.
//! @param nCols: number of matrix columns.
XC::Matrix::Matrix(double *theData, int nRows, int nCols) 
  :numRows(nRows),numCols(nCols), data(theData,nRows*nCols)
  {
#ifdef _G3DEBUG
    if(nRows < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: tried to init matrix with numRows: ";
        std::cerr << nRows << " <0\n";
        numRows= 0; numCols =0;
      }
    if(nCols < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: tried to init matrix with numCols: ";
        std::cerr << nCols << " <0\n";
        numRows= 0; numCols =0;
      }    
#endif
  }

//! @brief Constructor (Python interface).
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

//! @brief Zero's out the Matrix.
//!
//! Zero's out the Matrix, i.e. sets all the components of the matrix to
//! \f$0\f$.
void XC::Matrix::Zero(void)
  { data.Zero(); }

void XC::Matrix::Identity(void)
  {
    Zero();
    if(noRows()!=noCols())
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; not an square matrix." << std::endl;
    const int n= std::min(noRows(),noCols());
    for(int i=0;i<n;i++)
      (*this)(i,i)= 1.0;
  }

int XC::Matrix::resize(int n_rows, int n_columns)
  {

    const int newSize= n_rows*n_columns;

    if(newSize<0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; rows " << n_rows << " or cols " << n_columns
		  << " specified <= 0\n";
        return -1;
      }
    else if(newSize == 0)
      {
        data.resize(0);
        numRows= n_rows;
        numCols= n_columns;
      }
    else if(newSize > data.Size())
      {
        data.resize(newSize);
        numRows= n_rows;
        numCols= n_columns;
      }
    else
      {
        // just reset the cols and rows - save two memory calls at expense of holding 
        // onto extra memory
        numRows= n_rows;
        numCols= n_columns;
      }
    return 0;
  }

//! @brief Assembles the argument into the current Matrix.
//!
//! Assembles into the current Matrix the Matrix \p M. The contents of the
//! current matrix at location ({\em rows(i),cols(j)}) is set equal to the
//! current value plus \p fact times the value of the matrix \p M
//! at location ({\em i,j }). A warning is printed for every {\em rows(i),
//! cols(j)} specified which is outside the bounds of the matrix.
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
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; WARNING: position (" << pos_Rows
			  << "," << pos_Cols << ") outside bounds \n";
	        res= -1;
              }
          }
      }
    return res;
  }

extern "C" int dgesv_(int *N, int *NRHS, double *A, int *LDA, int *iPiv, 
		      double *B, int *LDB, int *INFO);

extern "C" int dgetrs_(char *TRANS, int *N, int *NRHS, double *A, int *LDA, 
		       int *iPiv, double *B, int *LDB, int *INFO);		       

extern "C" int dgetrf_(int *M, int *N, double *A, int *LDA, 
		       int *iPiv, int *INFO);

extern "C" double dlange_(const char *norm, const int *m, const int *n,
			  const double *a, const int *lda, double *work,
			  const int norm_len);

extern "C" int dgecon_(const char *norm, const int *n, double *a,
		       const int *lda, const double *anorm, double *rcond,
		       double *work, int *iwork, int *info, int len);

extern "C" int dgetri_(int *N, double *A, int *LDA, 
		       int *iPiv, double *Work, int *WORKL, int *INFO);
extern "C" int dgerfs_(char *TRANS, int *N, int *NRHS, double *A, int *LDA, 
		       double *AF, int *LDAF, int *iPiv, double *B, int *LDB, 
		       double *X, int *LDX, double *FERR, double *BERR, 
		       double *WORK, int *IWORK, int *INFO);


//! @brief Solve the equation {\em \f$Ax=V\f$} for the Vector \p x, which is
//! returned.
//!
//! Will solve the equation {\em \f$Ax=V\f$} for the Vector \p x, which is
//! returned. At the moment the current matrix is assumed to be symmetric
//! positive definite. THIS IS TO CHANGE. A Vector is created using
//! \p V. If this Vector is not of the correct size or if an error occurs
//! during factorization a warning message is printed and the Vector \p x
//! is returned. 
int XC::Matrix::Solve(const Vector &b, Vector &x) const
  {

    int n= numRows;

#ifdef _G3DEBUG    
    if(numRows != numCols)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	  	  << "; the matrix of dimensions " 
	          << numRows << ", " << numCols
		<< " is not square " << std::endl;
        return -1;
      }

    if(n != x.Size())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; dimension of x, " << numRows
		  << "is not same as matrix " <<  x.Size() << std::endl;
        return -2;
      }

    if(n != b.Size())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
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
    double *Aptr= matrixWork;
    double *Xptr= x.theData;
    int *iPIV= auxMatrix.getIntWork();
    

    int info;
    dgesv_(&n,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);

    return 0;
  }


int XC::Matrix::Solve(const Matrix &b, Matrix &x) const
  {

    int n= numRows;
    int nrhs= x.numCols;

#ifdef _G3DEBUG    
    if(numRows != numCols)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the matrix of dimensions ["
		  << numRows << " " <<  numCols << "] is not square\n";
        return -1;
      }

    if(n != x.numRows)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; #rows of X, " << x.numRows
		  << " is not same as the matrices: " << numRows << std::endl;
        return -2;
      }

    if(n != b.numRows)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; #rows of B, " << b.numRows
		  << " is not same as the matrices: " << numRows << std::endl;
        return -2;
      }

    if(x.numCols != b.numCols)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; #cols of B, " << b.numCols
		  << " , is not same as that of X, b " <<  x.numCols
		  << std::endl;
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

//! @brief Return the inverse matrix in the argument.
int XC::Matrix::Invert(Matrix &theInverse) const
  {

    int n= numRows;
    //int nrhs= theInverse.numCols;

#ifdef _G3DEBUG    
    if(numRows != numCols)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the matrix of dimensions ["
		  << numRows << "," << numCols << "] is not square\n";
        return -1;
      }

    if(n != theInverse.numRows)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
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
    
XC::Matrix XC::Matrix::getInverse(void) const
  {
    Matrix retval(*this);
    Invert(retval);
    return retval;
  }     	    

//! @brief Add a factor \p fact times the Matrix \p other to the current
//! Matrix.
//!
//! To add a factor \p fact times the Matrix \p other to the current
//! Matrix. The size of the other Matrix is first checked to see sizes are 
//! compatible; if size are not compatible nothing is done and a warning
//! message is printed. The method tests for the type of \p other, to see
//! whether the performance can be improved by avoiding having to call
//! \p other's overloaded (i,j) operators, if \p other is of type
//! genMatrix. 
int XC::Matrix::addMatrix(double factThis, const Matrix &other, double factOther)
  {
    if(factThis == 1.0 && factOther == 0.0)
      return 0;

#ifdef _G3DEBUG
    if((other.numRows != numRows) || (other.numCols != numCols)) {
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; incompatible matrices\n";
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

    // successful
    return 0;
}



int XC::Matrix::addMatrixProduct(double thisFact, const Matrix &B, const Matrix &C, double otherFact)
  {
    if(thisFact == 1.0 && otherFact == 0.0)
      return 0;
#ifdef _G3DEBUG
    if((B.numRows != numRows) || (C.numCols != numCols) || (B.numCols != C.numRows)) {
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; incompatible matrices, this\n";
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

int XC::Matrix::addMatrixTransposeProduct(double thisFact, 
				  const Matrix &B, 
				  const Matrix &C, 
				  double otherFact)
  {
  if (thisFact == 1.0 && otherFact == 0.0)
    return 0;

#ifdef _G3DEBUG
  if ((B.numCols != numRows) || (C.numCols != numCols) || (B.numRows != C.numRows)) {
    opserr << "Matrix::addMatrixProduct(): incompatible matrices, this\n";
    return -1;
  }
#endif

  if (thisFact == 1.0) {
    int numMults = C.numRows;
    double *aijPtr = getDataPtr();
    for (int j=0; j<numCols; j++) {
      for (int i=0; i<numRows; i++) {
	const double *bkiPtr  = &(B.data)[i*numMults];
	const double *cjkPtr  = &(C.data)[j*numMults];
	double sum = 0.0;
	for (int k=0; k<numMults; k++) {
	  sum += *bkiPtr++ * *cjkPtr++;
	}
	*aijPtr++ += sum * otherFact;
      }
    } 
  } else if (thisFact == 0.0) {
    int numMults = C.numRows;
    double *aijPtr = getDataPtr();
    for (int j=0; j<numCols; j++) {
      for (int i=0; i<numRows; i++) {
	const double *bkiPtr  = &(B.data)[i*numMults];
	const double *cjkPtr  = &(C.data)[j*numMults];
	double sum = 0.0;
	for (int k=0; k<numMults; k++) {
	  sum += *bkiPtr++ * *cjkPtr++;
	}
	*aijPtr++ = sum * otherFact;
      }
    } 
  } else {
    int numMults = C.numRows;
    double *aijPtr = getDataPtr();
    for (int j=0; j<numCols; j++) {
      for (int i=0; i<numRows; i++) {
	const double *bkiPtr  = &(B.data)[i*numMults];
	const double *cjkPtr  = &(C.data)[j*numMults];
	double sum = 0.0;
	for (int k=0; k<numMults; k++) {
	  sum += *bkiPtr++ * *cjkPtr++;
	}
	*aijPtr = *aijPtr * thisFact + sum * otherFact;
	aijPtr++;
      }
    } 
  }

  return 0;
}

//! @brief to perform this += T' * B * T
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
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; incompatible matrices\n";
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

//! @brief to perform this += At * B * C
int XC::Matrix::addMatrixTripleProduct(double thisFact, 
			       const Matrix &A, 
			       const Matrix &B,
			       const Matrix &C,
			       double otherFact)
  {
    if (thisFact == 1.0 && otherFact == 0.0)
      return 0;
#ifdef _G3DEBUG
    if ((numRows != A.numRows) || (A.numCols != B.numRows) || (B.numCols != C.numRows) ||
	(C.numCols != numCols)) {
      std::cerr << "Matrix::addMatrixTripleProduct() - incompatible matrices\n";
      return -1;
    }
#endif

    // cheack work area can hold the temporary matrix
    int sizeWork = B.numRows * numCols;

    if (sizeWork > sizeDoubleWork) {
      this->addMatrix(thisFact, A^B*C, otherFact);
      return 0;
    }

    // zero out the work area
    double *matrixWorkPtr = matrixWork;
    for (int l=0; l<sizeWork; l++)
      *matrixWorkPtr++ = 0.0;

    // now form B * C * fact store in matrixWork == A area
    // NOTE: looping as per blas3 dgemm_: j,k,i
    
    int rowsB = B.numRows;
    const double *ckjPtr  = &(C.data)[0];
    for (int j=0; j<numCols; j++) {
      double *aijPtrA = &matrixWork[j*rowsB];
      for (int k=0; k<rowsB; k++) {
	double tmp = *ckjPtr++ * otherFact;
	double *aijPtr = aijPtrA;
	const double *bikPtr = &(B.data)[k*rowsB];
	for (int i=0; i<rowsB; i++) 
	  *aijPtr++ += *bikPtr++ * tmp;
      }
    }

    // now form A' * matrixWork
    // NOTE: looping as per blas3 dgemm_: j,i,k
    int dimB = rowsB;
    if (thisFact == 1.0) {
      double *dataPtr = &data[0];
      for (int j=0; j< numCols; j++) {
	double *workkjPtrA = &matrixWork[j*dimB];
	for (int i=0; i<numRows; i++) {
	  const double *akiPtr = &(A.data)[i*dimB];
	  double *workkjPtr = workkjPtrA;
	  double aij = 0.0;
	  for (int k=0; k< dimB; k++)
	    aij += *akiPtr++ * *workkjPtr++;
	  *dataPtr++ += aij;
	}
      }
    } else if (thisFact == 0.0) {
      double *dataPtr = &data[0];
      for (int j=0; j< numCols; j++) {
	double *workkjPtrA = &matrixWork[j*dimB];
	for (int i=0; i<numRows; i++) {
	  const double *akiPtr = &(A.data)[i*dimB];
	  double *workkjPtr = workkjPtrA;
	  double aij = 0.0;
	  for (int k=0; k< dimB; k++)
	    aij += *akiPtr++ * *workkjPtr++;
	  *dataPtr++ = aij;
	}
      }

    } else {
      double *dataPtr = &data[0];
      for (int j=0; j< numCols; j++) {
	double *workkjPtrA = &matrixWork[j*dimB];
	for (int i=0; i<numRows; i++) {
	  const double *akiPtr = &(A.data)[i*dimB];
	  double *workkjPtr = workkjPtrA;
	  double aij = 0.0;
	  for (int k=0; k< dimB; k++)
	    aij += *akiPtr++ * *workkjPtr++;
	  double value = *dataPtr * thisFact + aij;
	  *dataPtr++ = value;
	}
      }
    }

    return 0;
}


//
// OVERLOADED OPERATOR () to CONSTRUCT A NEW_ MATRIX
//

//! Returns a new Matrix of dimension (rows.Size(), {\em
//! cols.Size()}). The contents of the new matrix are given by the
//! contents of the current matrix at the locations given by the {\em
//! rows} and \p cols objects. For example the contents of the new
//! matrix at location ({\em i,j}) are equal to the contents of the
//! current matrix at location ({\em rows(i),cols(j)}). Assumes ({\em
//! row,col}) is a valid location in the Matrix, a segmentation fault or
//! erroneous results can occur if this is not the case.
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

//! @brief Return the row which index being passed as parameter.
XC::Vector XC::Matrix::getRow(int row) const
  {
    Vector retval(numCols);
    for(int j=0; j<numCols; j++)
      retval(j)= (*this)(row,j);
    return retval;
  }

//! @brief Return the column which index being passed as parameter.
XC::Vector XC::Matrix::getCol(int col) const
  {
    Vector retval(numRows);
    for(int i=0; i<numRows; i++)
      retval(i)= (*this)(i,col);
    return retval;
  }

		
// Matrix &operator=(const Matrix  &V):
//      the assignment operator, This is assigned to be a copy of V. if sizes
//      are not compatible this.data [] is deleted. The data pointers will not
//      point to the same area in mem after the assignment.
//






// virtual XC::Matrix &operator+=(double fact);
// virtual XC::Matrix &operator-=(double fact);
// virtual XC::Matrix &operator*=(double fact);
// virtual XC::Matrix &operator/=(double fact); 
//	The above methods all modify the current matrix. If in
//	derived matrices data kept in data and of sizeData no redef necessary.

//! @brief A method to add \p fact to each component of the current Matrix.
//! 
//! A method to add \p fact to each component of the current Matrix. 
//! The method tests for the type of the current Matrix, to see whether
//! the performance can be improved by avoiding having to call the
//! overloaded (i,j) operators, if the current Matrix is of type
//! genMatrix. 
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

//! @brief A method to subtract \p fact from each component of the current
//! Matrix.
//!
//! A method to subtract \p fact from each component of the current Matrix. 
//! The method tests for the type of the current Matrix, to see whether
//! the performance can be improved by avoiding having to call the
//! overloaded (i,j) operators, if the current Matrix is of type
//! genMatrix. 
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

//! @brief A method to multiply each component of the current Matrix by \p fact.
//! 
//! A method to multiply each component of the current Matrix by \p fact. 
//! The method tests for the type of the current Matrix, to see whether
//! the performance can be improved by avoiding having to call the
//! overloaded (i,j) operators, if the current Matrix is of type
//! genMatrix. 
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

//! @brief A method which will divide each component of the current Matrix
//! by \p fact.
//!
//! A method which will divide each component of the current Matrix
//! by \p fact. If \p fact is equal to zero, an error message is
//! printed and the contents of the Matrix are set to
//! MATRIX\_VERY\_LARGE\_VALUE (defined in \f$<\f$Matrix.h\f$>\f$). The method
//! tests for the type of the current Matrix, to see whether the
//! performance can be improved by avoiding having to call the overloaded
//! (i,j) operators, if the current Matrix is of type genMatrix. 
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
            std::cerr << getClassName() << "::" << __FUNCTION__
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

//! @brief A method to return a new Matrix, whose components are equal to the
//! components of the current Matrix plus the value \p fact.
//!
//! A method to return a new Matrix, whose components are equal to the
//! components of the current Matrix plus the value \p fact. A new
//! Matrix object is constructed, using the current Matrix as the
//! argument to the constructor. The {\em +=} operator is then invoked 
//! on this Matrix with \p fact as the argument, and the new Matrix is
//! then returned. 
XC::Matrix XC::Matrix::operator+(double fact) const
  {
    Matrix result(*this);
    result+= fact;
    return result;
  }

//! @brief A method to return a new Matrix, whose components are equal to the
//! components of the current Matrix minus the value \p fact.
//!
//! A method to return a new Matrix, whose components are equal to the
//! components of the current Matrix minus the value \p fact. A new
//! Matrix object is constructed, using the current Matrix as the
//! argument to the constructor. The {\em -=} operator is then invoked 
//! on this Matrix with \p fact as the argument, and this new Matrix is
//! then returned. 
XC::Matrix XC::Matrix::operator-(double fact) const
  {
    Matrix result(*this);
    result-= fact;
    return result;
  }

//! @brief A method to return a new Matrix, whose components are equal to the
//! components of the current Matrix times the value \p fact.
//!
//! A method to return a new Matrix, whose components are equal to the
//! components of the current Matrix times the value \p fact. A new
//! Matrix object is constructed, using the current Matrix as the
//! argument to the constructor. The {\em *=} operator is then invoked 
//! on this Matrix with \p fact as the argument, and this new Matrix is
//! then returned. 
XC::Matrix XC::Matrix::operator*(double fact) const
  {
    Matrix result(*this);
    result *= fact;
    return result;
  }

//! @brief A method to return a new Matrix whose components are equal to the
//! components of the current Matrix divided the value \p fact.
//!
//! A method to return a new Matrix whose components are equal to the
//! components of the current Matrix divided the value \p fact. A new
//! Matrix object is constructed by using the current Matrix as the
//! argument to the constructor. The {\em /=} operator is then invoked 
//! on this Matrix with \p fact as the argument, and this new Matrix is
//! then returned. 
XC::Matrix XC::Matrix::operator/(double fact) const
  {
    if(fact == 0.0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ERROR divide-by-zero\n";
	exit(0);
      }
    Matrix result(*this);
    result/= fact;
    return result;
  }


//
// MATRIX_VECTOR OPERATIONS
//

//! @brief A method to return a new Vector, of size numRows, whose components
//! are equal to the product of the current Matrix times the Vector {\em
//! V}.
//!
//! A method to return a new Vector, of size numRows, whose components are
//! equal to the product of the current Matrix times the Vector {\em
//! V}. If the current Matrix and Vector \p V are not compatible,
//! i.e. V.Size() is not equal to numCols, an error message is printed and 
//! a zero Vector of size equal to the number of rows in the current
//! Matrix is returned. The method tests for the type of the current
//! Matrix, to see whether the performance can be improved by avoiding
//! having to call the overloaded (i,j) operators, if the current Matrix
//! is of type genMatrix. 
XC::Vector XC::Matrix::operator*(const Vector &V) const
  {
    Vector result(numRows);
    
    if(V.Size() != numCols)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; incompatible sizes\n";
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

//! @brief A method to return a new Vector, of size numCols, whose components
//! are equal to the product of the transpose of the current Matrix times the
//! Vector \p V.
//!
//! A method to return a new Vector, of size numCols, whose components are
//! equal to the product of the transpose of the current Matrix times the
//! Vector \p V. If the current Matrix and Vector \p V are not
//! compatible, i.e. V.Size() is not equal to numRows, an error message is
//! printed and a zero Vector of size equal to the number of columns in
//! the current Matrix is returned. The method tests for the type of the
//! current Matrix, to see whether the performance can be improved by
//! avoiding having to call the overloaded (i,j) operators, if the current
//! Matrix is of type genMatrix. 
XC::Vector XC::Matrix::operator^(const Vector &V) const
  {
    Vector result(numCols);
    
    if(V.Size() != numRows)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; incompatible sizes\n";
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
	    

//! @brief A method to return a new Matrix equal to the sum of the current
//! Matrix and the Matrix \p M.
//!
//! A method to return a new Matrix equal to the sum of the current Matrix
//! and the Matrix \p M. It does this by creating a new matrix passing
//! itself as an argument to the constructor. The addMatrix() method
//! is then invoked on this new Matrix with \f$M\f$ and \f$-1\f$ as the
//! arguments. The new Matrix is then returned. 
XC::Matrix XC::Matrix::operator+(const Matrix &M) const
  {
    Matrix result(*this);
    result.addMatrix(1.0,M,1.0);    
    return result;
  }
	    
//! @brief A method to return a new Matrix equal to the the current Matrix minus
//! the Matrix \p M.
//!
//! A method to return a new Matrix equal to the the current Matrix minus
//! the Matrix \p M. It does this by creating a new matrix passing
//! itself as an argument to the constructor. The addMatrix() method
//! is then invoked on this new Matrix with \f$M\f$ and \f$-1\f$ as the
//! arguments. The new Matrix is then returned. 
XC::Matrix XC::Matrix::operator-(const Matrix &M) const
  {
    Matrix result(*this);
    result.addMatrix(1.0,M,-1.0);    
    return result;
  }
	    
    
//! @brief A method to return a new Matrix equal to the product of the current
//! Matrix and the Matrix \p M.
//! A method to return a new Matrix equal to the product of the current
//! Matrix and the Matrix \p M. It does this by first creating a new
//! Matrix of size numRows and M.numCols. The contents of this new Matrix
//! are then determined and the resulting Matrix is returned. If the two
//! matrices are of incompatible sizes, a warning message is printed and a
//! zeroed Matrix is returned. The method tests for the type of the
//! current Matrix, to see whether the performance can be improved by
//! avoiding having to call the overloaded (i,j) operators, if \p M 
//! is of type genMatrix.
XC::Matrix XC::Matrix::operator*(const Matrix &M) const
  {
    Matrix result(numRows,M.numCols);
    
    if(numCols != M.numRows || result.numRows != numRows)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; incompatible sizes\n";
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

//! @brief A method to return a new Matrix equal to the product of the transpose
//! of the current Matrix and the Matrix \p M.
//! 
//! A method to return a new Matrix equal to the product of the transpose
//! of the current Matrix and the Matrix \p M. It does this by first
//! creating a new Matrix of size numRows and M.numRows. The contents of
//! this new Matrix are then determined and the resulting Matrix is
//! returned. If the two matrices are of incompatible sizes, a warning
//! message is printed and a zeroed Matrix is returned. The method tests
//! for the type of the current Matrix, to see whether the performance can
//! be improved by avoiding having to call the overloaded (i,j) operators,
//! if \p M is of type genMatrix.
XC::Matrix XC::Matrix::operator^(const Matrix &M) const
  {
    Matrix result(numCols,M.numCols);
  
    if(numRows != M.numRows || result.numRows != numCols)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; incompatible sizes\n";
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
    
//! @brief += operator.
XC::Matrix &XC::Matrix::operator+=(const Matrix &M)
  {
#ifdef _G3DEBUG
  if(numRows != M.numRows || numCols != M.numCols)
    {
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; matrices incompatible\n";
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

//! @brief -= operator.
XC::Matrix &XC::Matrix::operator-=(const Matrix &M)
  {
#ifdef _G3DEBUG
  if(numRows != M.numRows || numCols != M.numCols)
    {
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ";  matrices incompatible [" << numRows << " "
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


//! @brief Write to the argument stream.
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

//! @brief Converts a matrix of type m_double into other of type Matrix.
XC::Matrix XC::m_double_to_matrix(const m_double &m)
  {
    const size_t fls= m.getNumberOfRows();
    const size_t cls= m.getNumberOfColumns();
    Matrix retval(fls,cls);
    for( size_t i=1;i<=fls;i++)
      for( size_t j=1;j<=cls;j++)
        retval(i-1,j-1)= m(i,j);
    return retval;
  }

//! @brief Converts a Matrix into an m_double.
m_double XC::matrix_to_m_double(const Matrix &m)
  {
    const size_t fls= m.noRows();
    const size_t cls= m.noCols();
    m_double retval(fls,cls,0.0);
    for( size_t i=0;i<fls;i++)
      for( size_t j=0;j<cls;j++)
        retval(i+1,j+1)= m(i,j);
    return retval;
  }

//! @brief Read from string.
void XC::Matrix::Input(const std::string &s)
  {
    m_double tmp(1,1);
    tmp.Input(s);
    (*this)= m_double_to_matrix(tmp);
  }


//! @brief Write to a binary file.
void XC::Matrix::write(std::ofstream &os)
  {
    os.write((char *) &numRows,sizeof numRows);
    os.write((char *) &numCols,sizeof numCols); 
    data.write(os);
  }

//! @brief Read from a bynary file.
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

//! @brief Output operator.
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
     std::cerr << getClassName() << "::" << __FUNCTION__
	       << "; WARNING: position outside bounds \n";
     res= -1;
  }

  return res;
}

//! @brief Return the transpuesta.
XC::Matrix XC::Matrix::getTrn(void) const
  {
    Matrix retval(numCols,numRows);
    for(int i=0;i<numRows;i++)
      for(int j=0;j<numCols;j++)
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
     std::cerr << getClassName() << "::" << __FUNCTION__
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
     std::cerr << getClassName() << "::" << __FUNCTION__
	       << "; WARNING: position outside bounds \n";
     res= -1;
  }

  return res;
}


XC::Matrix XC::operator*(double a, const Matrix &V)
  { return V * a; }

// //! @brief Convers the text string argument into a matrix.
// void XC::Matrix::from_string(const std::string &str)
//   {
//     MatrixAny tmp= interpretaMatrixAny(str);
//     if(!tmp.empty())
//       {
//         const size_t numberOfRows= tmp.getNumberOfRows(); //number of rows.
//         const size_t nCols= tmp.getNumberOfColumns(); //number of columns.
//         resize(numberOfRows,nCols);
//         for(size_t i= 0;i<numberOfRows;i++)
//           for(size_t j= 0;j<nCols;j++)
//             XC::Matrix::operator()(i,j)= convert_to_double(tmp(i+1,j+1));
//       }
//   }

// //! @brief Return the matrix from the text string argument.
// XC::Matrix XC::interpreta_xc_matrix(const std::string &str)
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
    for(int j=1;j<this->noCols();j++)
      sumaf+= (*this)(i,j);
    return sumaf;
  }

//! @brief Returns the sum of the j-th row.
double XC::Matrix::columnSum(int j) const
  {
    double sumac= (*this)(0,j);
    for(int i=1;i<this->noRows();i++)
      sumac+= (*this)(i,j);
    return sumac;
  }

//! @brief Returns the squared modulus (euclidean norm) of the matrix.
double XC::Matrix::Norm2(void) const
  {
    double r= 0.0;
    int nCols= this->noCols();
    int nRows= this->noRows();
    if(nCols==nRows)
      {
        const Matrix trn= transposed(*this);
        const Matrix prod= (*this)*trn;
        nRows= prod.noRows();
        for(int i=0;i<nRows;i++)
          r+= prod(i,i);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
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
    for(int i=1;i<this->noRows();i++) maximo= std::max(maximo,rowSum(i));
    return maximo;
  }

//! @brief Returns the maximum value of the elements of the vector
//! that results that contains the column sums.
double XC::Matrix::columnNorm(void) const
  {
    double maximo= columnSum(0);
    for(int j=1;j<this->noCols();j++) maximo= std::max(maximo,columnSum(j));
    return maximo;
  }

//! @brief Returns the value of the one norm.
double XC::Matrix::OneNorm(void) const
  {
    double retval= 0.0;

#ifdef _G3DEBUG    
    if(numRows != numCols)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	  	  << "; the matrix of dimensions " 
	          << numRows << ", " << numCols
		<< " is not square " << std::endl;
        return -1;
      }

#endif
    
    // check work area can hold all the data
    int n= numRows;
    const int dataSize= data.Size();
    auxMatrix.resize(dataSize,n);

    double *matrixWork= auxMatrix.getMatrixWork();
    for(int i=0; i<dataSize; i++)
      matrixWork[i]= data(i);


    int ldA= n;
    double *Aptr= matrixWork;

    //Computes the norm of the matrix.
    Vector wrk(n);
    double *wrkPtr= wrk.getDataPtr();
    retval= dlange_("1", &n, &n, Aptr, &ldA, wrkPtr, 1);
    return retval;
  }

//! @brief Return an estimation of the reciprocal of the condition number.
//!
//! estimates the reciprocal of the condition number of the matrix
//! using the 1-norm. An estimate is obtained for norm(inv(A)), and
//! the reciprocal of the condition number is computed as
//!    RCond = 1 / ( norm(A) * norm(inv(A)) ).
double XC::Matrix::RCond(void) const
  {
    double retval= 0.0;

#ifdef _G3DEBUG    
    if(numRows != numCols)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	  	  << "; the matrix of dimensions " 
	          << numRows << ", " << numCols
		<< " is not square " << std::endl;
        return -1;
      }

#endif
    
    // check work area can hold all the data
    int n= numRows;
    const int dataSize= data.Size();
    auxMatrix.resize(dataSize,n);

    double *matrixWork= auxMatrix.getMatrixWork();
    for(int i=0; i<dataSize; i++)
      matrixWork[i]= data(i);

    int ldA= n;
    int info;
    double *Aptr= matrixWork;

    //Computes the "1" norm of the matrix using LAPACK.
    Vector wrk(n);
    double *wrkPtr= wrk.getDataPtr();
    const double anorm= dlange_("1", &n, &n, Aptr, &ldA, wrkPtr, 1);

    //Modifies matrixWork in place with a LU factorization.
    int *iPIV= auxMatrix.getIntWork();
    dgetrf_(&n, &n, Aptr, &ldA, iPIV, &info);
    // info
    //   = 0:  successful exit
    //   < 0:  if INFO = -i, the i-th argument had an illegal value
    //   > 0:  if INFO = i, U(i,i) is exactly zero. The factorization
    //         has been completed, but the factor U is exactly
    //         singular, and division by zero will occur if it is used
    //         to solve a system of equations.
    if(info != 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
    	        << "; LaPack dgtrf_ failure with error: " << info
    	        << std::endl;
    else
      {
	//Computes the reciprocal norm.
	wrk.resize(4*n);
        wrkPtr= wrk.getDataPtr();
	dgecon_("1", &n, Aptr, &ldA, &anorm, &retval, wrkPtr, iPIV, &info, 1);
	// info
	//   = 0:  successful exit
	//   < 0:  if INFO = -i, the i-th argument had an illegal value
	if(info != 0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; LaPack dgecon_ failure with error: " << info
		    << std::endl;
      }
    return retval;
  }
