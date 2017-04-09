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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/BisectionLineSearch.cpp,v $

// Written: fmk 
// Created: 11/01
// 
// What: "@(#)BisectionLineSearch.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/BisectionLineSearch.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <cmath>

//! @brief Constructor.
XC::BisectionLineSearch::BisectionLineSearch(void)
  :LineSearch(LINESEARCH_TAGS_BisectionLineSearch) {}

int XC::BisectionLineSearch::search(double s0, double s1, LinearSOE &theSOE, IncrementalIntegrator &theIntegrator)
  {
    double r0 = 0.0;
    if( s0 != 0.0 ) 
      r0 = fabs( s1 / s0 );
	
    if(r0 <= tolerance )
      return 0; // Line Search Not Required Residual Decrease Less Than Tolerance

    if(s1 == s0)
      return 0;  // Bisection will have a divide-by-zero error if continue

    // set some variables
    double eta= 1.0;
    double s= s1;
    double etaU= 1.0;
    double etaL= 0.0;
    double sU= s1;
    double sL= s0;
    double r= r0;
    double etaJ= 1.0;

    const Vector &dU = theSOE.getX();

    if(printFlag == 0)
      {
        std::cerr << "Bisection Line Search - initial: " 
	          << "     eta(0) : " << eta << " , Ratio |sj/s0| = " << r0 << std::endl;
      }

    // we first search for a bracket to a solution, i.e. we want sU * sL < 0.0
    int count = 0;
    while((sU * sL > 0.0) && (etaU < maxEta))
      {
        count++;
        etaU *= 2.0;

        //Updates la diferencia incremental and 
        //computes the new unbalanced vector.
        x= dU;
        x*= etaU-etaJ;
	    
        const int tmp= updateAndUnbalance(theIntegrator);
        if(tmp!=0)
          return tmp;

        //new unbalanced vector
        const Vector &ResidJ = theSOE.getB();
    
        //new value for sU
        sU = dU ^ ResidJ;

        // check if we have a solution we are happy with
        r= fabs( sU / s0 ); 
        if(r < tolerance)
          return 0;

        if(printFlag == 0)
          {
            std::clog << "Bisection Line Search - bracketing: " << count 
	              << " , eta(j) : " << etaU << " , Ratio |sj/s0| = " << r << std::endl;
          }
        etaJ = etaU;
      }

    // return if no bracket for a solution found, resetting to initial values
    if(sU * sL > 0.0)
      {
        x= dU;
        theSOE.setX(x);
        theIntegrator.update(x);
        theIntegrator.formUnbalance();
        return 0; 
      }

    // perform the secant iterations:
    //
    //                eta(j+1) = eta(l) + eta(u)
    //                           ---------------
    //                                2.0

    count = 0; //intial value of iteration counter 
    while( r > tolerance  &&  count < maxIter )
      {
        count++;

        eta = (etaU + etaL) * 0.5;

        //-- want to put limits on eta(i)
        if(r>r0)
          eta =  1.0;
    
        //Updates la diferencia incremental and 
        //computes the new unbalanced vector.
        x= dU;
        x*= eta-etaJ;
	    
        const int tmp= updateAndUnbalance(theIntegrator);
        if(tmp!=0)
          return tmp;

        //new unbalanced
        const Vector &ResidJ = theSOE.getB();
    
        //new value for s
        s= dU ^ ResidJ;
    
        //new value for r 
        r= fabs( s / s0 ); 

        // set variables for next iteration
        etaJ = eta;
    
        if(s*sU < 0.0)
          {
            etaL = eta;
            sL   = s;
          }
        else if(s*sU == 0.0)
          count = maxIter;
        else
          {
            etaU = eta;
            sU   = s;
          } 

        if(sL == sU)
          count = maxIter;

        if(printFlag == 0)
          {
            std::clog << "Bisection Line Search - iteration: " << count 
	              << " , eta(j) : " << eta << " , Ratio |sj/s0| = " << r << std::endl;
          }
      } //end while
    // set X in the SOE for the revised dU, needed for convergence tests
    x= dU;
    x*= eta;
    theSOE.setX(x);
    return 0;
  }
