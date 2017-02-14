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
// Created: Febuary 1997
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


//! @ingroup SOE
//
//! @brief Base class for band matrix system of equations.
class BandSPDLinSOE: public FactoredSOEBase
  {
  protected:
    int half_band;    
    Vector A;
    int aFactored;

    void inicA(const size_t &);
    bool setSolver(LinearSOESolver *);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    BandSPDLinSOE(SoluMethod *);    
    BandSPDLinSOE(SoluMethod *,int classTag);    
    BandSPDLinSOE(SoluMethod *,int N, int bandwidth, BandSPDLinSolver *the_Solver= NULL);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);

    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    
    virtual void zeroA(void);
    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    friend class BandSPDLinSolver;
    friend class BandSPDLinLapackSolver;    
    friend class BandSPDLinThreadSolver;        
    
  };
inline SystemOfEqn *BandSPDLinSOE::getCopy(void) const
  { return new BandSPDLinSOE(*this); }
} // end of XC namespace


#endif

