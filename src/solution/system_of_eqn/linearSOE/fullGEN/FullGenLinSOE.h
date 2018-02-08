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
                                                                        
// $Revision: 1.2 $
// $Date: 2001/12/07 00:17:52 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.h,v $
                                                                        
                                                                        
#ifndef FullGenLinSOE_h
#define FullGenLinSOE_h

// File: ~/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.h
//
// Written: fmk 
// Created: 02/97
// Revision: A
//
// Description: This file contains the class definition for FullGenLinSOE
// FullGenLinSOE is a subclass of LinearSOE. It stores all the components
// of the linear system of equation in 1d arrays.
//
// What: "@(#) FullGenLinSOE.h, revA"

#include "solution/system_of_eqn/linearSOE/FactoredSOEBase.h"
#include "utility/matrix/Vector.h"


namespace XC {
class FullGenLinSolver;

//! @ingroup SOE
//
//! @brief Base class for dense unsymmetric matrix systems of equations.
//!
//! FullGenLinSOE is class which is used to store a full general
//! system. The \f$A\f$ matrix is stored in a 1d double array with \f$n*n\f$
//! elements, where \f$n\f$ is the size of the system. \f$A_{i,j}\f$ is stored
//! at location \f$(i + j*(n)\f$, where \f$i\f$ and \f$j\f$ range
//! from \f$0\f$ to \f$n-1\f$, i.e. C notation. For example when \f$n=3\f$: 
//! 
//! \f$\f$
//! \left[
//! \begin{array}{ccc}
//! a_{0,0} & a_{0,1}  & a_{0,2}
//! a_{1,0} & a_{1,1} & a_{1,2}
//! a_{2,0} & a_{2,1} & a_{2,2}
//! \end{array}
//! \right] 
//! \f$\f$
//! 
//! is stored in:
//! 
//! \f$\f$
//! \left[
//! \begin{array}{cccccccccccccccccccc}
//! a_{0,0} & a_{1,0}  & a_{2,0} & a_{0,1} & a_{1,1} & a_{2,1} &
//! a_{0,2} & a_{1,2} & a_{2,2}
//! \end{array}
//! \right] 
//! \f$\f$
//! 
//! The \f$x\f$ and \f$b\f$ vectors are stored in 1d double arrays of length
//! \f$n\f$. To allow the solvers access to this data, the solvers that use
//! this class are all declared as friend classes.
class FullGenLinSOE : public FactoredSOEBase
  {
  private:
    Vector A; //!< Storage for the system matrix.
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    FullGenLinSOE(AnalysisAggregation *);        
    FullGenLinSOE(AnalysisAggregation *,int N);        
    SystemOfEqn *getCopy(void) const;
  public:
    int setSize(Graph &theGraph);
    int addA(const Matrix &, const ID &, double fact = 1.0);
    
    void zeroA(void);
    
    friend class FullGenLinLapackSolver;    

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace


#endif

