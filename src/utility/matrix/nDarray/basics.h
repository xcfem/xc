//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
// usefull arrays for constructors . . .
//#ifdef SASA
 static const int def_dim_4_2[]={2,2,2,2}; //  Sasa jan - 99
//#endif

#ifndef DEF_DIM
#define DEF_DIM
  static const int def_dim_1[] = {3};
  static const int def_dim_2[] = {3, 3}; // static-> see ARM pp289-290
  static const int def_dim_3[] = {3, 3, 3}; // static-> see ARM pp289-290
  static const int def_dim_4[] = {3, 3, 3, 3}; // static-> see ARM pp289-290

// Cosserat
  static const int Cosserat_def_dim_2[] = {6, 6}; // static-> see ARM pp289-290
  static const int Cosserat_def_dim_4[] = {6, 6, 6, 6}; // static-> see ARM pp289-290

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


