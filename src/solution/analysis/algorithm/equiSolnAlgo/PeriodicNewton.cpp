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
// $Date: 2005/11/29 22:42:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/PeriodicNewton.cpp,v $

// Written: MHS
// Created: Oct 2002
//
// Description: This file contains the class definition for
// PeriodicNewton. PeriodicNewton is a class which uses the
// Newton-Raphson solution algorihm
// to solve the equations. No member functions are declared as virtual as
// it is not expected that this class will be subclassed.

#include <solution/analysis/algorithm/equiSolnAlgo/PeriodicNewton.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/ID.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include "solution/SoluMethod.h"

//! @brief Constructor
XC::PeriodicNewton::PeriodicNewton(SoluMethod *owr,int theTangentToUse, int mc)
  :NewtonBased(owr,EquiALGORITHM_TAGS_PeriodicNewton,theTangentToUse), maxCount(mc) {}

//! @brief resuelve el paso actual.
int XC::PeriodicNewton::solveCurrentStep(void)
  {
    // set up some pointers and check they are valid
    // NOTE this could be taken away if we set Ptrs as protecetd in superclass
    AnalysisModel *theAnalysisModel= getAnalysisModelPtr();
    IncrementalIntegrator *theIncIntegratorr= getIncrementalIntegratorPtr();
    LinearSOE *theSOE = getLinearSOEPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();

    if((theAnalysisModel == 0) || (theIncIntegratorr == 0) || (theSOE == 0)
        || (theTest == 0))
      {
        std::cerr << "WARNING PeriodicNewton::solveCurrentStep() - ";
        std::cerr << "no se ha asignado modelo, integrador o sistema de ecuaciones.\n";
        return -5;
      }

    // we form the tangent

    if(theIncIntegratorr->formUnbalance() < 0)
      {
        std::cerr << "WARNING XC::PeriodicNewton::solveCurrentStep() -";
        std::cerr << "the XC::Integrator failed in formUnbalance()\n";
        return -2;
      }

    if(theIncIntegratorr->formTangent(tangent) < 0)
      {
        std::cerr << "WARNING XC::PeriodicNewton::solveCurrentStep() -";
        std::cerr << "the XC::Integrator failed in formTangent()\n";
        return -1;
      }

    // set itself as the XC::ConvergenceTest objects XC::EquiSolnAlgo
    theTest->set_owner(getSoluMethod());
    if(theTest->start() < 0)
      {
        std::cerr << "XC::PeriodicNewton::solveCurrentStep() -";
        std::cerr << "the XC::ConvergenceTest object failed in start()\n";
        return -3;
      }

    // repeat until convergence is obtained or reach max num iterations
    int result = -1;
    int count = 0;
    int iter = 0;
    do
      {
        if(theSOE->solve() < 0)
          {
            std::cerr << "WARNING XC::PeriodicNewton::solveCurrentStep() -";
            std::cerr << "the LinearSysOfEqn failed in solve()\n";
            return -3;
          }

        if(theIncIntegratorr->update(theSOE->getX()) < 0)
          {
            std::cerr << "WARNING XC::PeriodicNewton::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in update()\n";
            return -4;
          }

        if(theIncIntegratorr->formUnbalance() < 0)
          {
            std::cerr << "WARNING XC::PeriodicNewton::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in formUnbalance()\n";
            return -2;
          }

        this->record(count++); //Llama al método record(...) de todos los recorders definidos.
        result = theTest->test();

        iter++;
        if(iter > maxCount)
          {
            if(theIncIntegratorr->formTangent(tangent) < 0)
              {
                std::cerr << "WARNING XC::PeriodicNewton::solveCurrentStep() -";
                std::cerr << "the XC::Integrator failed in formTangent()\n";
                return -1;
              }
            iter = 0;
          }
      }
    while (result == -1);

    if(result == -2)
      {
        std::cerr << "XC::PeriodicNewton::solveCurrentStep() -";
        std::cerr << "the XC::ConvergenceTest object failed in test()\n";
        return -3;
      }
    return result;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::PeriodicNewton::sendData(CommParameters &cp)
  {
    int res= NewtonBased::sendData(cp);
    res+= cp.sendDouble(maxCount,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::PeriodicNewton::recvData(const CommParameters &cp)
  {
    int res= NewtonBased::recvData(cp);
    double tmp= maxCount;
    res+= cp.receiveDouble(tmp,getDbTagData(),CommMetaData(3));
    maxCount= tmp;
    return res;
  }

int XC::PeriodicNewton::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::PeriodicNewton::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::PeriodicNewton::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      {
        s << "PeriodicNewton" << std::endl;
        s << "Max count: " << maxCount << std::endl;
      }
  }
