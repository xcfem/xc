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

// $Revision: 1.7 $
// $Date: 2005/10/19 21:53:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/ArcLength.cpp,v $


// File: ~/analysis/integrator/ArcLength.C
//
// Written: fmk
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for XC::ArcLength.
// ArcLength is an algorithmic class for perfroming a static analysis
// using the arc length scheme, that is within a load step the follwing
// constraint is enforced: dU^TdU + alpha^2*dLambda^2 = arcLength^2
// where dU is change in nodal displacements for step, dLambda is
// change in applied load and arcLength is a control parameter.
//
// What: "@(#) ArcLength.C, revA"


#include <solution/analysis/integrator/static/ArcLength.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>

//! @brief Constructor.
//!
//! @param owr: set of objects used to perform the analysis.
//! @param arcLength: value for the arc length.
//! @param alpha: scaling factor on the reference loads.
XC::ArcLength::ArcLength(AnalysisAggregation *owr,double arcLength, double alpha)
  :ArcLengthBase(owr,INTEGRATOR_TAGS_ArcLength,arcLength,alpha) {}

//! @brief Virtual constructor.
XC::Integrator *XC::ArcLength::getCopy(void) const
  { return new ArcLength(*this); }

//! @brief Returns the value of dLambda para el método update.
double XC::ArcLength::getDLambdaUpdate(void) const
  {
    const double &dLStep= vectors.getDeltaLambdaStep();
    const Vector &dUhat= vectors.getDeltaUhat();
    const Vector &dUstep= vectors.getDeltaUstep();
    const Vector &dUbar= vectors.getDeltaUbar();
    // determine the coeeficients of our quadratic equation
    const double a = alpha2 + (dUhat^dUhat);
    double b = alpha2*dLStep + (dUhat^dUbar) + (dUstep^dUhat);
    b *= 2.0;
    const double c = 2*(dUstep^dUbar) + (dUbar^dUbar);
    // check for a solution to quadratic
    double b24ac = b*b - 4.0*a*c;
    if(b24ac < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; imaginary roots due to multiple instability"
		  << " directions - initial load increment was too large\n"
		  << "a: " << a << " b: " << b << " c: " << c
		  << " b24ac: " << b24ac << std::endl;
        return -1;
      }
    const double a2 = 2.0*a;
    if(a2 == 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; zero denominator"
		  << " alpha was set to 0.0 and zero reference load\n";
        return -2;
      }

    // determine the roots of the quadratic
    const double sqrtb24ac = sqrt(b24ac);
    const double dlambda1= (-b + sqrtb24ac)/a2;
    const double dlambda2= (-b - sqrtb24ac)/a2;

    const double val = dUhat^dUstep;
    double theta1 = (dUstep^dUstep) + (dUbar^dUstep);
    //    const double theta2 = theta1 + dlambda2*val;
    theta1+= dlambda1*val;

    // choose dLambda based on angle between incremental displacement before
    // and after this step -- want positive
    double retval= 0;
    if(theta1 > 0)
      retval= dlambda1;
    else
      retval= dlambda2;
    return retval;
  }

//! @brief Prints stuff.
//!
//! The object sends to \f$s\f$ its type, the current value of \f$\lambda\f$,
//! and \f$\delta \lambda\f$. 
void XC::ArcLength::Print(std::ostream &s, int flag)
  {
    ArcLengthBase::Print(s,flag);
    s << "  arcLength: " << sqrt(arcLength2) <<  "  alpha: ";
    s << sqrt(alpha2) << std::endl;
  }
