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
                                                                        
// $Revision: 1.9 $
// $Date: 2005/12/14 23:49:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/matrix/Vector.h,v $

                                                                        
// File: ~/utility/matrix/Vector.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for Vector.
// Vector is a concrete class implementing the vector abstraction.
//
// What: "@(#) Vector.h, revA"

#ifndef Vector_h
#define Vector_h 

#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_basic/src/matrices/m_double.h"
#include <cmath>

class Vector2d;
class Vector3d;

namespace XC {
class ID;

class Matrix; 
class Message;
class SystemOfEqn;

//! @ingroup Matrix
//
class Vector: public EntCmd
  {
  private:
    static double VECTOR_NOT_VALID_ENTRY;
    int sz;
    double *theData;
    int fromFree;
    void libera(void);
    void alloc(const size_t &sz);
  public:
    // constructors and destructor
    Vector(void);
    explicit Vector(const int &, const double &valor= 0.0);
    explicit Vector(const std::vector<double> &v);
    explicit Vector(const Vector2d &v);
    explicit Vector(const Vector3d &v);
    Vector(const double &,const double &,const double &);
    Vector(const Vector &);    
    Vector(double *data, int size);
    Vector(const boost::python::list &);
    virtual ~Vector(void);

    // utility methods
    int setData(double *newData, int size);
    const double *getDataPtr(void) const;
    double *getDataPtr(void);
    bool Nulo(void) const;
    int Assemble(const Vector &V, const ID &l, double fact = 1.0);
    double Norm2(void) const;
    double Norm(void) const;
    double pNorm(int p) const;
    double NormInf(void) const;
    int Size(void) const;
    int getNumBytes(void) const;
    int resize(int newSize);
    void Zero(void);
    bool isnan(void) const;
    int reset(const int &newSize);
    int Normalize(void);
    int NormalizeInf(void);
    Vector Normalized(void) const;
    Vector NormalizedInf(void) const;
    
    int addVector(double factThis, const Vector &other, double factOther);
    int addMatrixVector(double factThis, const Matrix &m, const Vector &v, double factOther); 
    int addMatrixTransposeVector(double factThis, const Matrix &m, const Vector &v, double factOther);

    
    //! @brief Returns the element at the row being passed as parameter.
    virtual double &at(const size_t &f);
    //! @brief Returns the element at the row being passed as parameter.
    virtual const double &at(const size_t &f) const;
    //! @brief Check the index being passed as parameter.
    virtual bool CheckIndice0(const size_t &i) const;

    // overloaded operators
    const double &operator()(int x) const;
    double &operator()(int x);
    const double &operator[](int x) const;  // these two operator do bounds checks
    double &operator[](int x);
    Vector operator()(const ID &rows) const;
    Vector &operator=(const Vector  &V);

    template <class TNSR>
    Vector &operator=(const TNSR &T);
    
    Vector &operator+=(double fact);
    Vector &operator-=(double fact);
    Vector &operator*=(double fact);
    Vector &operator/=(double fact); 

    Vector operator-(void) const;
    Vector operator+(double fact) const;
    Vector operator-(double fact) const;
    Vector operator*(double fact) const;
    Vector operator/(double fact) const;
    
    Vector &operator+=(const Vector &V);
    Vector &operator-=(const Vector &V);
    
    Vector operator+(const Vector &V) const;
    Vector operator-(const Vector &V) const;
    double operator^(const Vector &V) const;
    Vector operator/(const Matrix &M) const;

    double dot(const Vector &) const;    

    // methods added by Remo
    int Assemble(const Vector &V, int init_row, double fact = 1.0);
    int Extract(const Vector &V, int init_row, double fact = 1.0); 

    Vector getComponents(const ID &) const; 
    void putComponents(const Vector &,const ID &); 
    void addComponents(const Vector &,const ID &); 

    void write(std::ofstream &);
    void read(std::ifstream &);
    friend std::ostream &operator<<(std::ostream &s, const Vector &);
    friend std::string to_string(const Vector &);
    inline std::string toString(void) const
      { return to_string(*this); }
    // friend istream &operator>>(istream &s, Vector &V);    
    friend Vector operator*(double a, const Vector &V);
    
    friend class Message;
    friend class SystemOfEqn;
    friend class Matrix;
    friend class TCP_SocketNoDelay;    
    friend class TCP_Socket;
    friend class UDP_Socket;
    friend class MPI_Channel;
  };

std::vector<double> vector_to_std_vector(const Vector &);
m_double vector_to_m_double(const Vector &);

double dot(const Vector &a,const Vector &b);
Matrix prod_tensor(const Vector &,const Vector &);
Matrix operator&(const Vector &u,const Vector &v);

Vector normalize(const Vector &);
Vector normalize_inf(const Vector &);



/********* INLINED VECTOR FUNCTIONS ***********/
inline int Vector::Size(void) const 
  { return sz; }

inline int Vector::getNumBytes(void) const
  { return Size()*sizeof(double); }

inline const double *Vector::getDataPtr(void) const
  { return theData; }

inline double *Vector::getDataPtr(void)
  { return theData; }

inline bool Vector::Nulo(void) const
  { return (theData== nullptr); }

inline void Vector::Zero(void)
  {
    for(register int i=0; i<sz; i++)
      theData[i] = 0.0;
  }

inline bool Vector::isnan(void) const
  {
    bool retval= false;
    for(register int i=0; i<sz; i++)
      if(std::isnan(theData[i]))
        {
          retval= true;
          break;
        }
    return retval;
  }

inline int Vector::reset(const int &newSize)
  {
    const int retval= resize(newSize);
    Zero();
    return retval;
  }

inline const double &Vector::operator()(int x) const
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(x < 0 || x >= sz)
      {
	std::cerr << "XC::Vector::(loc) - loc " << x << " outside range [0, " << sz-1 << endln;
        return VECTOR_NOT_VALID_ENTRY;
      }
#endif
    return theData[x];
  }


inline double &Vector::operator()(int x)
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if (x < 0 || x >= sz)
      {
	std::cerr << "XC::Vector::(loc) - loc " << x << " outside range [0, " << sz-1 << std::endln;
        return VECTOR_NOT_VALID_ENTRY;
      }
#endif
    return theData[x];
  }

template <class TNSR>
Vector & Vector::operator=(const TNSR &V) 
  {
    int rank= V.rank();
    if(rank != 2)
      {
        std::cerr << "XC::Vector::operator=() - BJtensor must be of rank 2\n";
        return *this;
      }
    int dim= V.dim(1);
    if(dim != V.dim(2))
      {
        std::cerr << "XC::Vector::operator=() - BJtensor must have square dimensions\n";
        return *this;
      }
  
    if(dim != 2 || dim != 3 || dim != 1)
      {
        std::cerr << "XC::Vector::operator=() - BJtensor must be of dimension 2 or 3\n";
        return *this;
      }      
  
    if(dim == 1)
      {
        if(sz != 1)
           {
             std::cerr << "Vector::operator=() - Vector size must be 1\n"; 
             return *this;
           }
        theData[0]= V.cval(1,1);
      }
    else if(dim == 2)
      {
        if(sz != 3)
          {
            std::cerr << "Vector::operator=() - Vector size must be 3\n"; 
            return *this;
          }
        theData[0]= V.cval(1,1);
        theData[1]= V.cval(2,2);
        theData[2]= V.cval(1,2);
      }
    else
      {
        if(sz != 6)
          {
            std::cerr << "Vector::operator=() - Vector size must be 6\n"; 
            return *this;
          }      
        theData[0]= V.cval(1,1);
        theData[1]= V.cval(2,2);
        theData[2]= V.cval(3,3);
        theData[3]= V.cval(1,2);
        theData[4]= V.cval(1,3);
        theData[5]= V.cval(2,3);
      }
    return *this;
  }

} // end of XC namespace


#endif

