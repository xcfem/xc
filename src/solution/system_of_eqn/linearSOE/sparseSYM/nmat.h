/*
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
*/
/*
 * File:  nmat.h
 * =============
 * altered to improve data access
 *
 * Originally written by:  David R. Mackay
 *
 * Modified by:
 *  Jun Peng (junpeng@stanford.edu)
 *  Prof. Kincho H. Law
 *  Stanford University
 * --------------------
 */


#ifndef nmat_h
#define nmat_h

#include "FeStructs.h"

int pfsfct(int neqns, double *diag, double **penv, int nblks, 
	   int *xblk, OFFDBLK **begblk, OFFDBLK *first, int *rowblks);

int pfefct(int neqns, double **penv, double *diag);

void pfsslv(int neqns, double *diag, double **penv, int nblks, 
	    int *xblk, double *rhs, OFFDBLK **begblk);

void pflslv (int neqns, double **penv, double *diag, double *rhs);

void pfuslv(int neqns, double **penv, double *diag, double *rhs);


#endif
