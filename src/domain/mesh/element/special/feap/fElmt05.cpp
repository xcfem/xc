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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:20 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/feap/fElmt05.cpp,v $
                                                                        
                                                                        
// File: ~/element/fortran/fElmt05.C
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the implementation for the XC::fElmt05 class.
//
// What: "@(#) fElement.C, revA"

#include "domain/mesh/element/special/feap/fElmt05.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>


XC::fElmt05::fElmt05(int tag, int nd1, int nd2, double E, double A, double rho)
:fElement(tag, ELE_TAG_fElmt05, 3, 2, 0, 0)
{
    (*data)(0) = A;
    (*data)(1) = E;
    (*data)(2) = rho;
    
    (*connectedNodes)(0) = nd1; 
    (*connectedNodes)(1) = nd2;   
}
    
XC::fElmt05::fElmt05()
:fElement(ELE_TAG_fElmt05)    
{
    // does nothing
}

extern "C" int elmt05_(double *d, double *ul, double *xl, int *ix, double *tl, 
                       double *s, double *r, int *ndf, int *ndm, int *nst, int *isw, 
		       double *dm, int *nen, int *n, int *nh1, int *nh2, int *nh3, 
		       double *h, double *ctan, int *ior, int *iow);

int
XC::fElmt05::invokefRoutine(double *d, double *ul, double *xl, int *ix, double *tl, 
			double *s, double *r, int ndf, int ndm, int nst, int isw, 
			double dm, int nen, int n, int nh1, int nh2, int nh3, 
			double *h, double *ctan, int ior, int iow)
{
    // check that the values are acceptable to the fortran subroutine
    if (nst != 4 || nen != 2 || dm != 2)
	return 0;
    
    elmt05_(d, ul, xl, ix, tl, s, r, ndf, ndm, nst, isw, dm,
            nen, n, nh1, nh2, nh3, h, ctan, ior, iow);
        
    return nst;
}

