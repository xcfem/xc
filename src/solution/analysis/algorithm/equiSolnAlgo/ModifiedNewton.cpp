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

// $Revision: 1.7 $
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/ModifiedNewton.cpp,v $


// File: ~/OOP/analysis/algorithm/ModifiedNewton.C
//
// Written: fmk
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for
// ModifiedNewton. ModifiedNewton is a class which uses the
// Newton-Raphson solution algorihm
// to solve the equations. No member functions are declared as virtual as
// it is not expected that this class will be subclassed.
//
// What: "@(#)ModifiedNewton.C, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/ModifiedNewton.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/ID.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <utility/Timer.h>
#include "solution/SoluMethod.h"

// Constructor
XC::ModifiedNewton::ModifiedNewton(SoluMethod *owr,int theTangentToUse)
  :NewtonBased(owr,EquiALGORITHM_TAGS_ModifiedNewton,theTangentToUse) {}

//! @brief resuelve el paso actual.
int XC::ModifiedNewton::solveCurrentStep(void)
  {
    // set up some pointers and check they are valid
    // NOTE this could be taken away if we set Ptrs as protecetd in superclass
    AnalysisModel *theAnalysisModel= getAnalysisModelPtr();
    IncrementalIntegrator *theIncIntegratorr= getIncrementalIntegratorPtr();
    LinearSOE *theSOE= getLinearSOEPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();

    if((theAnalysisModel == 0) || (theIncIntegratorr == 0) || (theSOE == 0) || (theTest == 0))
      {
        std::cerr << "WARNING ModifiedNewton::solveCurrentStep() - no se ha asignado modelo, integrador o sistema de ecuaciones.\n";
        return -5;
      }

    // we form the tangent
    //    Timer timer1;
    // timer1.start();

    if(theIncIntegratorr->formUnbalance() < 0)
      {
        std::cerr << "WARNING XC::ModifiedNewton::solveCurrentStep() -";
        std::cerr << "the XC::Integrator failed in formUnbalance()\n";
        return -2;
      }


    if(theIncIntegratorr->formTangent(tangent) < 0)
      {
        std::cerr << "WARNING XC::ModifiedNewton::solveCurrentStep() -";
        std::cerr << "the XC::Integrator failed in formTangent()\n";
        return -1;
      }


    // set itself as the XC::ConvergenceTest objects XC::EquiSolnAlgo
    theTest->set_owner(getSoluMethod());
    if(theTest->start() < 0)
      {
        std::cerr << "XC::ModifiedNewton::solveCurrentStep() -";
        std::cerr << "the XC::ConvergenceTest object failed in start()\n";
        return -3;
      }

    // repeat until convergence is obtained or reach max num iterations
    int result = -1;
    int count = 0;
    do
      {
        //Timer timer2;
        //timer2.start();
        if(theSOE->solve() < 0)
          {
            std::cerr << "WARNING XC::ModifiedNewton::solveCurrentStep() -";
            std::cerr << "the LinearSysOfEqn failed in solve()\n";
            return -3;
          }
        //timer2.pause();
        //std::cerr << "TIMER::SOLVE()- " << timer2;

        if(theIncIntegratorr->update(theSOE->getX()) < 0)
          {
            std::cerr << "WARNING XC::ModifiedNewton::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in update()\n";
            return -4;
          }

        if(theIncIntegratorr->formUnbalance() < 0)
          {
            std::cerr << "WARNING XC::ModifiedNewton::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in formUnbalance()\n";
            return -2;
          }

        record(count++); //Llama al método record(...) de todos los recorders definidos.
        result = theTest->test();
      }
    while(result == -1);

    //timer1.pause();
    //std::cerr << "TIMER::solveCurrentStep - " << timer1;
    if(result == -2)
      {
        std::cerr << "XC::ModifiedNewton::solveCurrentStep() -";
        std::cerr << "the XC::ConvergenceTest object failed in test()\n";
        return -3;
      }

    return result;
  }

void XC::ModifiedNewton::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      { s << "ModifiedNewton"; }
  }
