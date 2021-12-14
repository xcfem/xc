/*
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
*/
/*
 * File:  utility.h
 * ================
 * This file contains the interface to the utility routines implemented in
 * utility.c.
 *
 * Originally written by:  David R. Mackay
 *
 * Modified by:
 *  Jun Peng (junpeng@stanford.edu)
 *  Prof. Kincho H. Law
 *  Stanford University
 * --------------------
 */

#ifndef UTILITY_H
#define UTILITY_H

/*
 * Function:  clear_real
 * =====================
 * Arguments:  array => array of real's to be cleared to 0's
 *	       n => length of "array"
 * Return Values:  none
 * Notes:
 * Used in ... main.c
 */
void  clear_real(double *array, int n);


/*
 * Function:  dot_real
 * ===================
 *
 * Notes:  Program to dot a vector with a vector.
 * Used in ... newsub.c, nmat.c
 */
double  dot_real( double *vect_1, double *vect_2, int n );


/*
 * Function:  i_greater
 * ====================
 *
 * Used in ... nest.c
 */
int  i_greater( int *p1, int *p2 );


/*
 * Function:  zeroi
 * ================
 *
 * Used in ... grcm.c, nest.c, newadj.c, newordr.c, nnsim.c
 */
void  zeroi( int n, int *v );


/*
 * Function:  minoni
 * =================
 *
 * Used in ... newordr.c
 */
void  minoni( int n, int *v );


/*
 * Function:  saxpy
 * ================
 *
 * Notes:  v1 = v1 + alpha*v2
 * Used in ... nmat.c
 */
void  saxpy( double *v1, double *v2, double alpha, int n );


/*
 * Function:  copyi
 * ================
 *
 * Used in ... newadj.c
 */
void  copyi( int n, int *from, int *to );


/*
 * Function:  rcomp
 * ================
 *
 * Used in ... main.c
 */
int  rcomp( double *p1, double *p2 );



/* UNUSED FUNCTIONS */
/*
 * Function:  move_real
 * ====================
 * Arguments:  from => source array of real's
 *	       to => destination array of real's
 *	       n => length of array "to"
 * Return Values:  none
 * Notes:
 */
void  move_real(double *from, double *to, int n);

int  icomp( int *p1, int *p2 );
int  fcomp( float *p1, float *p2 );


#endif

