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
// $Date: 2000/09/15 08:23:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/cg/ConjugateGradientSolver.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/ConjugateGradientSolver.h
//
// Written: fmk 
// Created: 06/00
// Revision: A
//
// Description: This file contains the class definition for 
// ConjugateGradientSolver. ConjugateGradientSolver is an abstract 
// that implements the method solve and which declares a method
// formAp to be pure virtual.
//
// What: "@(#) ConjugateGradientSolver.h, revA"

#ifndef ConjugateGradientSolver_h
#define ConjugateGradientSolver_h

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
namespace XC {
class LinearSOE;
class Vector;

//! @ingroup LinearSolver
//
//! @brief Objetos que se encargan de obtener la solución
//! de sistemas de ecuaciones lineales con matriz en banda
//! mediante el procedimiento del gradiente conjugado.
class ConjugateGradientSolver : public LinearSOESolver
  {
  private:
    Vector r, p, Ap, x;
    LinearSOE *theLinearSOE;
    double tolerance;

    ConjugateGradientSolver(int classTag, LinearSOE *theLinearSOE, double tol);
  public:
    

    virtual int setSize(void);    
    virtual int solve(void);
    virtual int formAp(const Vector &p, Vector &Ap) = 0;    
  };
} // end of XC namespace

#endif

