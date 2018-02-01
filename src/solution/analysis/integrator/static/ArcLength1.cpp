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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:00:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/ArcLength1.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/ArcLength1.C
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for XC::ArcLength1.
// ArcLength1 is an algorithmic class for perfroming a static analysis
// using the arc length scheme, that is within a load step the follwing
// constraint is enforced: dU^TdU + alpha^2*dLambda^2 = ArcLength1^2
// where dU is change in nodal displacements for step, dLambda is
// change in applied load and XC::ArcLength1 is a control parameter.
//
// What: "@(#) ArcLength1.C, revA"


#include <solution/analysis/integrator/static/ArcLength1.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>

//! @brief Constructor.
//!
//! @param owr: set of objects used to perform the analysis.
//! @param arcLength: value for the arc length.
//! @param alpha: scaling factor on the reference loads.
XC::ArcLength1::ArcLength1(AnalysisAggregation *owr,double arcLength, double alpha)
  : ArcLengthBase(owr,INTEGRATOR_TAGS_ArcLength1,arcLength,alpha) {}

//! @brief Virtual constructor.
XC::Integrator *XC::ArcLength1::getCopy(void) const
  { return new ArcLength1(*this); }

//! @brief Returns dLambda value.
double XC::ArcLength1::getDLambdaUpdate(void) const
  {
    // determine delta lambda(i)
    const double a= vectors.getDeltaUstep()^vectors.getDeltaUbar();
    const double b= (vectors.getDeltaUstep()^vectors.getDeltaUhat())
                    + alpha2*vectors.getDeltaLambdaStep();
    if(b == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; zero denominator,"
		  << " alpha was set to 0.0 and zero reference load.\n";
        return -1;
      }
    return -a/b;
  }

//! The object sends to \f$s\f$ its type, the current value of \f$\lambda\f$, and
//! \f$\delta \lambda\f$. 
void XC::ArcLength1::Print(std::ostream &s, int flag)
  {
    ArcLengthBase::Print(s,flag);
    s << "  " << getClassName() << ": "
      << sqrt(arcLength2) <<  "  alpha: "
      << sqrt(alpha2) << std::endl;
  }
