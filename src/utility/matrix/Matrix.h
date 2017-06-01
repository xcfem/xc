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
                                                                        
// $Revision: 1.10 $
// $Date: 2003/04/02 22:02:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/matrix/Matrix.h,v $
                                                                        
                                                                        
#ifndef Matrix_h
#define Matrix_h 

// File: ~/utility/matrix/Matrix.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for Matrix.
// Matrix is a concrete class implementing the matrix abstraction.
// Matrix class is used to provide the abstraction for the most
// general type of matrix, that of an unsymmetric full matrix.
//
// What: "@(#) Matrix.h, revA"

#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_basic/src/matrices/m_double.h"
#include "Vector.h"

namespace XC {
class ID;
class Vector;
class Message;
class AuxMatrix;

#define MATRIX_VERY_LARGE_VALUE 1.0e213

//! @ingroup Matrix
//
class Matrix: public EntCmd
  {
  private:
    static double MATRIX_NOT_VALID_ENTRY;
    static AuxMatrix auxMatrix;

    int numRows;
    int numCols;
    Vector data;

  public:
    friend class Vector;    
    friend class Message;
    friend class TCP_Socket;
    friend class TCP_SocketNoDelay;
    friend class UDP_Socket;
    friend class MPI_Channel;

    // constructors and destructor
    Matrix(void);	
    Matrix(int nrows, int ncols);
    Matrix(double *data, int nrows, int ncols);
    Matrix(const boost::python::list &l);
    inline virtual ~Matrix(void) {}

    // utility methods
    int setData(double *newData, int nRows, int nCols);
    const double *getDataPtr(void) const;
    double *getDataPtr(void);
    bool Nula(void) const;
    int getDataSize(void) const;
    int getNumBytes(void) const;
    int noRows() const;
    int noCols() const;
    void Zero(void);
    void Identity(void);
    int resize(int numRow, int numCol);
    //void from_string(const std::string &str);
    
    int  Assemble(const Matrix &,const ID &rows, const ID &cols, double fact = 1.0);  
    
    int Solve(const Vector &V, Vector &res) const;
    int Solve(const Matrix &M, Matrix &res) const;
    int Invert(Matrix &res) const;

    double rowSum(int i) const;
    double columnSum(int j) const;
    double rowNorm(void) const;
    double columnNorm(void) const;
    double Norm2(void) const;
    double Norm(void) const;

    int addMatrix(double factThis, const Matrix &other, double factOther);
    int addMatrixProduct(double factThis, const Matrix &A, const Matrix &B, double factOther); // AB
    int addMatrixTripleProduct(double factThis, const Matrix &A, const Matrix &B, double factOther); // A'BA
    //int addMatrixTripleProduct(const Matrix &A, const Matrix &B, const Matrix &C double fact = 1.0); //ABC
    
    // overloaded operators all of which are pure 
    double &operator()(int row, int col);
    const double &operator()(int row, int col) const;
    Matrix operator()(const ID &rows, const ID & cols) const;
    Vector getRow(int row) const;
    Vector getCol(int col) const;
    
    template <class TNSR>
    Matrix &operator=(const TNSR &);
    
    inline bool isRow(void) const
      { return (numRows == 1); }
    inline bool isColumn(void) const
      { return (numCols == 1); }

    // matrix operations which will preserve the derived type and
    // which can be implemented efficiently without many constructor calls.

    // matrix-scalar operations
    Matrix &operator+=(double fact);
    Matrix &operator-=(double fact);
    Matrix &operator*=(double fact);
    Matrix &operator/=(double fact); 

    // matrix operations which generate a new Matrix. They are not the
    // most efficient to use, as constructors must be called twice. They
    // however are usefull for matlab like expressions involving Matrices.

    // matrix-scalar operations
    Matrix operator+(double fact) const;
    Matrix operator-(double fact) const;
    Matrix operator*(double fact) const;
    Matrix operator/(double fact) const;
    
    // matrix-vector operations
    Vector operator*(const Vector &V) const;
    Vector operator^(const Vector &V) const;    

    
    Matrix operator-(void) const;
    // matrix-matrix operations
    Matrix operator+(const Matrix &M) const;
    Matrix operator-(const Matrix &M) const;
    Matrix operator*(const Matrix &M) const;
//     Matrix operator/(const Matrix &M) const;    
    Matrix operator^(const Matrix &M) const;
    Matrix &operator+=(const Matrix &M);
    Matrix &operator-=(const Matrix &M);

    Matrix getTrn(void) const;

    // methods to read/write to/from the matrix
    void Output(std::ostream &s) const;
    void Input(const std::string &);
    void write(std::ofstream &);
    void read(std::ifstream &);
    //    void Input(istream &s);
    
    // methods added by Remo
    int Assemble(const Matrix &V, int init_row, int init_col, double fact = 1.0);
    int AssembleTranspose(const Matrix &V, int init_row, int init_col, double fact = 1.0);
    int Extract(const Matrix &V, int init_row, int init_col, double fact = 1.0);


    friend std::ostream &operator<<(std::ostream &s, const Matrix &M);
    friend std::string to_string(const Matrix &);
    inline std::string toString(void) const
      { return to_string(*this); }
    //    friend istream &operator>>(istream &s, Matrix &M);    
    friend Matrix operator*(double a,const Matrix &M);    
    
  };

Matrix m_double_to_matrix(const m_double &m);
m_double matrix_to_m_double(const Matrix &m);
Matrix identity(const Matrix &m);


/********* INLINED MATRIX FUNCTIONS ***********/
inline bool Matrix::Nula(void) const
   { return data.Nulo(); }

inline int Matrix::getDataSize() const 
  { return data.Size(); }

inline int Matrix::getNumBytes(void) const
  { return data.getNumBytes(); }

inline int Matrix::noRows() const 
  { return numRows; }

inline int Matrix::noCols() const 
  { return numCols; }

inline const double *Matrix::getDataPtr(void) const
  { return data.getDataPtr(); }

inline double *Matrix::getDataPtr(void)
  { return data.getDataPtr(); }

inline double &Matrix::operator()(int row, int col)
  { 
#ifdef _G3DEBUG
    if((row < 0) || (row >= numRows))
      {
        std::cerr << "Matrix::operator() - row " << row << " our of range [0, " <<  numRows-1 << endln;
        return data(0);
      }
    else
      if((col < 0) || (col >= numCols))
        {
          std::cerr << "Matrix::operator() - row " << col << " our of range [0, " <<  numCols-1 << endln;
          return MATRIX_NOT_VALID_ENTRY;
        }
#endif
    return data(col*numRows + row);
  }


inline const double &Matrix::operator()(int row, int col) const
  {
#ifdef _G3DEBUG
    if((row < 0) || (row >= numRows))
      {
        std::cerr << "Matrix::operator() - row " << row 
                  << " our of range [0, " <<  numRows-1 << endln;
        return data(0);
      }
    else if((col < 0) || (col >= numCols))
      {
        std::cerr << "Matrix::operator() - row " << col 
                  << " our of range [0, " <<  numCols-1 << endln;
        return MATRIX_NOT_VALID_ENTRY;
      }
#endif
    return data(col*numRows + row);
  }

inline Matrix transposed(const Matrix &m)
  { return m.getTrn(); }

template <class TNSR>
XC::Matrix &XC::Matrix::operator=(const TNSR &V)
  {
    int rank= V.rank();
    if(rank != 4)
      {
        std::cerr << "Matrix::operator=() - BJtensor must be of rank 4\n";
        return *this;
      }
    int dim= V.dim(1);
    if(dim != V.dim(2) != V.dim(3) != V.dim(4))
      {
        std::cerr << "Matrix::operator=() - BJtensor must have square dimensions\n";
        return *this;
      }

    if(dim != 2 || dim != 3 || dim != 1)
      {
        std::cerr << "Matrix::operator=() - BJtensor must be of dimension 2 or 3\n";
        return *this;
      }      

    if(dim == 1)
      {
        if((numCols != 1) || (numRows != 1))
          {      
            std::cerr << "Matrix::operator=() - matrix must be 1x1 for XC::BJtensor of dimension 3\n";
            return *this;
          }      	  
        (*this)(0,0)= V.cval(1,1,1,1);
    
      }
    else if(dim == 2)
      {
        if((numCols != 3) || (numRows != 3))
          {      
            std::cerr << "Matrix::operator=() - matrix must be 1x1 for XC::BJtensor of dimension 3\n";      
            return *this;
          }
        (*this)(0,0)= V.cval(1,1,1,1);
        (*this)(0,1)= V.cval(1,1,2,2);
        (*this)(0,2)= V.cval(1,1,1,2);      
    
        (*this)(1,0)= V.cval(2,2,1,1);
        (*this)(1,1)= V.cval(2,2,2,2);
        (*this)(1,2)= V.cval(2,2,1,2);      
    
        (*this)(2,0)= V.cval(1,2,1,1);
        (*this)(2,1)= V.cval(1,2,2,2);
        (*this)(2,2)= V.cval(1,2,1,2);            
    
      }
    else
      {
        if((numCols != 6) || (numRows != 6))
          {      
            std::cerr << "Matrix::operator=() - matrix must be 1x1 for XC::BJtensor of dimension 3\n";      
            return *this;
          }      
        (*this)(0,0)= V.cval(1,1,1,1);
        (*this)(0,1)= V.cval(1,1,2,2);
        (*this)(0,2)= V.cval(1,1,3,3);      
        (*this)(0,3)= V.cval(1,1,1,2);
        (*this)(0,4)= V.cval(1,1,1,3);
        (*this)(0,5)= V.cval(1,1,2,3);      
    
        (*this)(1,0)= V.cval(2,2,1,1);
        (*this)(1,1)= V.cval(2,2,2,2);
        (*this)(1,2)= V.cval(2,2,3,3);      
        (*this)(1,3)= V.cval(2,2,1,2);
        (*this)(1,4)= V.cval(2,2,1,3);
        (*this)(1,5)= V.cval(2,2,2,3);            
    
        (*this)(2,0)= V.cval(3,3,1,1);
        (*this)(2,1)= V.cval(3,3,2,2);
        (*this)(2,2)= V.cval(3,3,3,3);      
        (*this)(2,3)= V.cval(3,3,1,2);
        (*this)(2,4)= V.cval(3,3,1,3);
        (*this)(2,5)= V.cval(3,3,2,3);                  
        
        (*this)(3,0)= V.cval(1,2,1,1);
        (*this)(3,1)= V.cval(1,2,2,2);
        (*this)(3,2)= V.cval(1,2,3,3);      
        (*this)(3,3)= V.cval(1,2,1,2);
        (*this)(3,4)= V.cval(1,2,1,3);
        (*this)(3,5)= V.cval(1,2,2,3);                        
        
        (*this)(4,0)= V.cval(1,3,1,1);
        (*this)(4,1)= V.cval(1,3,2,2);
        (*this)(4,2)= V.cval(1,3,3,3);      
        (*this)(4,3)= V.cval(1,3,1,2);
        (*this)(4,4)= V.cval(1,3,1,3);
        (*this)(4,5)= V.cval(1,3,2,3);                              
        
        (*this)(5,0)= V.cval(2,3,1,1);
        (*this)(5,1)= V.cval(2,3,2,2);
        (*this)(5,2)= V.cval(2,3,3,3);      
        (*this)(5,3)= V.cval(2,3,1,2);
        (*this)(5,4)= V.cval(2,3,1,3);
        (*this)(5,5)= V.cval(2,3,2,3);                                    
      }
    return *this;
  }
} // end of XC namespace

#endif




