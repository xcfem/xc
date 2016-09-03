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
// $Source: /usr/local/cvs/OpenSees/SRC/convergenceTest/CTestNormUnbalance.cpp,v $


#include <solution/analysis/convergenceTest/CTestNormUnbalance.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>


XC::CTestNormUnbalance::CTestNormUnbalance(EntCmd *owr)	    	
  : ConvergenceTestTol(owr,CONVERGENCE_TEST_CTestNormUnbalance)
  {}


XC::CTestNormUnbalance::CTestNormUnbalance(EntCmd *owr,double theTol, int maxIter, int printIt, int normType)
  : ConvergenceTestTol(owr,CONVERGENCE_TEST_CTestNormUnbalance,theTol,maxIter,printIt,normType,maxIter)
  {}


XC::ConvergenceTest* XC::CTestNormUnbalance::getCopy(void) const
  { return new CTestNormUnbalance(*this); }

//! @brief Comprueba si se ha producido la convergencia.
int XC::CTestNormUnbalance::test(void)
  {
    int retval= 0;
    // check to ensure the SOE has been set - this should not happen if the 
    // return from start() is checked
    if(!hasLinearSOE())
      retval= -2;
    // check to ensure the algo does invoke start() - this is needed otherwise
    // may never get convergence later on in analysis!
    else if(currentIter == 0)
      {
        std::cerr << "WARNING: CTestNormUnbalance::test() - start() was never invoked.\n";	
        retval= -2;
      }
    else
      {
        // get the B vector & determine it's norm & save the value in norms vector
        const double normB= getNormB();
        const double normX= getNormX();
        const Vector &b= getB();
        const Vector &x= getX();
        if(currentIter <= maxNumIter) 
          norms(currentIter-1)= normB;
    
        // print the data if required
        if(printFlag == 1)
          {
            std::clog << "CTestNormUnbalance::test() - iteration: " << currentIter;
            std::clog << " current Norm: " << normB << " (max: " << tol;
            std::clog << ", Norm deltaX: " << normX << ")\n";
          }
        if(printFlag == 4)
          {
            std::clog << "CTestNormUnbalance::test() - iteration: " << currentIter;
            std::clog << " current Norm: " << normB << " (max: " << tol << ")\n";
            std::clog << "\tNorm deltaX: " << normX << ", Norm deltaR: " << normB << std::endl;
            std::clog << "\tdeltaX: " << x << "\tdeltaR: " << b;
          }
    
        //if(Bloque().size()>0) EjecutaBloque(Bloque(),getNombre()); //DEPRECATED Informa de los resultados.
        //check if the algorithm converged
            
        // if converged - print & return ok
        if(normB <= tol)
          {  
            // do some printing first
            if(printFlag != 0)
              {
                if(printFlag == 1 || printFlag == 4) 
                  std::clog << std::endl;
                else if(printFlag == 2 || printFlag == 6)
                  {
                    std::clog << "CTestNormUnbalance::test() - iteration: " << currentIter;
                    std::clog << " current Norm: " << normB << " (max: " << tol;
                    std::clog << ", Norm deltaX: " << x.pNorm(nType) << ")\n";
                  }
              }
            retval= currentIter; // return the number of times test has been called
          }
        // algo failed to converged after specified number of iterations - but RETURN OK
        else if((printFlag == 5 || printFlag == 6) && currentIter >= maxNumIter)
          {
            std::cerr << "WARNING: XC::CTestNormUnbalance::test() - failed to converge but going on -";
            std::cerr << " current Norm: " << normB << " (max: " << tol;
            std::cerr << ", Norm deltaX: " << x.pNorm(nType) << ")\n";
            retval= currentIter;
          }
        // algo failed to converged after specified number of iterations - return FAILURE -2
        else if(currentIter >= maxNumIter)
          { // the algorithm failed to converge
            std::cerr << "WARNING: XC::CTestNormUnbalance::test() - failed to converge \n";
            std::cerr << "after: " << currentIter << " iterations\n";	
            currentIter++;  // we increment in case analysis does not check for convergence
            retval= -2;
          }        
        else // algorithm not yet converged - increment counter and return -1
          {
            currentIter++;
            retval= -1;
          }
      }
    return retval;
  }
