// -*-c++-*-
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
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:                                                                     #
//# CLASS:             nDarray                                                   #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.10, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic                                             #
//# PROGRAMMER(S):     Boris Jeremic                                             #
//#                                                                              #
//#                                                                              #
//# DATE:              May 28. - July  20  '93                                   #
//# UPDATE HISTORY:    july 8. '93. BJtensor02 - BJtensor multiplication             #
//#                                 inner and outer products                     #
//#                    December 23 1993 print from the base class, operator==,   #
//#                                     macheps . . .                            #
//#                    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                    January 16 '95 fixed the memory leakage introduced        #
//#                                   by previous work on +=, -+. I was          #
//#                                   by mistake decreasing                      #
//#                                   this->pc_nDarray_rep->total_numb--;        #
//#                                   inststead of                               #
//#                                   this->pc_nDarray_rep->n--;                 #
//#                    28June2004     added val4 for efficiency still            #
//#                                   to be worked on                            #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#ifndef NDARRAY_HH
#define NDARRAY_HH

#include "utility/matrix/nDarray/basics.h"
#include <string>
#include <boost/python.hpp>

// forward reference
namespace XC {
class BJtensor;
class BJmatrix;
class BJvector;

//class stiffness_BJmatrix;

//class Material_Model;
//class Elastic;
//class Drucker_Prager;
//class von_Mises;
//class MRS_Lade_cone;
//class Parabolic;
//
class stresstensor;
class straintensor;




//! @ingroup Matrix
//!
//! @brief Storage of n-dimensional array data.
class nDarray_rep
  {
  public:
    friend class nDarray;
    friend class BJtensor;
    friend class BJmatrix;
//    friend class skyBJmatrix;
    friend class stiffness_matrix;
    friend class BJvector;
    friend class stressstraintensor;
    friend class stresstensor;
    friend class straintensor;

    friend class Cosseratstresstensor;
    friend class Cosseratstraintensor;

  private:
    double *pd_nDdata;  // nD array as 1D array
    int nDarray_rank;   ///*  nDarray rank :
                        //     0  ->  scalar
                        //     1  ->  BJvector
                        //     2  ->  BJmatrix
                        //     *  ->  ********   */
    long int total_numb; // total number of elements in nDarray
    int *dim;          //  array of dimensions in each rank direction
                       //  for example, if nDarray_rank = 3 :
                       //      dim[0] = dimension in direction 1
                       //      dim[1] = dimension in direction 2
                       //      dim[2] = dimension in direction 3  */
    int n;             // reference count
  public:
// overloading operator new and delete in nDarray_rep class  ########
    void *operator new(size_t s); // see C++ reference manual by
    void operator delete(void *);  // by ELLIS and STROUSTRUP page 283.
                                   // and ECKEL page 529.
  };

//! @ingroup Matrix
//!
//! @brief n-dimensional array.
class nDarray
  {
  private:
    nDarray_rep * pc_nDarray_rep;
  private:
    friend class BJtensor;
    friend class BJmatrix;
    friend class BJvector;
    friend class stiffness_matrix;

    friend class stressstraintensor;
    friend class stresstensor;
    friend class straintensor;

    friend class Cosseratstresstensor;
    friend class Cosseratstraintensor;

//.. no need    friend class GaussPoint;
          // explanation why this one should be a friend instead
          // of inheriting all data through protected construct
          // see in J. Coplien "Advanced C++..." page 96.
  public:
    nDarray(int rank_of_nDarray=1, double initval=0.0);// default constructor
    nDarray(int rank_of_nDarray, const int *pdim, const double *values);
    nDarray(int rank_of_nDarray, const int *pdim, const std::vector<double> &);
    nDarray(int rank_of_nDarray, const int *pdim, const boost::python::list &);
    nDarray(int rank_of_nDarray, const int *pdim, double initvalue);
//..//  for skyBJmatrix --v
//..    nDarray(int dim);
//..    nDarray(int dim, double* initvalue);

// special case for BJmatrix and BJvector . . .
    nDarray(int rank_of_nDarray, int rows, int cols, double *values);
    nDarray(int rank_of_nDarray, int rows, int cols, double initvalue);

// special case when I don't want any initialization at all##
    explicit nDarray(const std::string &){};

    nDarray(const std::string &flag, int rank_of_nDarray, const int *pdim); // create a unit nDarray
    nDarray(const nDarray &x);  // copy-initializer
    virtual ~nDarray(void);

//##############################################################################
// copy only data because everything else has already been defined
// WATCH OUT IT HAS TO BE DEFINED BEFORE THIS FUNCTIONS IS CALLED
// use "from" and initialize already allocated nDarray from "from" values
    void Initialize(const nDarray &from );  // initialize data only
    void Initialize_all(const nDarray &from);// initialize and allocate all
                                               // ( dimensions, rank and data )
                                               // for BJtensor

    void Reset_to(double value );  // reset data to "value"

//..    double operator( ) (int subscript, ...) const; // same as val
                                                   // but overloaded (...)
                                                   // and public !

//@@@@@     double operator( )(int first) const;  // overloaded for ONE argument
//@@@@@     double operator( )(int first,
//@@@@@                        int second) const; // overloaded for TWO arguments
//@@@@@     double operator( )(int first,
//@@@@@                        int second,
//@@@@@                        int third) const;  // overloaded for THREE arguments
//@@@@@     double operator( )(int first,
//@@@@@                        int second,
//@@@@@                        int third,
//@@@@@                        int fourth) const;  // overloaded for FOUR arguments
//@@@@@
//@@@@@     double operator( )(int first,
//@@@@@                        int second,
//@@@@@                        int third,
//@@@@@                        int fourth,
//@@@@@                        int subscript,
//@@@@@                        ... ) const; // overloaded for more than FOUR arguments

//    BJtensor & operator()(char *indices_from_user);// to be defined in BJtensor class
    const double &val(int subscript, ...) const;
    double &val(int subscript, ...);
    const double &val4(int first, int second, int third, int fourth) const;  // overloaded for FOUR arguments for operator * for two tensors
    double &val4(int first, int second, int third, int fourth);  // overloaded for FOUR arguments for operator * for two tensors

// ..JB..     double & val(int first);  // overloaded for ONE argument
// ..JB..     double & val(int first,
// ..JB..                  int second); // overloaded for TWO arguments
// ..JB..     double & val(int first,
// ..JB..                  int second,
// ..JB..                  int third);  // overloaded for THREE arguments
// ..JB..     double & val(int first,
// ..JB..                  int second,
// ..JB..                  int third,
// ..JB..                  int fourth);  // overloaded for FOUR arguments
// ..JB..
//..    double & val(int first,
//..                 int second,
//..                 int third,
//..                 int fourth,
//..                 int subscript,
//..                 ... ); // overloaded for more than FOUR arguments
//..
    double cval(int subscript, ...) const; // const

//..



    nDarray& operator=(const nDarray &rval); // nDarray assignment

//++    nDarray operator+( nDarray & rval); // nDarray addition
//....// This is from JOOP May/June 1990 after ARKoenig
    nDarray& operator+=(const nDarray &); // nDarray addition


    friend nDarray operator+(const nDarray & , const nDarray & ); // nDarray addition
///////##############################################################################
/////// nDarray addition
/////friend nDarray operator+(const nDarray & lval, const nDarray & rval)
/////  {
/////    nDarray result(lval);
/////    result += rval;
/////    return result;
/////  }



//++    nDarray operator-( nDarray & rval); // nDarray subtraction
//....// This is from JOOP May/June 1990 after ARKoenig
    nDarray &operator-=(const nDarray & ); // nDarray subtraction
    friend nDarray operator-(const nDarray & , const nDarray & ); // nDarray subtraction

    nDarray operator+(double rval);  // scalar addition
    nDarray operator-(double rval);  // scalar subtraction
    nDarray operator*(const double rval) const ;  // scalar multiplication

    nDarray operator-();  // Unary minus

    double sum(void) const;    // summ of all the elements
    double trace(void) const;            // trace of a 2-nd BJtensor, BJmatrix

    nDarray deep_copy(void); // make an image

    bool operator==(const nDarray &rval);  // nDarray comparison
                                      // returns 1 if they are same
                                      // returns 0 if they are not

// prebacen u nDarray 14 oktobra 1996
  public:
    nDarray eigenvalues(void);
    nDarray eigenvectors(void);

    nDarray nDsqrt(void);


    void print(const std::string &name = "t",const std::string &msg = "Hi there#") const;
    void printshort(const std::string &msg = "Hi there#") const;
    void mathprint(void) const;
     // print nDarray with a "name" and the "message"

    double Frobenius_norm( void ); // return the Frobenius norm of
                                   // BJmatrix, BJtensor, BJvector
    double General_norm( double p ); // return the General p-th norm of
                                     // BJmatrix, BJtensor, BJvector

    int number_of_zeros(void) const; // number of members that are
                                      // smaller than sqrt(macheps)
//.....//-> the declaration also put in the nDaray_rep class definition because
//.....//-> frienship is not inhereted ( see ARM page 251 or around ).
//.....    friend BJtensor operator*(BJtensor& lval, BJtensor& rval); // inner/outer product

  public:
    int rank(void) const;
    int dim(int which) const;

// from Numerical recipes in C
  private:
    void tqli(double * d, double * e, int n, double ** z);
    void tred2(double ** a, int n, double * d, double * e);
    void eigsrt(double * d, double ** v, int n);

  private:
    double* data(void) const;
    void set_data_pointer(double* );
//  int rank(void) const;
    void rank(int );
    long int total_number(void) const ;
    void total_number(long int );
    int* dim(void) const ;
    int& get_dim_pointer(void) const ;
    void set_dim_pointer(int* );
    //    int dim(int which) const;
    int reference_count(int );
    void set_reference_count(int );

};
// GLOBAL
nDarray operator*(const double lval,const nDarray &rval);  // REVIEWER global *

} // end of XC namespace


#endif

