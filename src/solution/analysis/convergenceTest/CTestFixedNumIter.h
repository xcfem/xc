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
// $Date: 2005/12/15 00:13:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/convergenceTest/CTestFixedNumIter.h,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/05
// Revision: A

#ifndef CTestFixedNumIter_h
#define CTestFixedNumIter_h

#include <solution/analysis/convergenceTest/ConvergenceTest.h>

namespace XC {

//! @ingroup CTest
//
//! @brief This object performs a fixed number of iterations without
//! testing for convergence. This test is useful for hybrid simulation where
//! the residual error is corrected for.
class CTestFixedNumIter: public ConvergenceTest
  {
  public:
    // constructors
    CTestFixedNumIter(EntCmd *owr);
    CTestFixedNumIter(EntCmd *owr,int maxNumIter, int printFlag, int normType=2);
    
    ConvergenceTest *getCopy(void) const;
    
    virtual std::string getStatusMsg(const int &flag= 1) const;
    std::string getEnergyProductMessage(void) const;
    int test(void);
  };
} // end of XC namespace

#endif
