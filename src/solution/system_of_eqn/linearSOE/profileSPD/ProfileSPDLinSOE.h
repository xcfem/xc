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
// $Date: 2005/12/06 22:06:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.h,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for ProfileSPDLinSOE
// ProfileSPDLinSOE is a subclass of LinearSOE. It uses the LAPACK Upper storage
// scheme to store the components of the A matrix.

// What: "@(#) ProfileSPDLinSOE.h, revA"

#ifndef ProfileSPDLinSOE_h
#define ProfileSPDLinSOE_h

#include <solution/system_of_eqn/linearSOE/FactoredSOEBase.h>
#include "utility/matrix/Vector.h"
#include "utility/matrix/ID.h"

namespace XC {
class ProfileSPDLinSolver;

//! @ingroup SOE
//
//! @brief Profile matrix system of equations.
//!
//! ProfileSPDLinSOE is class which is used to store a symmetric
//! system of equations using a profile storage scheme. The upper
//! triangular part of \f$A\f$ is stored in a 1d double array with the
//! diagonals of \f$A\f$ located at positions given by an integer array
//! \f$iLoc\f$. For example when \f$n=5\f$ and \f$A\f$ as shown below:
//! 
//! \f$\f$
//! \left[
//! \begin{array}{ccccc}
//! a_{0,0} & a_{0,1}  & 0 & 0 & a_{0,4}
//! a_{1,0} & a_{1,1} & a_{1,2} & a_{1,3} & 0
//! a_{2,0} & a_{2,1} & a_{2,2} & a_{2,3} & a_{2,4}
//! 0 & a_{3,1} & a_{3,2} & a_{3,3} & a_{3,4}
//! 0 & 0 & a_{4,2} & a_{4,3} & a_{4,4}
//! \end{array}
//! \right] 
//! \f$\f$
//! 
//! this is stored using:
//! 
//! 
//! \f$\f$ A =
//! \left[
//! \begin{array}{cccccccccccccccccccc}
//! a_{0,0} & a_{0,1}  & a_{1,1} & a_{1,2} & a_{2,2} & a_{1,3} &
//! a_{2,3} & a_{3,3} & a_{0,4} & 0 & a_{2,4} & a_{3,4} & a_{4,4}
//! \end{array}
//! \right] 
//! \f$\f$
//! 
//! and 
//! 
//! \f$\f$ iLoc =
//! \left[
//! \begin{array}{cccccccccccccccccccc}
//! 1 & 3 & 5 & 8 & 13
//! \end{array}
//! \right] 
//! \f$\f$
//! Note \f$iLoc\f$ stores the diagonal locations using Fortran indexing. This
//! is to facilitate calls to Fortran libraries, e.g. Digital's DXML.
//! The \f$x\f$ and \f$b\f$ vectors are stored in 1d double arrays of
//! length \f$N\f$.
class ProfileSPDLinSOE : public FactoredSOEBase
  {
  protected:
    int profileSize;    
    Vector A;
    ID iDiagLoc;
    bool isAcondensed;
    int numInt;
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    ProfileSPDLinSOE(AnalysisAggregation *);
    ProfileSPDLinSOE(AnalysisAggregation *,int classTag);
    ProfileSPDLinSOE(AnalysisAggregation *,int N, int *iLoc, ProfileSPDLinSolver *theSolver= nullptr);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    
    virtual void zeroA(void);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    friend class ProfileSPDLinSolver;    
    friend class ProfileSPDLinDirectSolver;
    friend class ProfileSPDLinDirectBlockSolver;
    friend class ProfileSPDLinDirectThreadSolver;    
    friend class ProfileSPDLinDirectSkypackSolver;    
    friend class ProfileSPDLinSubstrSolver;
    friend class ProfileSPDLinSubstrThreadSolver;
  };
inline SystemOfEqn *ProfileSPDLinSOE::getCopy(void) const
  { return new ProfileSPDLinSOE(*this); }
} // end of XC namespace


#endif



