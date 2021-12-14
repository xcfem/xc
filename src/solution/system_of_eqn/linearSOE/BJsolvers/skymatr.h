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
//##############################################################################
//# COPYRIGHT (C):     :-))                                                    #
//# PROJECT:           Object Oriented Finite Element Program                  #
//# PURPOSE:                                                                   #
//# CLASS:             skymatrix                                               #
//#                                                                            #
//# VERSION:                                                                   #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.10, SUN C++ ver=2.1 )#
//# TARGET OS:         DOS || UNIX || . . .                                    #
//# PROGRAMMER(S):     Boris Jeremic                                           #
//#                                                                            #
//#                                                                            #
//# DATE:              November '92                                            #
//# UPDATE HISTORY:    05 - __ avgust '93.  redefined as derived class from    #
//#                                 ndarray class                              #
//#                    August 22-29 '94 choped to separate files and worked on #
//#                                   const and & issues                       #
//#                    August 30-31 '94 added use_def_dim to full the CC       #
//#                                   resolved problem with temoraries for     #
//#                                   operators + and - ( +=, -= )             #
//#                                                                            #
//#                    September 09 '94 starting to rewrite things after a talk#
//#                    by Stephen Jonson " Objecting the Objects". The point is#
//#                    to forget about inheriting skymatrix from ndarray and   #
//#                    start from separate branch!                             #
//#                                                                            #
//#                    September 11 '94 it works                               #
//#                    September 12-13 '94 looking for the solver for symmetric#
//#                                        and unsymmetric sparse matrices.    #
//#                                        One solution is Taylor's profile    #
//#                                        solver ( see FEM4ed by O.Z. and R.T.#
//#                    September 27 '94 profile solver for symmetric and       #
//#                                     Nonsymmetric systems works!            #
//#                                     (from FEM4ed by O.Z. and R.T.)         #
//#                                                                            #
//#                                                                            #
//##############################################################################

#ifndef SKYMATRIX_HH
#define SKYMATRIX_HH

#include <string>
//#include <femdata.h>
//#include <brick3d.h>
//#include <node.h>
//#include <stifmat.h>


namespace XC {
class skymatrix
  {
//    private:
      struct skymatrix_rep
        {
          int * columnheight;
          int * maxa;
          double * data;
          int square_dim;
        } *pc_skymatrix_rep;

    public:

      skymatrix(int order_n, int *maxa, double *initval);
      ~skymatrix();


      int dimension_of_sky_M(void ) const; // dimension of  sky matrix
      int *get_MAXA(void) const; // get pointer to array of

      double & val(int row,  int col); // element selection;
      double cval(int row,  int col) const; // element selection;

      double mmin( ); // find minimum element in the skymatrix
      double mmax( ); // find maximum element in the skymatrix

      void lower_print(const std::string &msg = ""); // print lower part of
                                        // skymatrix with a message
      void upper_print(const std::string &msg = ""); // print upper part of
                                        // skymatrix with a message
      void full_print(const std::string &msg =  ""); // print sky matrix
                                        // as a full matrix with a message




      skymatrix & v_ldl_factorize( ); // ldl factorizing sky matrix
      double * d_reduce_r_h_s_l_v ( double * );
      double * d_back_substitute ( double * );


    private:
      void error(const std::string &msg1,const std::string &msg2 = "") const; // private function

      double & mval(int , int ) const;
// full_val inline function allows you to treat skymatrix as if it is full
// float matrix. The function will calculate position inside sky matrix
// and return appropriate number if row and col are below skyline or
// return zero (0) if row and col are above sky line
      double full_val (int , int ) const;

  };
} // end of XC namespace


#endif
