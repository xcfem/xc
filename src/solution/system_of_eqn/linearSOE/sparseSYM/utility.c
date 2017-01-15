/*
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
*/
/*
 * File:  utility.c
 * ================
 * This file contains miscellaneous utility routines used throughout
 * the finite element program.
 *
 * Originally written by:  David R. Mackay
 *
 * Modified by:
 *  Jun Peng (junpeng@stanford.edu)
 *  Prof. Kincho H. Law
 *  Stanford University
 * --------------------
 */
    

#include <stdio.h>
#include <stdlib.h>
#include "utility.h"


void move_real(double *from, double *to, int n)
  {
    double *last= to + n;
    while(to < last) *to++ = *from++;
  }
/************************* end of function *************************/  

double dot_real(double *vect_1, double *vect_2, int n)
  {
    double *fstop ;
    double  sum    ;

    sum = 0.0  ;
    fstop = vect_1 + n ;
    for( ; vect_1 <fstop ; vect_1++, vect_2++ )
    	sum += (*vect_1 * *vect_2) ;

    return(sum);
  }
/************************* end of function *************************/

int i_greater(int *p1, int *p2)
  { return ( (int)(*p1 - *p2) ); }
/************************* end of function *************************/

void zeroi(int n, int *v)
  {
    int  *end ;
    end = v+n;
    for ( ; v < end ; v++)
      *v &= 0;
    return;
  }
/************************* end of function *************************/

void minoni(int n, int *v)
  {
    int  *end;
    int  min;
    min = -1;
    end = v+n;
    for( ; v < end ; v++)
      *v |= min;
    return;
  }
/************************* end of function *************************/

void saxpy(double *v1, double *v2, double alpha, int n)
  {
    double  *end;
    end = v1 + n;
    if(n <= 0)
      {
    	printf(" n %d\n", n);
    	exit(1);
      }
    for( ; v1 < end ; v1++, v2++)
    	*v1 += *v2 * alpha;
    return;
  }
/************************* end of function *************************/

void copyi(int n, int *from, int *to)
  {
    int  i;
    for(i=0 ; i < n ; i++, to++, from++)
      *to = *from;
    return;
  }
/************************* end of function *************************/

int rcomp(double *p1, double *p2)
  { return( (*p1 < *p2) ? -1 : (*p1 > *p2) ); }
/************************* end of function *************************/


/* UNUSED FUNCTIONS */

void clear_real(double *array, int n)
  {
    double *last= array + n;
    while(array < last) *array++ = 0.0;
  }
/************************* end of function *************************/

int icomp(int *p1, int *p2)
  { return( (*p1 < *p2) ? -1 : (*p1 > *p2) ); }
/************************* end of function *************************/

int fcomp(float *p1, float *p2)
  { return( (*p1 < *p2) ? -1 : (*p1 > *p2) ); }
