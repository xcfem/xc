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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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

int XC::InitialInterpolatedLineSearch::search(double s0, 
				      double s1, 
				      LinearSOE &theSOE, 
				      IncrementalIntegrator &theIntegrator)
{
  double s = s1;

  //intialize r = ratio of residuals 
  double r0 = 0.0;

  if ( s0 != 0.0 ) 
    r0 = fabs( s / s0 );
	
  if  (r0 <= tolerance )
    return 0; // Line Search Not Required Residual Decrease Less Than Tolerance

  double eta = 1.0;     //initial value of line search parameter
  double etaPrev = 1.0;
  double r = r0;

  const XC::Vector &dU = theSOE.getX();

  int count = 0; //intial value of iteration counter 

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


