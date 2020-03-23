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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/12/06 22:04:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandSPD/DistributedBandSPDLinSOE.h,v $
                                                                        
#ifndef DistributedBandSPDLinSOE_h
#define DistributedBandSPDLinSOE_h

// Written: fmk 
// Description: This file contains the class definition for DistributedBandSPDLinSOE
// DistributedBandSPDLinSOE is a subclass of LinearSOE. It uses the LAPACK storage
// scheme to store the components of the A matrix, which is a banded 
// unsymmetric matrix.
//
// What: "@(#) DistributedBandSPDLinSOE.h, revA"


#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.h>
#include "solution/system_of_eqn/linearSOE/DistributedBandLinSOE.h"
#include "utility/actor/channel/ChannelQueue.h"

namespace XC {
class DistributedBandSPDLinSolver;

//! @ingroup SOE
//
//! @brief Base class for band matrix distributed systems of equations.
class DistributedBandSPDLinSOE: public BandSPDLinSOE, public DistributedBandLinSOE
  {
  protected:
    void calcBand(Graph &);

    friend class AnalysisAggregation;
    DistributedBandSPDLinSOE(AnalysisAggregation *);
    SystemOfEqn *getCopy(void) const;
  public:
    // these methods need to be rewritten
    int addA(const Matrix &, const ID &, double fact = 1.0);
    int addB(const Vector &, const ID &,const double &fact= 1.0);    
    int setB(const Vector &, const double &fact = 1.0);            
    int setSize(Graph &theGraph);
    int solve(void);
    const Vector &getB(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    friend class BandSPDLinLapackSolver;

  };
inline SystemOfEqn *DistributedBandSPDLinSOE::getCopy(void) const
  { return new DistributedBandSPDLinSOE(*this); }
} // end of XC namespace


#endif

