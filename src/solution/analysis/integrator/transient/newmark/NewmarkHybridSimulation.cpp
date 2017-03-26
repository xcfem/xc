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

// $Revision: 1.1 $
// $Date: 2005/12/19 22:39:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/NewmarkHybridSimulation.cpp,v $


// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/05
// Revision: A
//
// Description: This file contains the implementation of the XC::NewmarkHybridSimulation class.
//
// What: "@(#) NewmarkHybridSimulation.cpp, revA"

#include <solution/analysis/integrator/transient/newmark/NewmarkHybridSimulation.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
XC::NewmarkHybridSimulation::NewmarkHybridSimulation(SoluMethod *owr)
    : NewmarkBase2(owr,INTEGRATOR_TAGS_NewmarkHybridSimulation), theTest(nullptr), rFact(1.0) {}

//! @brief Constructor.
XC::NewmarkHybridSimulation::NewmarkHybridSimulation(SoluMethod *owr,double _gamma, double _beta,
    ConvergenceTest &theT)
  :NewmarkBase2(owr,INTEGRATOR_TAGS_NewmarkHybridSimulation,_gamma,_beta), theTest(&theT), rFact(1.0) {}

//! @brief Constructor.
XC::NewmarkHybridSimulation::NewmarkHybridSimulation(SoluMethod *owr,double _gamma, double _beta,ConvergenceTest &theT,const RayleighDampingFactors &rF)
  : NewmarkBase2(owr,INTEGRATOR_TAGS_NewmarkHybridSimulation,_gamma,_beta,rF), theTest(&theT), rFact(1.0) {}

int XC::NewmarkHybridSimulation::newStep(double deltaT)
  {
    if(beta == 0 || gamma == 0)
      {
        std::cerr << "XC::NewmarkHybridSimulation::newStep() - error in variable\n";
        std::cerr << "gamma = " << gamma << " beta = " << beta << std::endl;
        return -1;
      }
    
    if(deltaT <= 0.0)
      {
        std::cerr << "XC::NewmarkHybridSimulation::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;	
      }

    // get a pointer to the XC::AnalysisModel
    AnalysisModel *theModel = getAnalysisModelPtr();
    
    // set the constants
    c1= 1.0;
    c2= gamma/(beta*deltaT);
    c3= 1.0/(beta*deltaT*deltaT);
    
    if(U.get().Size()==0)
      {
        std::cerr << "NewmarkHybridSimulation::newStep() - domainChange() failed or hasn't been called\n";
        return -3;	
      }
    
    // set response at t to be that at t+deltaT of previous step
    Ut= U;        
    
    // increment the time and apply the load
    double time = getCurrentModelTime();
    time += deltaT;
    applyLoadModel(time);
    
    // determine new_ velocities and accelerations at t+deltaT
    const double a1 = (1.0 - gamma/beta); 
    const double a2 = (deltaT)*(1.0 - 0.5*gamma/beta);
    U.getDot().addVector(a1, Ut.getDotDot(), a2);
    
    const double a3 = -1.0/(beta*deltaT);
    const double a4 = 1.0 - 0.5/beta;
    U.getDotDot().addVector(a4, Ut.getDot(), a3);
    
    // set the trial response quantities for the nodes
    theModel->setVel(U.getDot());
    theModel->setAccel(U.getDotDot());
    
    return 0;
  }

int XC::NewmarkHybridSimulation::revertToLastStep(void)
  {
    // set response at t+deltaT to be that at t .. for next newStep
    if(U.get().Size())
      { U= Ut; }
    return 0;
  }

int XC::NewmarkHybridSimulation::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();
    
    if(statusFlag == CURRENT_TANGENT)
      {
        theEle->addKtToTang(c1);
        theEle->addCtoTang(c2);
        theEle->addMtoTang(c3);
      }
    else if(statusFlag == INITIAL_TANGENT)
      {
        theEle->addKiToTang(c1);
        theEle->addCtoTang(c2);
        theEle->addMtoTang(c3);
      }
    return 0;
  }

int XC::NewmarkHybridSimulation::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();

    theDof->addCtoTang(c2);
    theDof->addMtoTang(c3);  
    
    return 0;
  }    

int XC::NewmarkHybridSimulation::domainChanged()
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const Vector &x = theLinSOE->getX();
    int size = x.Size();
    
    setRayleighDampingFactors();
    
    if(Ut.get().Size() != size)
      {
        // create the new
        Ut.resize(size);
        U.resize(size);
      }
    
    // now go through and populate U, Udot and Udotdot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel
    PopulateUs(myModel);
    return 0;
  }

int XC::NewmarkHybridSimulation::update(const XC::Vector &deltaU)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel)
      {
        std::cerr << "WARNING XC::NewmarkHybridSimulation::update() - no XC::AnalysisModel set\n";
        return -1;
      }
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.get().Size() == 0)
      {
        std::cerr << "WARNING XC::NewmarkHybridSimulation::update() - domainChange() failed or not called\n";
        return -2;
      }
    
    // check deltaU is of correct size
    if(deltaU.Size() != U.get().Size())
      {
        std::cerr << "WARNING XC::NewmarkHybridSimulation::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << deltaU.Size() << std::endl;
        return -3;
      }
    
    // determine the displacement increment reduction factor
    rFact = 1.0/(theTest->getMaxNumTests() - theTest->getNumTests() + 1.0);

    //  determine the response at t+deltaT
    U.get()+= rFact*deltaU;
    
    U.getDot().addVector(1.0, deltaU, rFact*c2);
    
    U.getDotDot().addVector(1.0, deltaU, rFact*c3);
    
    // update the response at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());        
    if(updateModel() < 0)
      {
        std::cerr << "XC::NewmarkHybridSimulation::update() - failed to update the domain\n";
        return -4;
      }
    return 0;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::NewmarkHybridSimulation::sendData(CommParameters &cp)
  {
    int res= NewmarkBase2::sendData(cp);
    res+= cp.sendMovablePtr(theTest,getDbTagData(),PtrCommMetaData(14,15));
    res+= cp.sendMovable(Ut,getDbTagData(),CommMetaData(16));
    res+= cp.sendDouble(rFact,getDbTagData(),CommMetaData(17));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::NewmarkHybridSimulation::recvData(const CommParameters &cp)
  {
    int res= NewmarkBase2::recvData(cp);
    //XXX arreglar.
    //theTest= cp.receiveMovablePtr(theTest,getDbTagData(),PtrCommMetaData(14,15));
    res+= cp.receiveMovable(Ut,getDbTagData(),CommMetaData(16));
    res+= cp.receiveDouble(rFact,getDbTagData(),CommMetaData(17));
    return res;
  }

int XC::NewmarkHybridSimulation::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(18);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::NewmarkHybridSimulation::recvSelf(const CommParameters &cp)
  {
    inicComm(18);
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

void XC::NewmarkHybridSimulation::Print(std::ostream &s, int flag)
  {
    NewmarkBase2::Print(s,flag);
    s << "  gamma: " << gamma << "  beta: " << beta << std::endl;
    s << " c1: " << c1 << " c2: " << c2 << " c3: " << c3 << std::endl;
  }
