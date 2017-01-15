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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/NewmarkExplicit.cpp,v $


// File: ~/analysis/integrator/NewmarkExplicit.cpp
//
// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/05
// Revision: A
//
// Description: This file contains the implementation of the XC::NewmarkExplicit class.
//
// What: "@(#) NewmarkExplicit.cpp, revA"

#include <solution/analysis/integrator/transient/newmark/NewmarkExplicit.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>
#include "utility/actor/actor/ArrayCommMetaData.h"


//! @brief Constructor.
XC::NewmarkExplicit::NewmarkExplicit(SoluMethod *owr)
  :NewmarkBase(owr,INTEGRATOR_TAGS_NewmarkExplicit) {}

//! @brief Constructor.
XC::NewmarkExplicit::NewmarkExplicit(SoluMethod *owr,double _gamma)
  : NewmarkBase(owr,INTEGRATOR_TAGS_NewmarkExplicit,_gamma) {}

//! @brief Constructor.
XC::NewmarkExplicit::NewmarkExplicit(SoluMethod *owr,double _gamma,const RayleighDampingFactors &rF)
    : NewmarkBase(owr,INTEGRATOR_TAGS_NewmarkExplicit,_gamma,rF) {}


int XC::NewmarkExplicit::newStep(double deltaT)
  {
    updateCount = 0;

    if(gamma == 0)
      {
        std::cerr << "XC::NewmarkExplicit::newStep() - error in variable\n";
        std::cerr << "gamma = " << gamma << std::endl;
        return -1;
      }

    if(deltaT <= 0.0)
      {
        std::cerr << "XC::NewmarkExplicit::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;
      }

    // get a pointer to the XC::AnalysisModel
    AnalysisModel *theModel = this->getAnalysisModelPtr();

    // set the constants
    c2= gamma*deltaT;
    c3= 1.0;

    if(U.get().Size() == 0)
      {
        std::cerr << "XC::NewmarkExplicit::newStep() - domainChange() failed or hasn't been called\n";
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

    // set the trial response quantities for the elements
    theModel->setDisp(U.get());
    theModel->setVel(U.getDot());

    // increment the time and apply the load
    double time = getCurrentModelTime();
    time += deltaT;
    if(updateModel(time, deltaT) < 0)
      {
        std::cerr << "XC::NewmarkExplicit::newStep() - failed to update the domain\n";
        return -4;
      }

    // determine the accelerations at t+deltaT
    U.getDotDot().Zero();

    // set the trial response quantities for the nodes
    theModel->setAccel(U.getDotDot());

    return 0;
  }


int XC::NewmarkExplicit::revertToLastStep(void)
  {
    // set response at t+deltaT to be that at t .. for next step
    if(U.get().Size()>0)
      { U= Ut; }
    return 0;
  }


int XC::NewmarkExplicit::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();

    theEle->addCtoTang(c2);
    theEle->addMtoTang(c3);

    return 0;
  }


int XC::NewmarkExplicit::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();

    theDof->addCtoTang(c2);
    theDof->addMtoTang(c3);

    return 0;
  }


int XC::NewmarkExplicit::domainChanged(void)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const XC::Vector &x = theLinSOE->getX();
    int size = x.Size();

    setRayleighDampingFactors();

    if(Ut.get().Size() != size)
      {
        // create the new
        Ut.resize(size);
        U.resize(size);
      }

    // now go through and populate U, Udot and Udotdot by iterating through
    // the DOF_Groups and getting the last committed velocity and acceleration
    PopulateUs(myModel);
    return 0;
  }


int XC::NewmarkExplicit::update(const XC::Vector &aiPlusOne)
  {
    updateCount++;
    if(updateCount > 1)
      {
        std::cerr << "WARNING XC::NewmarkExplicit::update() - called more than once -";
        std::cerr << " NewmarkExplicit integration scheme requires a LINEAR solution algorithm\n";
        return -1;
      }

    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(!theModel)
      {
        std::cerr << "WARNING XC::NewmarkExplicit::update() - no XC::AnalysisModel set\n";
        return -1;
      }

    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.get().Size()==0)
      {
        std::cerr << "WARNING XC::NewmarkExplicit::update() - domainChange() failed or not called\n";
        return -2;
      }

    // check aiPlusOne is of correct size
    if(aiPlusOne.Size() != U.get().Size())
      {
        std::cerr << "WARNING XC::NewmarkExplicit::update() - Vectors of incompatible size ";
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

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::NewmarkExplicit::sendData(CommParameters &cp)
  {
    int res= NewmarkBase::sendData(cp);
    res+= cp.sendInt(updateCount,getDbTagData(),CommMetaData(13));
    res+= cp.sendMovable(Ut,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::NewmarkExplicit::recvData(const CommParameters &cp)
  {
    int res= NewmarkBase::recvData(cp);
    res+= cp.receiveInt(updateCount,getDbTagData(),CommMetaData(13));
    res+= cp.receiveMovable(Ut,getDbTagData(),CommMetaData(14));
    return res;
  }


int XC::NewmarkExplicit::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(23);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::NewmarkExplicit::recvSelf(const CommParameters &cp)
  {
    inicComm(23);
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


void XC::NewmarkExplicit::Print(std::ostream &s, int flag)
  {
    NewmarkBase::Print(s,flag);
    s << "  gamma: " << gamma << std::endl;
    s << "  c2: " << c2 << " c3: " << c3 << std::endl;
  }





