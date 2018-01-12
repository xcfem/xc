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
class FullGenLinSOE : public FactoredSOEBase
  {
  private:
    Vector A;
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
inline SystemOfEqn *FullGenLinSOE::getCopy(void) const
  { return new FullGenLinSOE(*this); }
} // end of XC namespace


#endif

