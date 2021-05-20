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
** (C) Copyright 2001, The Regents of the University of California    **
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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:00:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/LineSearch.h,v $
                                                                        
#ifndef LineSearch_h
#define LineSearch_h

// Written: fmk 
// Created: 11/01

// Description: This file contains the class definition for 
// LineSearch. LineSearch is an abstract base class, 
// i.e. no objects of it's type can be created.  Its subclasses seek
// to find a better solution to R(U)=0 than the solution Ui-1 + delta Ui
// would give, typically Ui = Ui-1 + factor * delta Ui.
// 
// What: "@(#)LineSearch.h, revA"

#include <utility/actor/actor/MovableObject.h>
#include "utility/kernel/CommandEntity.h"
#include <iostream>
#include "utility/matrix/Vector.h"

namespace XC {
class SolutionAlgorithm;
class IncrementalIntegrator;
class LinearSOE;

//! @ingroup AnalAlgo
//!
//! @defgroup EQSolAlgo Algorithms for solving system of equations.
//
//! @ingroup EQSolAlgo
//
//! @brief  Base class for line search algorithms.
//
//! Its subclasses seek to find a better solution to \f$R(U)=0\f$ than
//! the solution \f$U_{i-1} + \delta U_i\f$
//! would give, typically \f$U_i = U_{i-1} + factor * \delta U_i\f$.
class LineSearch: public MovableObject, public CommandEntity
  {
  protected:
    Vector x;
    double tolerance;
    int    maxIter;
    double minEta;
    double maxEta;
    int    printFlag;


    friend class NewtonLineSearch;
    LineSearch(int classTag,const double &tol= 0.8, const int &mi= 10,const double &mneta= 0.1,const double &mxeta= 10,const int &flag= 1);
    virtual LineSearch *getCopy(void) const= 0;
    int updateAndUnbalance(IncrementalIntegrator &);
  public:
    inline virtual ~LineSearch(void) {}
    // virtual functions
    virtual int newStep(LinearSOE &theSOE);
    virtual int search(double s0, 
		       double s1, 
		       LinearSOE &theSOE, 
		       IncrementalIntegrator &theIntegrator) =0;
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;    

  };
} // end of XC namespace

#endif


