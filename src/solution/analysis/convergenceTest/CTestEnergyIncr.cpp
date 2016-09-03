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

// $Revision: 1.6 $
// $Date: 2005/12/15 00:19:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/convergenceTest/CTestEnergyIncr.cpp,v $


// File: ~/convergenceTest/CTestEnergyIncr.C
//
// Written: fmk 
// Date: 09/98
// Revised:
//
// Purpose: This file contains the class implementation for XC::CTestEnergyIncr.
// A XC::CTestEnergyIncr object tests for convergence using the energy increment,
// which is 0.5 times the absolute value of the product of the rhs and 
// the solution vector of the XC::LinearSOE.

#include <solution/analysis/convergenceTest/CTestEnergyIncr.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>

XC::CTestEnergyIncr::CTestEnergyIncr(EntCmd *owr)	    	
  : ConvergenceTestTol(owr,CONVERGENCE_TEST_CTestEnergyIncr) {}

XC::CTestEnergyIncr::CTestEnergyIncr(EntCmd *owr,double theTol, int maxIter, int printIt, int normType)
  : ConvergenceTestTol(owr,CONVERGENCE_TEST_CTestEnergyIncr,theTol,maxIter,printIt,normType,maxIter) {}

XC::ConvergenceTest* XC::CTestEnergyIncr::getCopy(void) const
  { return new CTestEnergyIncr(*this); }

//! @brief Comprueba si se ha producido la convergencia.
int XC::CTestEnergyIncr::test(void)
  {
    // check to ensure the SOE has been set - this should not happen if the 
    // return from start() is checked
    if(!hasLinearSOE()) return -2;
    
    // check to ensure the algo does invoke start() - this is needed otherwise
    // may never get convergence later on in analysis!
    if(currentIter == 0)
      {
        std::cerr << "WARNING: XC::CTestEnergyIncr::test() - start() was never invoked.\n";	
        return -2;
      }
    
    // determine the energy & save value in norms vector
    const Vector &b= getB();
    const Vector &x= getX();
    double product = x ^ b;
    if(product < 0.0)
      product *= -0.5;
    else
      product *= 0.5;
    
    if(currentIter <= maxNumIter) 
      norms(currentIter-1) = product;
    
    // print the data if required
    if(printFlag == 1)
      {
        std::clog << "XC::CTestEnergyIncr::test() - iteration: " << currentIter;
        std::clog << " current EnergyIncr: " << product << " (max: " << tol << ")\n";
      }
    if(printFlag == 4)
      {
        std::clog << "XC::CTestEnergyIncr::test() - iteration: " << currentIter;
        std::clog << " current EnergyIncr: " << product << " (max: " << tol << ")\n";
        std::clog << "\tNorm deltaX: " << x.pNorm(nType) << ", Norm deltaR: " << b.pNorm(nType) << std::endl;
        std::clog << "\tdeltaX: " << x << "\tdeltaR: " << b;
      }
    
    //
    // check if the algorithm converged
    //
    
    // if converged - print & return ok
    if(product <= tol)
      {
        // do some printing first
        if(printFlag != 0)
          {
            if(printFlag == 1 || printFlag == 4) 
              std::clog << std::endl;
            else if(printFlag == 2 || printFlag == 6)
              {
                std::clog << "XC::CTestEnergyIncr::test() - iteration: " << currentIter;
                std::clog << " last EnergyIncr: " << product << " (max: " << tol << ")\n";
              }
          }
        // return the number of times test has been called - SUCCESSFULL
        return currentIter;
      }
    // algo failed to converged after specified number of iterations - but RETURN OK
    else if((printFlag == 5 || printFlag == 6) && currentIter >= maxNumIter)
      {
        std::cerr << "WARNING: XC::CTestEnergyIncr::test() - failed to converge but goin on -";
        std::cerr << " current EnergyIncr: " << product << " (max: " << tol << ")\n";
        std::cerr << "\tNorm deltaX: " << x.pNorm(nType) << ", Norm deltaR: " << b.pNorm(nType) << std::endl;
        return currentIter;
      }
    // algo failed to converged after specified number of iterations - return FAILURE -2
    else if(currentIter >= maxNumIter)
      { // >= in case algorithm does not check
        std::cerr << "WARNING: XC::CTestEnergyIncr::test() - failed to converge \n";
        std::cerr << "after: " << currentIter << " iterations\n";	
        currentIter++;    
        return -2;
      } 
    // algorithm not yet converged - increment counter and return -1
    else
      {
        currentIter++;    
        return -1;
      }
  }




