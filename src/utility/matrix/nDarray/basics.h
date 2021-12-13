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

// $Revision: 1.2 $
// $Date: 2004/06/01 21:19:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/nDarray/basics.h,v $

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
//##
///*
//################################################################################
//# COPY-YES  (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:                                                                     #
//# CLASS:                                                                       #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.10, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic                                             #
//# PROGRAMMER(S):     Boris Jeremic                                             #
//#                                                                              #
//# DATE:              November '92                                              #
//# UPDATE HISTORY:    05 - __ avgust '93.  redefined as derived class from      #
//#                                 nDarray class                                #
//#                    january 06 '93  added matrix2BJtensor_1, matrix2BJtensor_2    #
//#                                   matrix2BJtensor_3                            #
//#                    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                                                                              #
//################################################################################
//*/
#ifndef BASICS_HH
#define BASICS_HH

#include <vector>
#include <cmath>
//#include <values.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <ctime>


// Define BJmatrix as matrix type
#ifndef matrix
#define matrix BJmatrix
#endif

#ifndef tensor
#define Tensor BJtensor
#endif

// redefine Pi from math.h M_PI
#ifndef PI
#define PI 3.14159265358979323846
#endif
//##############################################################################
#ifndef TWOOVERTHREE
#define TWOOVERTHREE 0.6666666666667
#endif
//##############################################################################
#ifndef ONEOVERTHREE
#define ONEOVERTHREE 0.3333333333333
#endif
//##############################################################################
//##############################################################################
// useful arrays for constructors . . .
//#ifdef SASA
 static const int def_dim_4_2[]={2,2,2,2}; //  Sasa jan - 99
//#endif

#ifndef DEF_DIM
#define DEF_DIM
static const std::vector<int> def_dim_1({3});
static const std::vector<int> def_dim_2({3, 3}); // static-> see ARM pp289-290
static const std::vector<int> def_dim_3({3, 3, 3}); // static-> see ARM pp289-290
static const std::vector<int> def_dim_4({3, 3, 3, 3}); // static-> see ARM pp289-290

// Cosserat
static const std::vector<int> Cosserat_def_dim_2({6, 6}); // static-> see ARM pp289-290
static const std::vector<int> Cosserat_def_dim_4({6, 6, 6, 6}); // static-> see ARM pp289-290

#endif

#ifndef TEST
#define TEST
  const int tst = 3;
#endif


#ifndef DZERO
#define DZERO 0.0
//  double ZERO = 0.0;
#endif

float       f_macheps();
double      d_macheps();
long double ld_macheps();

#endif


