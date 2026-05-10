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
//#############################################################################
//                                                                            #
//                                                                            #
//             /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\                #
//            |                                          |____|               #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |        B A S E   C L A S S E S           |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |          C + +     H E A D E R           |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//         /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/   |                    #
//         \_________________________________________\__/                     #
//                                                                            #
//                                                                            #
//#############################################################################
//#############################################################################
///*
//################################################################################
//# COPYRIGHT (C):     :-))                                                   #
//# PROJECT:           Object Oriented Finite Element Program                 #
//# PURPOSE:                                                                  #
//# CLASS:             nDarray                                                #
//#                                                                           #
//# VERSION:                                                                  #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.10, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                   #
//# DESIGNER(S):       Boris Jeremic                                          #
//# PROGRAMMER(S):     Boris Jeremic                                          #
//#                                                                           #
//#                                                                           #
//# DATE:              May 28. - July  20  '93                                #
//# UPDATE HISTORY:    july 8. '93. BJtensor02 - BJtensor multiplication          #
//#                                 inner and outer products                  #
//#                    December 23 1993 print from the base class, operator==,#
//#                                     macheps . . .                         #
//#                    August 22-29 '94 choped to separate files and worked on#
//#                                   const and & issues                      #
//#                    August 30-31 '94 added use_def_dim to full the CC      #
//#                                   resolved problem with temoraries for    #
//#                                   operators + and - ( +=, -= )            #
//#                    January 16 '95 fixed the memory leakage introduced     #
//#                                   by previous work on +=, -+. I was       #
//#                                   by mistake decreasing                   #
//#                                   this->pc_nDarray_rep->total_numb--;     #
//#                                   inststead of                            #
//#                                   this->pc_nDarray_rep->n--;              #
//#                    28June2004     added val4 for efficiency still         #
//#                                   to be worked on                         #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//#############################################################################
//*/

#ifndef NDARRAY_HH
#define NDARRAY_HH

#include "utility/matrix/nDarray/nDarray_rep.h"
#include <string>
#include <iostream>
#include <boost/python.hpp>
#include "tmpl_operators.h"

// forward reference
namespace XC {

//! @brief n-dimensional array.
//! @ingroup Matrix
class nDarray
  {
  protected:
    nDarray_rep pc_nDarray_rep;

    const double *data(void) const;
    const std::vector<double> &vector_data(void) const;
    void set_dim(const std::vector<int> &);
    void rank(int);
  private:
//  int rank(void) const;
    size_t total_number(void) const;
    void total_number(size_t );
    void clear_dim(void);
    void clear_data(void);
    void clear_dim_data(void);
    const int &get_dim_pointer(void) const;
    //    int dim(int which) const;
    
  public:
    nDarray(int rank_of_nDarray=1, const double &initval=0.0);// default constructor
    nDarray(const std::vector<int> &pdim, const double *values);
    nDarray(const std::vector<int> &pdim, const std::vector<double> &);
    nDarray(const std::vector<int> &pdim, const boost::python::list &);
    nDarray(const boost::python::list &, const boost::python::list &);
    nDarray(const std::vector<int> &pdim, double initvalue);

    // special case for BJmatrix and BJvector . . .
    nDarray(int rows, int cols, double *values);
    nDarray(int rows, int cols, const std::vector<double> &values);
    nDarray(int rows, int cols, const boost::python::list &);
    nDarray(int rows, int cols, double initvalue);

// special case when I don't want any initialization at all##
    explicit nDarray(const std::string &){}

    nDarray(const std::string &flag, const std::vector<int> &pdim); // create a unit nDarray
    inline virtual ~nDarray(void){};

//##############################################################################
// copy only data because everything else has already been defined
// WATCH OUT IT HAS TO BE DEFINED BEFORE THIS FUNCTIONS IS CALLED
// use "from" and initialize already allocated nDarray from "from" values
    void Initialize(const nDarray &from );  // initialize data only
    void Initialize_all(const nDarray &from);// initialize and allocate all
                                               // ( dimensions, rank and data )
                                               // for BJtensor

    void Reset_to(const double &value);  // reset data to "value"

    const std::vector<int> &dim(void) const;
    boost::python::list dimPy(void) const;
    
    inline const double &operator()(int first) const
      { return pc_nDarray_rep(first); }
    
    inline double &operator()(int first)
      {
	nDarray *this_no_const= const_cast<nDarray *>(this);
	return this_no_const->operator()(first);
      }
    
    inline const double &operator()(int first, int second) const
      { return pc_nDarray_rep(first, second); }

    inline double &operator()(int first, int second)
      {
	nDarray *this_no_const= const_cast<nDarray *>(this);
	return this_no_const->operator()(first, second);
      }
    
    inline const double &operator()(int first, int second, int third) const
      { return pc_nDarray_rep(first, second, third); }

    inline double &operator()(int first, int second, int third)
      {
	nDarray *this_no_const= const_cast<nDarray *>(this);
	return this_no_const->operator()(first, second, third);
      }
    inline const double &operator()(int first, int second, int third, int fourth) const
      { return pc_nDarray_rep(first, second, third, fourth); }
    
    inline double &operator()(int first, int second, int third, int fourth)
      {
	return pc_nDarray_rep(first, second, third, fourth);
      }

    //const double &val(int subscript, ...) const;
    //double &val(int subscript, ...);
    inline const double &_val(const std::vector<int> &subscripts) const
      { return _cval(subscripts); }
    double &_val(const std::vector<int> &);
    template<class...nums>
    const double &val(nums...args) const
      {
	const std::vector<int> vec = {args...};
	return _val(vec);
      }
    template<class...nums>
    double &val(nums...args)
      {
	const std::vector<int> vec = {args...};
	return _val(vec);
      }
    
    const double &val4(int first, int second, int third, int fourth) const;  // overloaded for FOUR arguments for operator * for two tensors
    double &val4(int first, int second, int third, int fourth);  // overloaded for FOUR arguments for operator * for two tensors

    //const double &cval(int subscript, ...) const;
    const double &_cval(const std::vector<int> &) const;
    template<class...nums>
    const double &cval(nums...args) const
      {
	std::vector<int> vec = {args...};
	return _cval(vec);
      }

//..



//++    nDarray operator+( nDarray & rval); // nDarray addition
//....// This is from JOOP May/June 1990 after ARKoenig
    nDarray& operator+=(const nDarray &); // nDarray addition


//++    nDarray operator-( nDarray & rval); // nDarray subtraction
//....// This is from JOOP May/June 1990 after ARKoenig
    nDarray &operator-=(const nDarray & ); // nDarray subtraction

    nDarray operator+(const double &rval);  // scalar addition
    nDarray operator-(const double &rval);  // scalar subtraction
    nDarray &operator*=(const double &rval); // scalar multiplication
    nDarray operator*(const double &rval) const; // scalar multiplication

    nDarray operator-();  // Unary minus

    double sum(void) const;    // summ of all the elements
    double trace(void) const;            // trace of a 2-nd BJtensor, BJmatrix

    bool operator==(const nDarray &rval) const;

// prebacen u nDarray 14 oktobra 1996
  public:
    nDarray eigenvalues(void);
    nDarray eigenvectors(void);


    double Frobenius_norm( void ); // return the Frobenius norm of
                                   // BJmatrix, BJtensor, BJvector
    double General_norm( double p ); // return the General p-th norm of
                                     // BJmatrix, BJtensor, BJvector
  public:
    int rank(void) const;
    int dim(int which) const;
    
    void output(std::ostream &os) const;
    void outputshort(std::ostream &os) const;
    void print(const std::string &name = "t",const std::string &msg = "Hi there#", std::ostream &os= std::cout) const;
    void printshort(std::ostream &os, const std::string &msg = "Hi there#") const;
    void mathprint(std::ostream &os) const;
    friend std::string to_string(const nDarray &);
    inline std::string toString(void) const
      { return to_string(*this); }

// from Numerical recipes in C
  private:
    static void tqli(std::vector<double> &d, std::vector<double> &, int n, std::vector<std::vector<double> > &z);
    static void tred2(std::vector<std::vector<double> > &a, int n, std::vector<double> &d, std::vector<double> &e);
    static void eigsrt(std::vector<double> &d, std::vector<std::vector<double> > &v, int n);

  };
  
template nDarray operator*(const double & , const nDarray & );
template nDarray operator+(const nDarray & , const nDarray & );
template nDarray operator-(const nDarray & , const nDarray & );
std::ostream& operator<<(std::ostream &, const nDarray &);
std::string to_string(const  nDarray &);

} // end of XC namespace


#endif

