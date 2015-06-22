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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/RegulaFalsiLineSearch.cpp,v $

// Written: fmk 
// Created: 11/01
// 
// What: "@(#)RegulaFalsiLineSearch.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/RegulaFalsiLineSearch.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include <cmath>

XC::RegulaFalsiLineSearch::RegulaFalsiLineSearch(void)
  :LineSearch(LINESEARCH_TAGS_RegulaFalsiLineSearch) {}

int XC::RegulaFalsiLineSearch::search(double s0, double s1, LinearSOE &theSOE, IncrementalIntegrator &theIntegrator)
{
  double r0 = 0.0;

  if ( s0 != 0.0 ) 
    r0 = fabs( s1 / s0 );
	
  if  (r0 <= tolerance )
    return 0; // Line Search Not Required Residual Decrease Less Than Tolerance

  if (s1 == s0)
    return 0;  // RegulaFalsi will have a divide-by-zero error if continue

  // set some variables
  double eta    = 1.0;
  double s      = s1;
  double etaU   = 1.0;
  double etaL   = 0.0;
  double sU     = s1;
  double sL     = s0;
  double r      = r0;
  double etaJ   = 1.0;


  const XC::Vector &dU = theSOE.getX();

  if (printFlag == 0) {
    std::cerr << "RegulaFalsi Line Search - initial: "
	 << "      eta(0) : " << eta << " , Ratio |s/s0| = " << r0 << std::endl;
  }

  // perform the secant iterations:
  //
  //                eta(j+1) = eta(u) -  s(u) * (eta(l) -eta(u))
  //                                     ------------------------
  //                                           s(l) - s(u)

  int count = 0; //intial value of iteration counter 
  while ( r > tolerance  &&  count < maxIter ) {
    
    count++;

    eta = etaU - sU * (etaL-etaU) / (sL - sU);


    //-- want to put limits on eta(i)
    if (eta > maxEta)  eta = maxEta;
    if (  r >  r0   )  eta =  1.0;
    if (eta < minEta)  eta = minEta;

    
    //actualiza la diferencia incremental y 
    //determina el nuevo vector de desequilibrio.
    x= dU;
    x*= eta-etaJ;
	    
    if (theIntegrator.update(x) < 0) {
      std::cerr << "WARNING XC::RegulaFalsiLineSearch::search() -";
      std::cerr << "the XC::Integrator failed in update()\n";	
      return -1;
    }
    
    if (theIntegrator.formUnbalance() < 0) {
      std::cerr << "WARNING XC::RegulaFalsiLineSearch::search() -";
      std::cerr << "the XC::Integrator failed in formUnbalance()\n";	
      return -2;
    }	

    //nuevo residuo
    const Vector &ResidJ = theSOE.getB();
    
    //nuevo valor de s
    s= dU ^ ResidJ;
    
    //nuevo valor de r 
    r= fabs( s / s0 ); 


    if(printFlag == 0)
      {
        std::cerr << "RegulaFalsi Line Search - iteration: " << count 
	     << " , eta(j) : " << eta 
             << " , Ratio |sj/s0| = " << r << std::endl;
      }
    

    if (etaJ == eta)
      count = maxIter;

    // set variables for next iteration
    etaJ = eta;
    
    if (s*sU < 0.0) {
      etaL = eta;
      sL   = s;
    } else if (s*sU == 0.0)
      count = maxIter;
    else {
      etaU = eta;
      sU   = s;
    } 

    if (sL == sU)
      count = maxIter;

  } //end while

  // set X in the SOE for the revised dU, needed for convergence tests
  x= dU;
  x*= eta;
  theSOE.setX(x);
  
  return 0;
}


