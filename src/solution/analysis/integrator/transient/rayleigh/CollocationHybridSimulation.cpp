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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/CollocationHybridSimulation.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/05
// Revision: A
//
// Description: This file contains the implementation of XC::CollocationHybridSimulation.
//
// What: "@(#) CollocationHybridSimulation.cpp, revA"

#include <solution/analysis/integrator/transient/rayleigh/CollocationHybridSimulation.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>

//! @brief Constructor.
XC::CollocationHybridSimulation::CollocationHybridSimulation(AnalysisAggregation *owr)
    : HHTBase(owr,INTEGRATOR_TAGS_CollocationHybridSimulation),
    theta(1.0), rFact(1.0), theTest(nullptr) {}

//! @brief Constructor.
XC::CollocationHybridSimulation::CollocationHybridSimulation(AnalysisAggregation *owr,double _theta,ConvergenceTest &theT)
  : HHTBase(owr,INTEGRATOR_TAGS_CollocationHybridSimulation,_theta,0.0,0.5),
    theta(_theta), rFact(1.0), theTest(&theT)
  {
    beta = -6.018722044382699e+002 * pow(theta,9) +
            6.618777151634235e+003 * pow(theta,8) +
           -3.231561059595987e+004 * pow(theta,7) +
            9.195359004558867e+004 * pow(theta,6) +
           -1.680788908312227e+005 * pow(theta,5) +
            2.047005794710718e+005 * pow(theta,4) +
           -1.661421563528177e+005 * pow(theta,3) +
            8.667950092619179e+004 * pow(theta,2) +
           -2.638652989051994e+004 * theta +
            3.572862280471971e+003;
  }

//! @brief Constructor.
XC::CollocationHybridSimulation::CollocationHybridSimulation(AnalysisAggregation *owr,double _theta, ConvergenceTest &theT,const RayleighDampingFactors &rF)
    : HHTBase(owr,INTEGRATOR_TAGS_CollocationHybridSimulation,_theta,0.0,0.5,rF),
      theta(_theta), rFact(1.0)
  {
    beta = -6.018722044382699e+002 * pow(theta,9) +
            6.618777151634235e+003 * pow(theta,8) +
           -3.231561059595987e+004 * pow(theta,7) +
            9.195359004558867e+004 * pow(theta,6) +
           -1.680788908312227e+005 * pow(theta,5) +
            2.047005794710718e+005 * pow(theta,4) +
           -1.661421563528177e+005 * pow(theta,3) +
            8.667950092619179e+004 * pow(theta,2) +
           -2.638652989051994e+004 * theta +
            3.572862280471971e+003;
  }

//! @brief Constructor.
XC::CollocationHybridSimulation::CollocationHybridSimulation(AnalysisAggregation *owr,double _theta,
    double _beta, double _gamma, ConvergenceTest &theT)
  : HHTBase(owr,INTEGRATOR_TAGS_CollocationHybridSimulation,_theta,_beta,_gamma),
    theta(_theta), rFact(1.0), theTest(&theT) {}

//! @brief Constructor.
XC::CollocationHybridSimulation::CollocationHybridSimulation(AnalysisAggregation *owr,double _theta,
    double _beta, double _gamma, ConvergenceTest &theT,const RayleighDampingFactors &rF)
    : HHTBase(owr,INTEGRATOR_TAGS_CollocationHybridSimulation,_theta,_beta,_gamma,rF),
    theta(_theta), rFact(1.0), theTest(&theT) {}


int XC::CollocationHybridSimulation::newStep(double _deltaT)
  {
    deltaT = _deltaT;
    if (theta <= 0.0 )  {
        std::cerr << "XC::CollocationHybridSimulation::newStep() - error in variable\n";
        std::cerr << "theta: " << theta << " <= 0.0\n";
        return -1;
    }
    
    if (deltaT <= 0.0)  {
        std::cerr << "XC::CollocationHybridSimulation::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;
    }
    
    // get a pointer to the XC::AnalysisModel
    AnalysisModel *theModel = this->getAnalysisModelPtr();

    // set the constants
    c1 = 1.0;
    c2 = gamma/(beta*theta*deltaT);
    c3 = 1.0/(beta*theta*theta*deltaT*deltaT);
    
    if (U.get().Size() == 0)  {
        std::cerr << "XC::CollocationHybridSimulation::newStep() - domainChange() failed or hasn't been called\n"; 
        return -3;	
    }
    
    // set response at t to be that at t+deltaT of previous step
    Ut= U;

    // increment the time to t+theta*deltaT and apply the load
    double time = getCurrentModelTime();
    time += theta*deltaT;
//    theModel->applyLoadDomain(time);
    if(updateModel(time, deltaT) < 0)
      {
        std::cerr << "XC::CollocationHybridSimulation::newStep() - failed to update the domain\n";
        return -4;
      }
    
    // determine new_ velocities and accelerations at t+theta*deltaT
    double a1 = (1.0 - gamma/beta); 
    double a2 = theta*deltaT*(1.0 - 0.5*gamma/beta);
    U.getDot().addVector(a1, Ut.getDotDot(), a2);
      
    double a3 = -1.0/(beta*theta*deltaT);
    double a4 = 1.0 - 0.5/beta;
    U.getDotDot().addVector(a4, Ut.getDot(), a3);
    
    // set the trial response quantities for the nodes
    theModel->setVel(U.getDot());
    theModel->setAccel(U.getDotDot());
    
    return 0;
}


int XC::CollocationHybridSimulation::revertToLastStep(void)
  {
    // set response at t+deltaT to be that at t .. for next step
    if(U.get().Size()>0)
      U= Ut;
    return 0;
  }


int XC::CollocationHybridSimulation::formEleTangent(FE_Element *theEle)
{
    theEle->zeroTangent();
    if (statusFlag == CURRENT_TANGENT)  {
        theEle->addKtToTang(c1);
        theEle->addCtoTang(c2);
        theEle->addMtoTang(c3);
    } else if (statusFlag == INITIAL_TANGENT)  {
        theEle->addKiToTang(c1);
        theEle->addCtoTang(c2);
        theEle->addMtoTang(c3);
    }
    
    return 0;
}


int XC::CollocationHybridSimulation::formNodTangent(DOF_Group *theDof)
{
    theDof->zeroTangent();

    theDof->addCtoTang(c2);
    theDof->addMtoTang(c3);
    
    return 0;
}    


int XC::CollocationHybridSimulation::domainChanged(void)
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
      }
    
    // now go through and populate U, Udot and Udotdot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel
    DOF_GrpIter &theDOFGroups = myModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while((dofGroupPtr = theDOFGroups()) != 0)
      {
        const XC::ID &id = dofGroupPtr->getID();
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


int XC::CollocationHybridSimulation::update(const XC::Vector &deltaU)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if (theModel == 0)  {
        std::cerr << "WARNING XC::CollocationHybridSimulation::update() - no XC::AnalysisModel set\n";
        return -1;
    }	
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if (Ut.get().Size() == 0)  {
        std::cerr << "WARNING XC::CollocationHybridSimulation::update() - domainChange() failed or not called\n";
        return -2;
    }	
    
    // check deltaU is of correct size
    if (deltaU.Size() != U.get().Size())  {
        std::cerr << "WARNING XC::CollocationHybridSimulation::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << deltaU.Size() << std::endl;
        return -3;
    }
    
    // determine the displacement increment reduction factor
    rFact = 1.0/(theTest->getMaxNumTests() - theTest->getNumTests() + 1.0);

    // determine the response at t+theta*deltaT
    (U.get()) += rFact*deltaU;

    U.getDot().addVector(1.0, deltaU, rFact*c2);
    
    U.getDotDot().addVector(1.0, deltaU, rFact*c3);
    
    // update the response at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());        
    if(updateModel() < 0)
      {
        std::cerr << "XC::CollocationHybridSimulation::update() - failed to update the domain\n";
        return -4;
      }
    return 0;
  }    


int XC::CollocationHybridSimulation::commit(void)
  {
    
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if (theModel == 0)  {
        std::cerr << "WARNING XC::CollocationHybridSimulation::commit() - no XC::AnalysisModel set\n";
        return -1;
    }	  
        
    // determine response quantities at t+deltaT
    U.getDotDot().addVector(1.0/theta, Ut.getDotDot(), (theta-1.0)/theta);
    
    (U.getDot()) = Ut.getDot();
    double a1 = deltaT*(1.0 - gamma);
    double a2 = deltaT*gamma;
    U.getDot().addVector(1.0, Ut.getDotDot(), a1);
    U.getDot().addVector(1.0, U.getDotDot(), a2);
    
    (U.get()) = Ut.get();
    U.get().addVector(1.0, Ut.getDot(), deltaT);
    double a3 = deltaT*deltaT*(0.5 - beta);
    double a4 = deltaT*deltaT*beta;
    U.get().addVector(1.0, Ut.getDotDot(), a3);
    U.get().addVector(1.0, U.getDotDot(), a4);

    // update the response at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());        
//    if (theModel->updateDomain() < 0)  {
//        std::cerr << "XC::CollocationHybridSimulation::commit() - failed to update the domain\n";
//        return -4;
//    }
    
    // set the time to be t+delta t
    double time = getCurrentModelTime();
    time += (1.0-theta)*deltaT;
    setCurrentModelTime(time);
    
    return commitModel();
  }

//! @brief Send object members through the communicator argument.
int XC::CollocationHybridSimulation::sendData(Communicator &comm)
  {
    int res= HHTBase::sendData(comm);
    res+= comm.sendDoubles(theta,rFact,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::CollocationHybridSimulation::recvData(const Communicator &comm)
  {
    int res= HHTBase::recvData(comm);
    res+= comm.receiveDoubles(theta,rFact,getDbTagData(),CommMetaData(9));
    return res;
  }


int XC::CollocationHybridSimulation::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::CollocationHybridSimulation::recvSelf(const Communicator &comm)
  {
    inicComm(7);
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


void XC::CollocationHybridSimulation::Print(std::ostream &s, int flag) const
  {
    HHTBase::Print(s,flag);
    s << "  theta: " << theta << std::endl;
  }
