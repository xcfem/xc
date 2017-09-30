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
                                                                        
// $Revision: 1.1 $
// $Date: 2001/12/07 01:02:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/LineSearch.cpp,v $
                                                                        
// Written: fmk 
// Created: 11/01

// Description: This file contains the class implementation for 
// LineSearch. 
// 
// What: "@(#)LineSearch.C, revA"

#include "LineSearch.h"
#include "utility/matrix/Vector.h"
#include "solution/system_of_eqn/linearSOE/LinearSOE.h"
#include "solution/analysis/integrator/IncrementalIntegrator.h"


XC::LineSearch::LineSearch(int clasTag,const double &tol, const int &mi,const double &mneta,const double &mxeta,const int &flag)
  :MovableObject(clasTag), tolerance(tol), maxIter(mi), minEta(mneta), maxEta(mxeta),printFlag(flag) {}

int XC::LineSearch::updateAndUnbalance(IncrementalIntegrator &theIntegrator)
  {
    int retval= 0;
    if(theIntegrator.update(x) < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; the integrator failed in update()\n";	
        retval= -1;
      }
    
    if(theIntegrator.formUnbalance() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "the integrator failed in formUnbalance()\n";	
        retval= -2;
      }
    return retval;
  }

int XC::LineSearch::newStep(LinearSOE &theSOE)
  {
    const Vector &dU = theSOE.getX();

    if(x.Size() != dU.Size())
      x= dU;
    return 0;
  }

int XC::LineSearch::sendSelf(CommParameters &cp)
  { return 0; }

int XC::LineSearch::recvSelf(const CommParameters &cp)
  { return 0; }

void XC::LineSearch::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      {
        s << getClassName() << " :: Line Search Tolerance = " << tolerance << std::endl; 
        s << "                         max num Iterations = " << maxIter << std::endl;
        s << "                         min value on eta = " << minEta << std::endl;
        s << "                         max value on eta = " << maxEta << std::endl;
      }
  }

