// -*-c++-*-
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
///*
//################################################################################
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           strain tensor with all necessary functions                #
//# CLASS:             straintensor                                              #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic                                             #
//# PROGRAMMER(S):     Boris Jeremic                                             #
//#                                                                              #
//#                                                                              #
//# DATE:              July 25 '93                                               #
//# UPDATE HISTORY:    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC,        #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/
//
#ifndef STRAINTENSOR_HH
#define STRAINTENSOR_HH

#include "stress_strain_tensor.h"
#include "tmpl_operators.h"

namespace XC {

//! @ingroup Matrix
//
//! @brief Strain tensor
class straintensor: public stressstraintensor
  {
  public: // just send appropriate arguments to the base constructor
//    straintensor (int rank_of_tensor=2, double initval=0.00000003141528);
    straintensor(double initval=0.0);
// default constructor           // this is just PI/10^8 to check default constructor

    straintensor(const double *values);
    straintensor(const std::vector<double> &values);
    explicit straintensor(const Vector &);

    straintensor(const straintensor & x );
    explicit straintensor(const BJtensor & x); // copy-initializer
    explicit straintensor(const nDarray & x); // copy-initializer

    straintensor operator=(const straintensor & rval); // straintensor assignment
    straintensor operator=(const BJtensor & rval);// tensor assignment to straintensor
    straintensor &operator+=(const straintensor & rval); // straintensor addition
    straintensor &operator-=(const straintensor & rval); // straintensor subtraction
    
    straintensor &operator*=(const double &rval); // product
    straintensor operator*(const double &rval) const; // scalar multiplication

//ini  // use "from" and initialize already allocated strain tensor from "from" values
//ini      void Initialize( const straintensor & from );

//___// operator() overloading for 3D Gauss points!
//___    straintensor & operator()(short ir, short is, short it,
//___                              short tr, short ts, short tt  );
 

    double equivalent(void) const;	  //Zhaohui added 09-02-2000

    straintensor deviator(void) const;
    straintensor principal(void) const;



    straintensor pqtheta2strain( double, double, double );
    straintensor evoleq2strain( double, double );

    void report(const std::string &) const;
    void reportshort(const std::string &) const;

    friend std::ostream &operator<<(std::ostream &, const straintensor &);

//..// polynomial root solver friend functions definitions
//..public:
//..friend void laguer(complex *, int , complex *, double , int );
//..friend void zroots(complex *, int , complex *, int );
//..
  };

std::ostream &operator<<(std::ostream &, const straintensor &);
template straintensor operator*(const double & , const straintensor & );
template straintensor operator+(const straintensor & , const straintensor & );
template straintensor operator-(const straintensor & , const straintensor & );
BJtensor operator+(const BJtensor &, const straintensor &);
BJtensor operator-(const BJtensor &, const straintensor &);
BJtensor operator+(const straintensor &, const BJtensor &);
BJtensor operator-(const straintensor &, const BJtensor &);

} // end of XC namespace

#endif

