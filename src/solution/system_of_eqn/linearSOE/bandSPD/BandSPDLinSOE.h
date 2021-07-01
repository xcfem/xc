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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/12/06 22:03:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.h,v $
                                                                        
                                                                        
#ifndef BandSPDLinSOE_h
#define BandSPDLinSOE_h

// File: ~/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.h
//
// Written: fmk 
// Created: February 1997
// Revision: A
//
// Description: This file contains the class definition for BandSPDLinSOE
// BandSPDLinSOE is a subclass of LinearSOE. It uses the LAPACK Upper storage
// scheme to store the components of the A matrix.
//
// What: "@(#) BandSPDLinSOE.h, revA"



#include <solution/system_of_eqn/linearSOE/FactoredSOEBase.h>
#include "utility/matrix/Vector.h"

namespace XC {
class BandSPDLinSolver;


/**
 @brief Base class for band matrix system of equations.
 @ingroup SOE

 BandSPDLinSOE is class which is used to store a banded symmetric system
 with ku superdiagonals. The \f$A\f$ matrix is stored in a
 1d double array with (ku+1)*n elements, where n is the size of the
 system. \f$A_{i,j}\f$ is stored at location \f$(ku+1+i-j) +
 j*(ku+1)\f$, where \f$i\f$ and \f$j\f$ range from \f$0\f$ to\f$n-1\f$,
 i.e. C notation. For example when \f$n=5\f$, \f$ku = 2\f$: 
 \f$
 \left[
 \begin{array}{ccccc}
  a_{0,0} & a_{0,1}  & a_{0,1} & 0 & 0\\
 a_{1,0} & a_{1,1} & a_{1,2} & a_{1,3} & 0\\
 a_{2,0} & a_{2,1} & a_{2,2} & a_{2,3} & a_{2,4}\\
 0 & a_{3,1} & a_{3,2} & a_{3,3} & a_{3,4}\\
 0 & 0 & a_{4,2} & a_{4,3} & a_{4,4}
 \end{array}
 \right] 
 \f$
 
 is stored in:
 
 \f$
 \left[
 \begin{array}{cccccccccccccccccccc}
 * & * & a_{0,0} & * & a_{0,1}  & a_{1,1} & a_{0,2} & a_{1,2} & a_{2,2} &
  a_{1,3} & a_{2,3} & a_{3,3} & a_{2,4} & a_{3,4} & a_{4,4}
 \end{array}
 \right] 
 \f$
 
 The \f$X\f$ and \f$B\f$ vectors are stored in 1d double arrays of
 length \f$N\f$.
*/
class BandSPDLinSOE: public FactoredSOEBase
  {
  protected:
    int half_band;    
    Vector A; //!< Storage for the matrix.

    void inicA(const size_t &);
    bool setSolver(LinearSOESolver *);

    friend class SolutionStrategy;
    friend class FEM_ObjectBroker;
    BandSPDLinSOE(SolutionStrategy *);    
    BandSPDLinSOE(SolutionStrategy *,int classTag);    
    BandSPDLinSOE(SolutionStrategy *,int N, int bandwidth, BandSPDLinSolver *the_Solver= nullptr);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);

    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    
    virtual void zeroA(void);
    
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
    
    friend class BandSPDLinSolver;
    friend class BandSPDLinLapackSolver;    
    friend class BandSPDLinThreadSolver;        
    
  };
} // end of XC namespace


#endif

