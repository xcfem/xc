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

#ifndef NDARRAY_REP_HH
#define NDARRAY_REP_HH

#include "utility/matrix/nDarray/basics.h"
#include <boost/python.hpp>
#include <iostream>

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
    size_t total_numb; // total number of elements in nDarray
    std::vector<int> dim; //  array of dimensions in each rank direction
                          //  for example, if get_get_nDarray_rank() = 3 :
                          //      dim[0] = dimension in direction 1
                          //      dim[1] = dimension in direction 2
                          //      dim[2] = dimension in direction 3  */
    
    inline int get_nDarray_rank(void) const ///*  nDarray rank :
      { return dim.size(); }                //     0  ->  scalar
                                            //     1  ->  BJvector
                                            //     2  ->  BJmatrix
                                            //     *  ->  ********   */
    inline size_t get_index(int first, int second) const
      {
        //assert(get_nDarray_rank()==3);
        return (first - 1)*dim[1]+second - 1;
      }
    inline size_t get_index(int first, int second, int third) const
      {
        //assert(get_nDarray_rank()==3);
        return ((first - 1)*dim[1]+second - 1)*dim[2]+third - 1;
      }
    inline size_t get_index(int first, int second, int third, int fourth) const
      {
        //assert(get_nDarray_rank()==4);
        return (((first - 1)*dim[1]+second - 1)*dim[2]+third - 1)*dim[3]+fourth - 1;
      }
  public:
    nDarray_rep(void)
      : pd_nDdata(), total_numb(), dim() {}
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
    inline const std::vector<double> &get_data(void) const
      { return pd_nDdata; }
    inline std::vector<double> &get_data(void)
      { return pd_nDdata; }
    inline double *get_data_ptr(void)
      { return pd_nDdata.data(); }
    inline const double *get_data_ptr(void) const
      { return pd_nDdata.data(); }
    bool equal_data(const std::vector<double> &other_data) const;
    inline const double &val(const size_t &where) const
      { return pd_nDdata[where]; }
    inline double &val(const size_t &where)
      { return pd_nDdata[where]; }
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
    inline const double &operator()(int first) const
      {
	if(get_nDarray_rank()==0)
	  return (pd_nDdata[0]);
	return val(static_cast<size_t>(first - 1));
      }
    inline double &operator()(int first)
      {
	if(get_nDarray_rank()==0)
	  return (pd_nDdata[0]);
	return val(static_cast<size_t>(first - 1));
      }
    inline const double &operator()(int first, int second) const
      {
        const size_t i= get_index(first, second);
	return val(i);
      }
    inline double &operator()(int first, int second)
      {
        const size_t i= get_index(first, second);
	return val(i);
      }
    inline const double &operator()(int first, int second, int third) const
      {
        //assert(get_nDarray_rank()==3);
        const size_t i= get_index(first, second, third);
	return val(i);
      }
    inline double &operator()(int first, int second, int third)
      {
        const size_t i= get_index(first, second, third);
	return val(i);
      }
    inline const double &operator()(int first, int second, int third, int fourth) const
      {
        //assert(get_nDarray_rank()==4);
        const size_t i= get_index(first, second, third, fourth);
	return val(i);
      }
    inline double &operator()(int first, int second, int third, int fourth)
      {
        const size_t i= get_index(first, second, third, fourth);
	return val(i);
      }
    void print(std::ostream &os) const;
  };
  
std::ostream& operator<<(std::ostream &, const nDarray_rep &);

} // end of XC namespace


#endif

