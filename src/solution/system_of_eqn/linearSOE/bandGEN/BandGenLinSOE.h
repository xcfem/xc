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
// $Date: 2005/12/06 21:56:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.h,v $
                                                                        
                                                                        
#ifndef BandGenLinSOE_h
#define BandGenLinSOE_h

// Written: fmk 
//
// Description: This file contains the class definition for BandGenLinSOE
// BandGenLinSOE is a subclass of LinearSOE. 
// What: "@(#) BandGenLinSOE.h, revA"


#include <solution/system_of_eqn/linearSOE/FactoredSOEBase.h>
#include "utility/matrix/Vector.h"

namespace XC {
class BandGenLinSolver;

//! @ingroup SOE
//
//! @brief Uses the LAPACK storage
//! scheme to store the components of the A matrix, which is a banded
//! unsymmetric matrix.
//!
//! BandGenLinSOE is class which is used to store a banded
//! unsymmetric system with kl subdiagonals and ku superdiagonals. The \f$A\f$
//! matrix is stored in a 1d double array with (kl+ku+1)*n elements, where
//! n is the size of the system. \f$A_{i,j}\f$ is stored at location
//! \f$(ku+1+i-j) + j*(ku+1+kl)\f$, where \f$i\f$ and \f$j\f$ range
//! from \f$0\f$ to\f$n-1\f$, i.e. C
//! notation. For example when \f$n=5\f$, \f$kl = 2\f$ and \f$ku=1\f$: 
//!
//! \f$\f$
//! \left[
//! \begin{array}{ccccc}
//! a_{0,0} & a_{0,1}  & 0 & 0 & 0
//! a_{1,0} & a_{1,1} & a_{1,2} & 0 & 0
//! a_{2,0} & a_{2,1} & a_{2,2} & a_{2,3} & 0
//! 0 & a_{3,1} & a_{3,2} & a_{3,3} & a_{3,4}
//! 0 & 0 & a_{4,2} & a_{4,3} & a_{4,4}
//! \end{array}
//! \right] 
//! \f$\f$
//! 
//! is stored in:
//! 
//! \f$\f$
//! \left[
//! \begin{array}{ccccccccccccccccccccc}
//! * & a_{0,0} & a_{1,0}  & a_{2,0} & a_{0,1} & a_{1,1} & a_{2,1} &
//! a_{3,1} & a_{1,2} & a_{2,2} & a_{3,2} & a_{4,2} & a_{2,3} & a_{3,3} &
//! a_{4,3} & * & a_{3,4} & a_{4,4} & * & *
//! \end{array}
//! \right] 
//! \f$\f$
//! 
//! The \f$x\f$ and \f$b\f$ vectors are stored in 1d double arrays of length
//! \f$N\f$. To allow the solvers access to this data, the solvers that use
//! this class are all declared as friend classes. 
class BandGenLinSOE: public FactoredSOEBase
  {
  protected:
    int numSuperD, numSubD;    
    Vector A;
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    BandGenLinSOE(AnalysisAggregation *);
    BandGenLinSOE(AnalysisAggregation *,int classTag);
    BandGenLinSOE(AnalysisAggregation *,int N, int numSuperDiagonals, int numSubDiagonal,BandGenLinSolver *theSolver= nullptr);        
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);
    
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);

    virtual void zeroA(void);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    friend class BandGenLinLapackSolver;
  };
inline SystemOfEqn *BandGenLinSOE::getCopy(void) const
  { return new BandGenLinSOE(*this); }
} // end of XC namespace


#endif

