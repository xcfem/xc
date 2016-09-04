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

// $Revision: 1.9 $
// $Date: 2005/11/29 22:42:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/NewtonRaphson.cpp,v $


// File: ~/OOP/analysis/algorithm/NewtonRaphson.C
//
// Written: fmk
// Created: Sun Sept 15 15:06:47: 1996
// Revision: A
//

// Description: This file contains the class definition for
// NewtonRaphson. NewtonRaphson is a class which uses the
// Newton-Raphson solution algorihm
// to solve the equations. No member functions are declared as virtual as
// it is not expected that this class will be subclassed.
//
// What: "@(#)NewtonRaphson.C, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/NewtonRaphson.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <utility/matrix/ID.h>
#include "solution/SoluMethod.h"

// Constructor
XC::NewtonRaphson::NewtonRaphson(SoluMethod *owr,int theTangentToUse)
  :NewtonBased(owr,EquiALGORITHM_TAGS_NewtonRaphson,theTangentToUse) {}


int XC::NewtonRaphson::solveCurrentStep(void)
  {
    // set up some pointers and check they are valid
    // NOTE this could be taken away if we set Ptrs as protecetd in superclass
    AnalysisModel *theAnaModel= getAnalysisModelPtr();
    IncrementalIntegrator *theIntegrator= getIncrementalIntegratorPtr();
    LinearSOE *theSOE= getLinearSOEPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();

    if((theAnaModel == 0) || (theIntegrator == 0) || (theSOE == 0) || (theTest == 0))
      {
        std::cerr << "WARNING NewtonRaphson::solveCurrentStep() - setLinks() has"
                  << "no se ha asignado modelo, integrador o sistema de ecuaciones.\n";
        return -5;
      }

    if(theIntegrator->formUnbalance() < 0)
      {
        std::cerr << "WARNING NewtonRaphson::solveCurrentStep() -"
                  << "the Integrator failed in formUnbalance()\n";
        return -2;
      }

    // set itself as the ConvergenceTest objects EquiSolnAlgo
    theTest->set_owner(getSoluMethod());
    if(theTest->start() < 0)
      {
        std::cerr << "NewtnRaphson::solveCurrentStep() -"
                  << "the ConvergenceTest object failed in start()\n";
        return -3;
      }

    int result = -1;
    int count = 0;
    do
      {

        if(tangent == INITIAL_THEN_CURRENT_TANGENT)
          {
            if(count == 0)
              {
                if(theIntegrator->formTangent(INITIAL_TANGENT) < 0)
                  {
                    std::cerr << "WARNING NewtonRaphson::solveCurrentStep() -";
                    std::cerr << "the Integrator failed in formTangent()\n";
                    return -1;
                  }
              }
            else
              {
                if(theIntegrator->formTangent(CURRENT_TANGENT) < 0)
                  {
                    std::cerr << "WARNING NewtonRaphson::solveCurrentStep() -";
                    std::cerr << "the Integrator failed in formTangent()\n";
                    return -1;
                  }
              }
          }
        else
          {
            if(theIntegrator->formTangent(tangent) < 0)
              {
                std::cerr << "WARNING NewtonRaphson::solveCurrentStep() -";
                std::cerr << "the Integrator failed in formTangent()\n";
                return -1;
              }
          }
        if(theSOE->solve() < 0)
          {
            std::cerr << "WARNING NewtonRaphson::solveCurrentStep() -";
            std::cerr << "the LinearSysOfEqn failed in solve()\n";
            return -3;
          }
        if(theIntegrator->update(theSOE->getX()) < 0)
          {
            std::cerr << "WARNING NewtonRaphson::solveCurrentStep() -";
            std::cerr << "the Integrator failed in update()\n";
            return -4;
          }

        if(theIntegrator->formUnbalance() < 0)
          {
            std::cerr << "WARNING NewtonRaphson::solveCurrentStep() -";
            std::cerr << "the Integrator failed in formUnbalance()\n";
            return -2;
          }

        result = theTest->test();
        this->record(count++); //Llama al método record(...) de todos los recorders definidos.
      }
    while(result == -1);

    if(result == -2)
      {
        std::cerr << "NewtonRaphson::solveCurrentStep() -"
                  << "the ConvergenceTest object failed in test()\n"
                  << "convergence test message: "
		  << theTest->getStatusMsg(1) << std::endl;
        return -3;
      }

    // note - if postive result we are returning what the convergence test returned
    // which should be the number of iterations
    return result;
  }

void XC::NewtonRaphson::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      s << "NewtonRaphson" << std::endl;
  }









