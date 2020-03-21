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

// $Revision: 1.1 $
// $Date: 2007/11/29 19:31:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/eigenSOE/FullGenEigenSOE.h,v $


#ifndef FullGenEigenSOE_h
#define FullGenEigenSOE_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 11/07
// Revision: A
//
// Description: This file contains the class definition for
// FullGenEigenSOE, which stores full nonsymmetric matrices,
// A and M, for generalized eigenvalue computations.

#include "EigenSOE.h"
#include "utility/matrix/Vector.h"

namespace XC {
class FullGenEigenSolver;

//! @ingroup EigenSOE
//
//! @brief Dense matrix eigenproblem system of equations.
class FullGenEigenSOE: public EigenSOE
  {
  private:
    Vector A;
    Vector M;
  protected:
    bool setSolver(EigenSolver *);

    friend class AnalysisAggregation;
    friend class FullGenEigenSolver;
    FullGenEigenSOE(AnalysisAggregation *);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);

    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    virtual int addM(const Matrix &, const ID &, double fact = 1.0);    

    virtual void zeroA(void);
    virtual void zeroM(void);
    virtual void identityM(void);

    virtual void save(void) const;
    virtual void restore(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
inline SystemOfEqn *FullGenEigenSOE::getCopy(void) const
  { return new FullGenEigenSOE(*this); }
} // end of XC namespace

#endif
