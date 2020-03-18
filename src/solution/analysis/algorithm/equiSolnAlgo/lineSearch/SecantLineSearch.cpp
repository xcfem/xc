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
// $Date: 2003/04/02 22:02:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/SecantLineSearch.cpp,v $

// Written: fmk 
// Created: 11/01
// 
// What: "@(#)SecantLineSearch.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/SecantLineSearch.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include <cmath>

XC::SecantLineSearch::SecantLineSearch(void)
  :LineSearch(LINESEARCH_TAGS_SecantLineSearch) {}

//! @brief Virtual constructor.
XC::LineSearch *XC::SecantLineSearch::getCopy(void) const
  { return new SecantLineSearch(*this); }

int XC::SecantLineSearch::search(double s0, double s1, LinearSOE &theSOE, IncrementalIntegrator &theIntegrator)
  {
    double r0 = 0.0;

    if(s0!=0.0) 
      r0 = fabs( s1 / s0 );
	
  if  (r0 <= tolerance )
    return 0; // Line Search Not Required Residual Decrease Less Than Tolerance

  if (s1 == s0)
    return 0;  // Secant will have a divide-by-zero if continue

  // set some variables
  double eta    = 1.0;
  double s      = s1;
  double etaJ   = 1.0;
  double etaJm1 = 0.0;
  double sJ     = s1;
  double sJm1   = s0;
  double r = r0;

  const XC::Vector &dU = theSOE.getX();

  if (printFlag == 0) {
    std::cerr << "Secant Line Search - initial: "
	 << "      eta(0) : " << eta << " , Ratio |s/s0| = " << r0 << std::endl;
  }

  // perform the secant iterations:
  //
  //                eta(j+1) = eta(j) -  s(j) * (eta(j-1)-eta(j))
  //                                     ------------------------
  //                                           s(j-1) - s(j)

  int count = 0; //initial value of iteration counter 
  while ( r > tolerance  &&  count < maxIter ) {
    
    count++;

    eta = etaJ - sJ * (etaJm1-etaJ) / (sJm1 - sJ);

    //-- want to put limits on eta and stop solution blowing up
    if (eta > maxEta)  eta = maxEta;
    if (r   > r0    )  eta =  1.0;
    if (eta < minEta)  eta = minEta;
    
    //updates the diferencia incremental y 
    //computes the new unbalanced vector.
    x= dU;
    x*= eta-etaJ;
	    
    const int tmp= updateAndUnbalance(theIntegrator);
    if(tmp!=0)
      return tmp;

    //new residual
    const Vector &ResidJ = theSOE.getB();
    
    //new_ value of s
    s = dU ^ ResidJ;
    
    //new_ value of r 
    r = fabs( s / s0 ); 

    if (printFlag == 0)
      {
      std::cerr << "Secant Line Search - iteration: " << count 
	   << " , eta(j) : " << eta << " , Ratio |sj/s0| = " << r << std::endl;
      }

    if (etaJ == eta)
      count = maxIter;

    // set variables for next iteration
    etaJm1 = etaJ;
    etaJ = eta;
    sJm1 = sJ;
    sJ = s;

    if (sJm1 == sJ)
      count = maxIter;
    
  } //end while

  // set X in the SOE for the revised dU, needed for convergence tests
  x= dU;
  x*= eta;
  theSOE.setX(x);

  return 0;
}








