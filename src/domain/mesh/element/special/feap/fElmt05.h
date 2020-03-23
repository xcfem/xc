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
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/feap/fElmt05.h,v $
                                                                        
                                                                        
#ifndef fElmt05_h
#define fElmt05_h

// File: ~/element/fortran/fElmt05.h
// 
// Written: fmk 
// Created: 03/99
// Revision: A
//
// Description: This file contains the class definition for fElmt05. fElmt05
// is a wrapper used to call fortran element subroutine elmt05. elmt05 is a 
// linear elastic 2d element
//
// What: "@(#) fElmt05.h, revA"

#include <domain/mesh/element/special/feap/fElement.h>

namespace XC {
//! @ingroup fElem
//
//! @brief Interface with FEAP elements.
class fElmt05: public fElement
  {
  public:
    // constructor
    fElmt05(int tag,
	    int Nd1, int Nd2,
	    double E, double A, double rho = 0.0);  
    fElmt05();    
    
    virtual int getNumExternalNodes(void) const;
    virtual const ID &getExternalNodes(void) const;

    virtual int invokefRoutine(double *d, double *ul, double *xl, int *ix, 
			       double *tl, double *s, double *r, int ndf, 
			       int ndm, int nst, int isw, double dm, int nen, int n, 
			       int nh1, int nh2, int nh3, double *h,  
			       double *ctan, int ior, int iow);
  };
} // end of XC namespace

#endif



