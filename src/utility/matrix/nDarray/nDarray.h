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

#include "utility/matrix/nDarray/basics.h"
#include <string>
#include <boost/python.hpp>
#include "tmpl_operators.h"

// forward reference
namespace XC {
class BJtensor;
class BJmatrix;
class BJvector;


class stresstensor;
class straintensor;


//! @brief Storage of n-dimensional array data.
//! @ingroup Matrix
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
    std::vector<double> pd_nDdata;  // nD array as 1D array
    int nDarray_rank;   ///*  nDarray rank :
                        //     0  ->  scalar
                        //     1  ->  BJvector
                        //     2  ->  BJmatrix
                        //     *  ->  ********   */
    size_t total_numb; // total number of elements in nDarray
    std::vector<int> dim; //  array of dimensions in each rank direction
                          //  for example, if nDarray_rank = 3 :
                          //      dim[0] = dimension in direction 1
                          //      dim[1] = dimension in direction 2
                          //      dim[2] = dimension in direction 3  */
  public:
    void init_dim(const size_t &, const int &default_dim= 1);
    void init_dim(const std::vector<int> &pdim);
    inline void clear_dim(void)
      { dim.clear(); }
    inline bool equal_dim(const std::vector<int> &rval) const
      { return (dim==rval); }
    void init_data(void);
    void init_data(const double &);
    void init_data(const double *);
    void init_data(const std::vector<double> &);
    void init_data(const boost::python::list &);
    void reset_data_to(const double &);
    inline double *get_data_ptr(void)
      { return pd_nDdata.data(); }
    inline const double *get_data_ptr(void) const
      { return pd_nDdata.data(); }
    bool equal_data(const std::vector<double> &other_data) const;
    inline const double &val(const size_t &where) const
      {
        const double *p_value = pd_nDdata.data() + where;
        return (*p_value);
      }
    inline double &val(const size_t &where)
      {
        double *p_value = pd_nDdata.data() + where;
        return (*p_value);
      }
    inline void clear_data(void)
      { pd_nDdata.clear(); }

    inline void clear(void)
      {
	clear_data();
	clear_dim();
      }
    void sum_data(const std::vector<double> &);
    void substract_data(const std::vector<double> &);
    void neg(void);
    double sum(void) const;
    bool operator==(const nDarray_rep &rval) const;
  };

//! @brief n-dimensional array.
//! @ingroup Matrix
class nDarray
  {
  private:
//  int rank(void) const;
    size_t total_number(void) const;
    void total_number(size_t );
    void clear_dim(void);
    void clear_data(void);
    void clear_dim_data(void);
    const int &get_dim_pointer(void) const;
    //    int dim(int which) const;

  protected:
    nDarray_rep pc_nDarray_rep;

    const double *data(void) const;
    void set_dim(const std::vector<int> &);
    const std::vector<int> &dim(void) const;
    void rank(int);
  public:
    nDarray(int rank_of_nDarray=1, const double &initval=0.0);// default constructor
    nDarray(int rank_of_nDarray, const std::vector<int> &pdim, const double *values);
    nDarray(int rank_of_nDarray, const std::vector<int> &pdim, const std::vector<double> &);
    nDarray(int rank_of_nDarray, const std::vector<int> &pdim, const boost::python::list &);
    nDarray(int rank_of_nDarray, const std::vector<int> &pdim, double initvalue);

    // special case for BJmatrix and BJvector . . .
    nDarray(int rank_of_nDarray, int rows, int cols, double *values);
    nDarray(int rank_of_nDarray, int rows, int cols, double initvalue);

// special case when I don't want any initialization at all##
    explicit nDarray(const std::string &){};

    nDarray(const std::string &flag, int rank_of_nDarray, const std::vector<int> &pdim); // create a unit nDarray
    virtual ~nDarray(void);

//##############################################################################
// copy only data because everything else has already been defined
// WATCH OUT IT HAS TO BE DEFINED BEFORE THIS FUNCTIONS IS CALLED
// use "from" and initialize already allocated nDarray from "from" values
    void Initialize(const nDarray &from );  // initialize data only
    void Initialize_all(const nDarray &from);// initialize and allocate all
                                               // ( dimensions, rank and data )
                                               // for BJtensor

    void Reset_to(const double &value);  // reset data to "value"

    const double &val(int subscript, ...) const;
    double &val(int subscript, ...);
    const double &val4(int first, int second, int third, int fourth) const;  // overloaded for FOUR arguments for operator * for two tensors
    double &val4(int first, int second, int third, int fourth);  // overloaded for FOUR arguments for operator * for two tensors

    const double &cval(int subscript, ...) const; // const

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

    void print(const std::string &name = "t",const std::string &msg = "Hi there#") const;
    void printshort(const std::string &msg = "Hi there#") const;
    void mathprint(void) const;
     // print nDarray with a "name" and the "message"

    double Frobenius_norm( void ); // return the Frobenius norm of
                                   // BJmatrix, BJtensor, BJvector
    double General_norm( double p ); // return the General p-th norm of
                                     // BJmatrix, BJtensor, BJvector
  public:
    int rank(void) const;
    int dim(int which) const;

// from Numerical recipes in C
  private:
    static void tqli(std::vector<double> &d, std::vector<double> &, int n, std::vector<std::vector<double> > &z);
    static void tred2(std::vector<std::vector<double> > &a, int n, std::vector<double> &d, std::vector<double> &e);
    static void eigsrt(std::vector<double> &d, std::vector<std::vector<double> > &v, int n);

  };
  
template nDarray operator*(const double & , const nDarray & );
template nDarray operator+(const nDarray & , const nDarray & );
template nDarray operator-(const nDarray & , const nDarray & );

} // end of XC namespace


#endif

