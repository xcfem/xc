//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/InitialInterpolatedLineSearch.cpp,v $

// Written: fmk 
// Created: 11/01
// 
// What: "@(#)InitialInterpolatedLineSearch.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/InitialInterpolatedLineSearch.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>

XC::InitialInterpolatedLineSearch::InitialInterpolatedLineSearch(void)
  : LineSearch(LINESEARCH_TAGS_InitialInterpolatedLineSearch) {}

//! @brief Virtual constructor.
XC::LineSearch *XC::InitialInterpolatedLineSearch::getCopy(void) const
  { return new InitialInterpolatedLineSearch(*this); }

int XC::InitialInterpolatedLineSearch::search(double s0, 
				      double s1, 
				      LinearSOE &theSOE, 
				      IncrementalIntegrator &theIntegrator)
  {
  double s = s1;

  //initialize r = ratio of residuals 
  double r0 = 0.0;

  if ( s0 != 0.0 ) 
    r0 = fabs( s / s0 );
	
  if  (r0 <= tolerance )
    return 0; // Line Search Not Required Residual Decrease Less Than Tolerance

  double eta = 1.0;     //initial value of line search parameter
  double etaPrev = 1.0;
  double r = r0;

  const Vector &dU = theSOE.getX();

  int count = 0; //initial value of iteration counter 

  if (printFlag == 0) {
    std::cerr << "InitialInterpolated Line Search - initial       "
	 << "    eta : " << eta 
	 << " , Ratio |s/s0| = " << r0 << std::endl;
  }    


  // Solution procedure follows the one in Crissfields book.
  // (M.A. Crissfield, Nonlinear Finite XC::Element XC::Analysis of Solid and Structures, Wiley. 97).
  // NOTE: it is not quite linear interpolation/false-position/regula-falsi as eta(0) = 0.0
  // does not change. uses eta(i) = eta(i-1)*s0
  //                                -----------
  //                                s0 - s(i-1)  to compute eta(i)


  while ( r > tolerance  &&  count < maxIter ) {

    count++;
    
    eta *=  s0 / (s0 - s); 

    //-- want to put limits on eta(i)
    if (eta > maxEta)  eta = maxEta;
    if (r   > r0    )  eta =  1.0;
    if (eta < minEta)  eta = minEta;
   
    
    //dx = ( eta * dx0 ); 
    x= dU;
    x*= eta-etaPrev;
	    
    if (theIntegrator.update(x) < 0) {
      std::cerr << "WARNInG XC::InitialInterpolatedLineSearch::search() -";
      std::cerr << "the XC::Integrator failed in update()\n";	
      return -1;
    }
    
    if (theIntegrator.formUnbalance() < 0) {
      std::cerr << "WARNInG XC::InitialInterpolatedLineSearch::search() -";
      std::cerr << "the XC::Integrator failed in formUnbalance()\n";	
      return -2;
    }	

    //new_ residual
    const Vector &ResidI = theSOE.getB();
    
    //new_ value of s
    s = dU ^ ResidI;

    //new_ value of r 
    r = fabs( s / s0 ); 

    if (printFlag == 0) {
      std::cerr << "InitialInterpolated Line Search - iteration: " << count 
	   << " , eta(j) : " << eta
	   << " , Ratio |sj/s0| = " << r << std::endl;
    }    

    // reset the variables, also check not just hitting bounds over and over
    if (eta == etaPrev)
      count = maxIter;
    else
      etaPrev = eta;

  } //end while

  // set X in the SOE for the revised dU, needed for convergence tests
  x= dU;
  x*= eta;
  theSOE.setX(x);

  return 0;
}


