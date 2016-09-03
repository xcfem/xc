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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/12/06 18:20:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.C
// 
// Written: fmk 
// Created: 10/00
// Revision: A
//
// Description: This file contains the implementation of the
// VariableTimeStepDirectIntegrationAnalysis class.
//
// What: "@(#) VariableTimeStepDirectIntegrationAnalysis.C, revA"

#include <solution/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/analysis/integrator/TransientIntegrator.h>
#include <domain/domain/Domain.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <cfloat>
#include "solution/SoluMethod.h"

//! @brief Constructor.
XC::VariableTimeStepDirectIntegrationAnalysis::VariableTimeStepDirectIntegrationAnalysis(SoluMethod *metodo)
  :DirectIntegrationAnalysis(metodo) {}    

//! @brief Ejecuta el análisis.
int XC::VariableTimeStepDirectIntegrationAnalysis::analyze(int numSteps, double dT, double dtMin, double dtMax, int Jd)
  {
    assert(metodo_solu);
    EntCmd *old= metodo_solu->Owner();
    metodo_solu->set_owner(this);

    // get some pointers
    Domain *theDom = this->getDomainPtr();
    EquiSolnAlgo *theAlgo= this->getEquiSolutionAlgorithmPtr();
    TransientIntegrator *theIntegratr = getTransientIntegratorPtr();
    ConvergenceTest *theTest= this->getConvergenceTestPtr();

    // set some variables
    int result = 0;  
    double totalTimeIncr = numSteps * dT;
    double currentTimeIncr = 0.0;
    double currentDt = dT;
  
    // loop until analysis has performed the total time incr requested
    while(currentTimeIncr < totalTimeIncr)
      {

        if(this->checkDomainChange() != 0)
          {
            std::cerr << "XC::VariableTimeStepDirectIntegrationAnalysis::analyze() - failed checkDomainChange\n";
            return -1;
          }

        //
        // do newStep(), solveCurrentStep() and commit() as in regular
        // DirectINtegrationAnalysis - difference is we do not return
        // if a failure - we stop the analysis & resize time step if failure
        //

        if(theIntegratr->newStep(currentDt) < 0)
          { result = -2; }
    
        if(result >= 0)
          {
            result = theAlgo->solveCurrentStep();
            if(result < 0) 
	      result = -3;
          }    

        if(result >= 0)
          {
            result = theIntegratr->commit();
            if(result < 0) 
	      result = -4;
          }

        // if the time step was successfull increment delta T for the analysis
        // othewise revert the XC::Domain to last committed state & see if can go on

        if(result >= 0) 
          currentTimeIncr += currentDt;
        else
          {
            // invoke the revertToLastCommit
            theDom->revertToLastCommit();	    
            theIntegratr->revertToLastStep();

            // if last dT was <= min specified the analysis FAILS - return FAILURE
            if(currentDt <= dtMin)
              {
	        std::cerr << "XC::VariableTimeStepDirectIntegrationAnalysis::analyze() - ";
	        std::cerr << " failed at time " << theDom->getTimeTracker().getCurrentTime() << std::endl;
                return result;
              }
            // if still here reset result for next loop
            result = 0;
          }
        // now we determine a new_ delta T for next loop
        currentDt = this->determineDt(currentDt, dtMin, dtMax, Jd, theTest);
      }
    metodo_solu->set_owner(old);
    return 0;
  }

//! @brief 
double XC::VariableTimeStepDirectIntegrationAnalysis::determineDt(double dT, double dtMin, double dtMax, int Jd, ConvergenceTest *theTest)
  {
    double newDt = dT;
    
    // get the number of trial steps in the last solveCurrentStep()
    double numLastIter = 1.0;
    if(theTest) numLastIter = theTest->getNumTests();
  
  
    // determine new_ dT based on last dT and Jd and #iter of last step
    double factor = Jd/numLastIter;
    newDt *= factor;
  
    // ensure: dtMin <~~ dT <= dtMax
    if(newDt < dtMin)
      newDt = dtMin - DBL_EPSILON;  // to ensure we get out of the analysis 
                               // loop if can't converge on next step
    else if(newDt > dtMax)
      newDt = dtMax;
    return newDt;
  }


