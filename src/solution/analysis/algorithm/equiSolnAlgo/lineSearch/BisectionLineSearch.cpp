//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

        //actualiza la diferencia incremental y 
        //determina el nuevo vector de desequilibrio.
        x= dU;
        x*= etaU-etaJ;
	    
        if(theIntegrator.update(x) < 0)
          {
            std::cerr << "WARNING XC::BisectionLineSearch::search() -";
            std::cerr << "the XC::Integrator failed in update()\n";	
            return -1;
          }

        if(theIntegrator.formUnbalance() < 0)
          {
            std::cerr << "WARNING XC::BisectionLineSearch::search() -";
            std::cerr << "the XC::Integrator failed in formUnbalance()\n";	
            return -2;
          }

        //nuevo residuo
        const Vector &ResidJ = theSOE.getB();
    
        //nuevo valor de sU
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
    
        //actualiza la diferencia incremental y 
        //determina el nuevo vector de desequilibrio.
        x= dU;
        x*= eta-etaJ;
	    
        if(theIntegrator.update(x) < 0)
          {
            std::clog << "WARNING XC::BisectionLineSearch::search() -";
            std::clog << "the XC::Integrator failed in update()\n";	
            return -1;
          }
    
        if(theIntegrator.formUnbalance() < 0)
          {
            std::cerr << "WARNING XC::BisectionLineSearch::search() -";
            std::cerr << "the XC::Integrator failed in formUnbalance()\n";	
            return -2;
          }

        //nuevo residuo
        const Vector &ResidJ = theSOE.getB();
    
        //nuevo valor de s
        s= dU ^ ResidJ;
    
        //nuevo valor de r 
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
