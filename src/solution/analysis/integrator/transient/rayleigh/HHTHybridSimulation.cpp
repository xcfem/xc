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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHTHybridSimulation.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/05
// Revision: A
//
// Description: This file contains the implementation of the XC::HHTHybridSimulation class.
//
// What: "@(#) HHTHybridSimulation.cpp, revA"

#include <solution/analysis/integrator/transient/rayleigh/HHTHybridSimulation.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>

//! @brief Constructor.
XC::HHTHybridSimulation::HHTHybridSimulation(AnalysisAggregation *owr)
  : HHTBase(owr,INTEGRATOR_TAGS_HHTHybridSimulation,1.0),
    alphaF(1.0), theTest(nullptr), rFact(1.0) {}

//! @brief Constructor.
XC::HHTHybridSimulation::HHTHybridSimulation(AnalysisAggregation *owr,double _rhoInf, ConvergenceTest &theT)
  : HHTBase(owr,INTEGRATOR_TAGS_HHTHybridSimulation,(2.0-_rhoInf)/(1.0+_rhoInf),1.0/(1.0+_rhoInf)/(1.0+_rhoInf),0.5*(3.0-_rhoInf)/(1.0+_rhoInf)), alphaF(1.0/(1.0+_rhoInf)),theTest(&theT),
    rFact(1.0)
  {}

//! @brief Constructor.
XC::HHTHybridSimulation::HHTHybridSimulation(AnalysisAggregation *owr,double _rhoInf, ConvergenceTest &theT,const RayleighDampingFactors &rF)
  : HHTBase(owr,INTEGRATOR_TAGS_HHTHybridSimulation,(2.0-_rhoInf)/(1.0+_rhoInf),1.0/(1.0+_rhoInf)/(1.0+_rhoInf),0.5*(3.0-_rhoInf)/(1.0+_rhoInf),rF), alphaF(1.0/(1.0+_rhoInf)),
    theTest(&theT), rFact(1.0) {}

//! @brief Constructor.
XC::HHTHybridSimulation::HHTHybridSimulation(AnalysisAggregation *owr,double _alphaI, double _alphaF, double _beta, double _gamma, ConvergenceTest &theT)
    : HHTBase(owr,INTEGRATOR_TAGS_HHTHybridSimulation,_alphaI,_beta,_gamma),
    alphaF(_alphaF), theTest(&theT), rFact(1.0) {}

//! @brief Constructor.
XC::HHTHybridSimulation::HHTHybridSimulation(AnalysisAggregation *owr,double _alphaI, double _alphaF,
    double _beta, double _gamma, ConvergenceTest &theT,const RayleighDampingFactors &rF)
    : HHTBase(owr,INTEGRATOR_TAGS_HHTHybridSimulation,_alphaI,_beta,_gamma,rF), alphaF(_alphaF),
    theTest(&theT), rFact(1.0) {}



int XC::HHTHybridSimulation::newStep(double _deltaT)
{
    deltaT = _deltaT;
    if (beta == 0 || gamma == 0 )  {
        std::cerr << "XC::HHTHybridSimulation::newStep() - error in variable\n";
        std::cerr << "gamma = " << gamma << " beta = " << beta << std::endl;
        return -1;
    }
    
    if (deltaT <= 0.0)  {
        std::cerr << "XC::HHTHybridSimulation::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;	
    }

    // get a pointer to the XC::AnalysisModel
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    
    // set the constants
    c1 = 1.0;
    c2 = gamma/(beta*deltaT);
    c3 = 1.0/(beta*deltaT*deltaT);
       
    if (U.get().Size() == 0)  {
        std::cerr << "XC::HHTHybridSimulation::newStep() - domainChange() failed or hasn't been called\n";
        return -3;	
    }
    
    // set response at t to be that at t+deltaT of previous step
    Ut= U;        

    // increment the time to t+alpha*deltaT and apply the load
    double time = getCurrentModelTime();
    time += alphaF*deltaT;
//    theModel->applyLoadDomain(time);
    if(updateModel(time, deltaT) < 0)
      {
        std::cerr << "XC::HHTHybridSimulation::newStep() - failed to update the domain\n";
        return -4;
      }

    // determine new_ velocities and accelerations at t+deltaT
    double a1 = (1.0 - gamma/beta);
    double a2 = deltaT*(1.0 - 0.5*gamma/beta);
    U.getDot().addVector(a1, Ut.getDotDot(), a2);
    
    double a3 = -1.0/(beta*deltaT);
    double a4 = 1.0 - 0.5/beta;  
    U.getDotDot().addVector(a4, Ut.getDot(), a3);
    
    // determine the velocities and accelerations at t+alpha*deltaT
    (Ualpha.getDot()) = Ut.getDot();
    Ualpha.get().addVector((1.0-alphaF), U.getDot(), alphaF);
    
    (Ualpha.getDotDot()) = Ut.getDotDot();
     Ualpha.getDotDot().addVector((1.0-alphaI()), U.getDotDot(), alphaI());

    // set the trial response quantities for the nodes
    theModel->setVel(Ualpha.getDot());
    theModel->setAccel(Ualpha.getDotDot());
        
    return 0;
}


int XC::HHTHybridSimulation::revertToLastStep()
  {
    // set response at t+deltaT to be that at t .. for next step
    if(U.get().Size() != 0)
      { U= Ut;    }
    return 0;
  }


int XC::HHTHybridSimulation::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();
    if (statusFlag == CURRENT_TANGENT)  {
        theEle->addKtToTang(alphaF*c1);
        theEle->addCtoTang(alphaF*c2);
        theEle->addMtoTang(alphaI()*c3);
    } else if (statusFlag == INITIAL_TANGENT)  {
        theEle->addKiToTang(alphaF*c1);
        theEle->addCtoTang(alphaF*c2);
        theEle->addMtoTang(alphaI()*c3);
    }
    
    return 0;
  }    


int XC::HHTHybridSimulation::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();

    theDof->addCtoTang(alphaF*c2);
    theDof->addMtoTang(alphaI()*c3);
    
    return 0;
  }


int XC::HHTHybridSimulation::domainChanged(void)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const Vector &x = theLinSOE->getX();
    int size = x.Size();
    
    setRayleighDampingFactors();
    
    if(Ut.get().Size() != size)
      {
        
        Ut.resize(size);
        U.resize(size);
        Ualpha.resize(size);
      }
    
    // now go through and populate U, Udot and Udotdot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel
    DOF_GrpIter &theDOFGroups = myModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while((dofGroupPtr = theDOFGroups()) != 0)
      {
        const ID &id = dofGroupPtr->getID();
        const Vector &disp = dofGroupPtr->getCommittedDisp();	
        U.setDisp(id,disp);
        const Vector &vel = dofGroupPtr->getCommittedVel();
        U.setVel(id,vel);
        const Vector &accel = dofGroupPtr->getCommittedAccel();
        U.setAccel(id,accel);
        // NOTE WE CAN't DO TOGETHER BECAUSE DOF_GROUPS USING SINGLE VECTOR
      }
    return 0;
  }


int XC::HHTHybridSimulation::update(const XC::Vector &deltaU)
{
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if (theModel == 0)  {
        std::cerr << "WARNING XC::HHTHybridSimulation::update() - no XC::AnalysisModel set\n";
        return -1;
    }	
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if (Ut.get().Size() == 0)  {
        std::cerr << "WARNING XC::HHTHybridSimulation::update() - domainChange() failed or not called\n";
        return -2;
    }	
    
    // check deltaU is of correct size
    if (deltaU.Size() != U.get().Size())  {
        std::cerr << "WARNING XC::HHTHybridSimulation::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << deltaU.Size() << std::endl;
        return -3;
    }
    
    // determine the displacement increment reduction factor
    rFact = 1.0/(theTest->getMaxNumTests() - theTest->getNumTests() + 1.0);

    //  determine the response at t+deltaT
    (U.get()) += rFact*deltaU;

    U.getDot().addVector(1.0, deltaU, rFact*c2);
    
    U.getDotDot().addVector(1.0, deltaU, rFact*c3);

    // determine displacement and velocity at t+alpha*deltaT
    (Ualpha.get()) = Ut.get();
    Ualpha.get().addVector((1.0-alphaF), U.get(), alphaF);

    (Ualpha.getDot()) = Ut.getDot();
    Ualpha.get().addVector((1.0-alphaF), U.getDot(), alphaF);
    
    (Ualpha.getDotDot()) = Ut.getDotDot();
    Ualpha.getDotDot().addVector((1.0-alphaI()), U.getDotDot(), alphaI());

    // update the response at the DOFs
    theModel->setResponse(Ualpha.get(),Ualpha.getDot(),Ualpha.getDotDot());
    if(updateModel() < 0)
      {
        std::cerr << "XC::HHTHybridSimulation::update() - failed to update the domain\n";
        return -4;
      }
    return 0;
  }


int XC::HHTHybridSimulation::commit(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)
      {
        std::cerr << "WARNING XC::HHTHybridSimulation::commit() - no XC::AnalysisModel set\n";
        return -1;
      }
    
    // update the response at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());
//    if (theModel->updateDomain() < 0)  {
//        std::cerr << "XC::HHTHybridSimulation::commit() - failed to update the domain\n";
//        return -4;
//    }
    
    // set the time to be t+deltaT
    double time = getCurrentModelTime();
    time+= (1.0-alphaF)*deltaT;
    setCurrentModelTime(time);

    return commitModel();
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::HHTHybridSimulation::sendData(Communicator &comm)
  {
    int res= HHTBase::sendData(comm);
    res+= comm.sendDouble(alphaF,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::HHTHybridSimulation::recvData(const Communicator &comm)
  {
    int res= HHTBase::recvData(comm);
    res+= comm.receiveDouble(alphaF,getDbTagData(),CommMetaData(9));
    return res;
  }

int XC::HHTHybridSimulation::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(10);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::HHTHybridSimulation::recvSelf(const Communicator &comm)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }


void XC::HHTHybridSimulation::Print(std::ostream &s, int flag) const
  {
    HHTBase::Print(s,flag);
    s << "  alphaI: " << alphaI() << " alphaF: " << alphaF  << " beta: " << beta  << " gamma: " << gamma << std::endl;
    s << "  c1: " << c1 << " c2: " << c2 << " c3: " << c3 << std::endl;
  }

