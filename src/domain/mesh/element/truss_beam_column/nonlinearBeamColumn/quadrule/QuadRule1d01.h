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
// $Date: 2000/09/15 08:23:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/nonlinearBeamColumn/quadrule/QuadRule1d01.h,v $
                                                                        
                                                                        
// File: ~/QuadRule/QuadRule1d01.h
//
// Written: rms
// Created: 12/98
// Revision: 
//
// Description: This file contains the class definition for 
// QuadRule1d01 (Quadrature Rule 1dim). QuadRule1d01 is an abstract base class 
// and thus no objects of  it's type can be instatiated. It has pure 
// virtual functions which  must be implemented in it's derived classes.
//
// What: "@(#) QuadRule1d01.h, revA"


#ifndef QuadRule1d01_h
#define QuadRule1d01_h


#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/QuadRule.h>

namespace XC {
class Vector;
class Matrix;

//! @ingroup Quadrature
//
//! @brief One-dimensional quadrature.
class QuadRule1d01: public QuadRule
  {
  public:
    QuadRule1d01 ();
  };
} // end of XC namespace


#endif

