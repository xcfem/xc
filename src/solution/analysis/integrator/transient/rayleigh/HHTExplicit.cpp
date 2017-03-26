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

// $Revision: 1.2 $
// $Date: 2005/12/21 00:32:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHTExplicit.cpp,v $


// File: ~/analysis/integrator/HHTExplicit.cpp
// 
// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/05
// Revision: A
//
// Description: This file contains the implementation of the XC::HHTExplicit class.
//
// What: "@(#) HHTExplicit.cpp, revA"

#include <solution/analysis/integrator/transient/rayleigh/HHTExplicit.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>

//! @brief Constructor.
XC::HHTExplicit::HHTExplicit(SoluMethod *owr)
    : HHTRayleighBase(owr,INTEGRATOR_TAGS_HHTExplicit) {}

//! @brief Constructor.
XC::HHTExplicit::HHTExplicit(SoluMethod *owr,double _alpha)
  : HHTRayleighBase(owr,INTEGRATOR_TAGS_HHTExplicit,_alpha,0.5) {}

//! @brief Constructor.
XC::HHTExplicit::HHTExplicit(SoluMethod *owr,double _alpha,const RayleighDampingFactors &rF)
    : HHTRayleighBase(owr,INTEGRATOR_TAGS_HHTExplicit,_alpha,0.5,rF) {}

//! @brief Constructor.
XC::HHTExplicit::HHTExplicit(SoluMethod *owr,double _alpha, double _gamma)
    : HHTRayleighBase(owr,INTEGRATOR_TAGS_HHTExplicit,_alpha,_gamma) {}

//! @brief Constructor.
XC::HHTExplicit::HHTExplicit(SoluMethod *owr,double _alpha, double _gamma,const RayleighDampingFactors &rF)
    : HHTRayleighBase(owr,INTEGRATOR_TAGS_HHTExplicit,_alpha,_gamma,rF) {}


int XC::HHTExplicit::newStep(double _deltaT)
  {
    updateCount = 0;

    deltaT = _deltaT;
    if(gamma == 0 )  {
        std::cerr << "XC::HHTExplicit::newStep() - error in variable\n";
        std::cerr << "gamma = " << gamma << std::endl;
        return -1;
    }
    
    if(deltaT <= 0.0)  {
        std::cerr << "XC::HHTExplicit::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;	
    }
    
    // get a pointer to the XC::AnalysisModel
    AnalysisModel *theModel = this->getAnalysisModelPtr();

    // set the constants
    c2 = gamma*deltaT;
    c3 = 1.0;
       
    if(U.get().Size() == 0)  {
        std::cerr << "XC::HHTExplicit::newStep() - domainChange() failed or hasn't been called\n";
        return -3;	
    }
    
    // set response at t to be that at t+deltaT of previous step
    Ut= U;

    // determine new_ displacements and velocities at time t+deltaT
    U.get().addVector(1.0, Ut.getDot(), deltaT);
    double a1 = 0.5*deltaT*deltaT;
    U.get().addVector(1.0, Ut.getDotDot(), a1);
    
    double a2 = deltaT*(1.0 - gamma);
    U.getDot().addVector(1.0, Ut.getDotDot(), a2);

    // determine the displacements and velocities at t+alpha*deltaT
    (Ualpha.get()) = Ut.get();
    Ualpha.get().addVector((1.0-alpha), U.get(), alpha);
    
    (Ualpha.getDot()) = Ut.getDot();
    Ualpha.get().addVector((1.0-alpha), U.getDot(), alpha);
        
    // set the trial response quantities for the elements
    theModel->setDisp(Ualpha.get());
    theModel->setVel(Ualpha.getDot());

    // increment the time to t+alpha*deltaT and apply the load
    double time = getCurrentModelTime();
    time += alpha*deltaT;
    if(updateModel(time, deltaT) < 0)
      {
        std::cerr << "XC::HHTExplicit::newStep() - failed to update the domain\n";
        return -4;
      }
    
    // determine the accelerations at t+alpha*deltaT
    U.getDotDot().Zero();
    
    // set the new_ trial response quantities for the nodes
    theModel->setAccel(U.getDotDot());
    
    return 0;
}


int XC::HHTExplicit::revertToLastStep()
  {
    // set response at t+deltaT to be that at t .. for next step
    if(U.get().Size()!=0)
      U= Ut;
    return 0;
  }


int XC::HHTExplicit::formEleTangent(FE_Element *theEle)
{
    theEle->zeroTangent();
    
    theEle->addCtoTang(alpha*c2);
    theEle->addMtoTang(c3);
    
    return 0;
}    


int XC::HHTExplicit::formNodTangent(DOF_Group *theDof)
{
    theDof->zeroTangent();

    theDof->addCtoTang(alpha*c2);
    theDof->addMtoTang(c3);
    
    return 0;
}


int XC::HHTExplicit::domainChanged()
{
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const XC::Vector &x = theLinSOE->getX();
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
    
    while ((dofGroupPtr = theDOFGroups()) != 0)
      {
        const XC::ID &id = dofGroupPtr->getID();
        const Vector &disp = dofGroupPtr->getCommittedDisp();	
        U.setDisp(id,disp);
        const Vector &vel = dofGroupPtr->getCommittedVel();
        U.setVel(id,vel);
        const Vector &accel = dofGroupPtr->getCommittedAccel();
        U.setAccel(id,accel);

        // NOTE WE CAN't DO TOGETHER BECAUSE DOF_GROUPS USING SINGLE VECTOR **
      }    
    
    return 0;
  }


int XC::HHTExplicit::update(const Vector &aiPlusOne)
{
    updateCount++;
    if(updateCount > 1)  {
        std::cerr << "WARNING XC::HHTExplicit::update() - called more than once -";
        std::cerr << " HHTExplicit integration scheme requires a LINEAR solution algorithm\n";
        return -1;
    }

    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)  {
        std::cerr << "WARNING XC::HHTExplicit::update() - no XC::AnalysisModel set\n";
        return -1;
    }	
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.get().Size() == 0)  {
        std::cerr << "WARNING XC::HHTExplicit::update() - domainChange() failed or not called\n";
        return -2;
    }	
    
    // check aiPlusOne is of correct size
    if(aiPlusOne.Size() != U.get().Size())  {
        std::cerr << "WARNING XC::HHTExplicit::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << aiPlusOne.Size() << std::endl;
        return -3;
    }
    
    //  determine the response at t+deltaT
    U.getDot().addVector(1.0, aiPlusOne, c2);

    (U.getDotDot()) = aiPlusOne;
    
    // update the response at the DOFs
    theModel->setVel(U.getDot());
    theModel->setAccel(U.getDotDot());
    
    return 0;
}


int XC::HHTExplicit::commit(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)  {
        std::cerr << "WARNING XC::HHTExplicit::commit() - no XC::AnalysisModel set\n";
        return -1;
    }	  
        
    // set the time to be t+deltaT
    double time = getCurrentModelTime();
    time += (1.0-alpha)*deltaT;
    setCurrentModelTime(time);

    return commitModel();
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::HHTExplicit::sendData(CommParameters &cp)
  {
    int res= HHTRayleighBase::sendData(cp);
    res+= cp.sendInt(updateCount,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::HHTExplicit::recvData(const CommParameters &cp)
  {
    int res= HHTRayleighBase::recvData(cp);
    res+= cp.receiveInt(updateCount,getDbTagData(),CommMetaData(8));
    return res;
  }

int XC::HHTExplicit::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(27);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::HHTExplicit::recvSelf(const CommParameters &cp)
  {
    inicComm(27);
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

void XC::HHTExplicit::Print(std::ostream &s, int flag)
  {
    RayleighBase::Print(s,flag);
    s << "  alpha: " << alpha << " gamma: " << gamma << std::endl;
    s << "  c2: " << c2 << " c3: " << c3 << std::endl;
  }



