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

// $Revision: 1.13 $
// $Date: 2006/01/10 00:34:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/Newmark.cpp,v $

// Written : fmk
// Created : 11/98
// Modified: 02/05 ahs
// Revision: A
//
// Description: This file contains the implementation of the Newmark class.
//
// What: "@(#) Newmark.C, revA"

#include <solution/analysis/integrator/transient/newmark/Newmark.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>

#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
XC::Newmark::Newmark(SoluMethod *owr)
  : NewmarkBase2(owr,INTEGRATOR_TAGS_Newmark),
    displ(true), determiningMass(false) {}

//! @brief Constructor.
XC::Newmark::Newmark(SoluMethod *owr,double _gamma, double _beta, bool dispFlag)
  : NewmarkBase2(owr,INTEGRATOR_TAGS_Newmark,_gamma,_beta),
    displ(dispFlag), determiningMass(false) {}

//! @brief Constructor.
XC::Newmark::Newmark(SoluMethod *owr,double _gamma, double _beta,const RayleighDampingFactors &rF,bool dispFlag)
  : NewmarkBase2(owr,INTEGRATOR_TAGS_Newmark,_gamma,_beta,rF),
    displ(dispFlag), determiningMass(false) {}


int XC::Newmark::newStep(double deltaT)
  {
    if(beta == 0 || gamma == 0)
      {
        std::cerr << "XC::Newmark::newStep() - error in variable\n";
        std::cerr << "gamma = " << gamma << " beta = " << beta << std::endl;
        return -1;
      }
    
    if(deltaT <= 0.0)
      {
        std::cerr << "XC::Newmark::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;	
      }

    // get a pointer to the XC::AnalysisModel
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    
    // set the constants
    if(displ == true)
      {
        c1 = 1.0;
        c2 = gamma/(beta*deltaT);
        c3 = 1.0/(beta*deltaT*deltaT);
      }
    else
      {
        c1 = beta*deltaT*deltaT;
        c2 = gamma*deltaT;
        c3 = 1.0;
      }
    
    if(U.get().Size() == 0)
      {
        std::cerr << "XC::Newmark::newStep() - domainChange() failed or hasn't been called\n";
        return -3;	
      }
    
    // set response at t to be that at t+deltaT of previous step
    Ut= U;        
    
    if(displ == true)
      {        
        // determine new_ velocities and accelerations at t+deltaT
        double a1= (1.0 - gamma/beta); 
        double a2= (deltaT)*(1.0 - 0.5*gamma/beta);
        U.getDot().addVector(a1, Ut.getDotDot(), a2);
        
        double a3= -1.0/(beta*deltaT);
        double a4= 1.0 - 0.5/beta;
        U.getDotDot().addVector(a4, Ut.getDot(), a3);

        // set the trial response quantities
        theModel->setVel(U.getDot());
        theModel->setAccel(U.getDotDot());
      }
    else
      {
        // determine new_ displacements at t+deltaT      
        double a1 = (deltaT*deltaT/2.0);
        U.get().addVector(1.0,Ut.getDot(), deltaT);
        U.get().addVector(1.0,Ut.getDotDot(), a1);
        
        U.getDot().addVector(1.0, Ut.getDotDot(), deltaT);
    
        // set the trial response quantities
        theModel->setDisp(U.get());
        theModel->setVel(U.getDot());
      }   
    // increment the time and apply the load
    double time= getCurrentModelTime();
    time += deltaT;
    if(updateModel(time, deltaT) < 0)
      {
        std::cerr << "XC::Newmark::newStep() - failed to update the domain\n";
        return -4;
      }
    return 0;
  }


int XC::Newmark::revertToLastStep(void)
  {
    // set response at t+deltaT to be that at t .. for next newStep
    if(U.get().Size()>0)
      {
        U= Ut;        
      }
    return 0;
  }


int XC::Newmark::formEleTangent(FE_Element *theEle)
  {
    if(determiningMass == true)
      return 0;
    
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


int XC::Newmark::formNodTangent(DOF_Group *theDof)
  {
    if(determiningMass == true)
      return 0;
    
    theDof->zeroTangent();
    theDof->addCtoTang(c2);      
    theDof->addMtoTang(c3);
    
    return 0;
  }    


int XC::Newmark::domainChanged(void)
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
      }
    
    // now go through and populate U, Udot and Udotdot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel
    PopulateUs(myModel);
    return 0;
  }


int XC::Newmark::update(const XC::Vector &deltaU)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)
      {
        std::cerr << "WARNING XC::Newmark::update() - no XC::AnalysisModel set\n";
        return -1;
      }	
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.get().Size() == 0)
      {
        std::cerr << "WARNING XC::Newmark::update() - domainChange() failed or not called\n";
        return -2;
      }
    
    // check deltaU is of correct size
    if(deltaU.Size() != U.get().Size())
      {
        std::cerr << "WARNING XC::Newmark::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << deltaU.Size() << std::endl;
        return -3;
      }
    
    //  determine the response at t+deltaT
    if(displ == true)
      {
        U.get()+= deltaU;
        U.getDot().addVector(1.0, deltaU, c2);
        U.getDotDot().addVector(1.0, deltaU, c3);
      }
    else
      {
        U.get().addVector(1.0, deltaU, c1);
        U.getDot().addVector(1.0, deltaU, c2);
        (U.getDotDot())+= deltaU;
      }
    
    // update the response at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());
    if(updateModel() < 0)
      {
        std::cerr << "XC::Newmark::update() - failed to update the domain\n";
        return -4;
      }
    return 0;
  }    

//! @brief Send object members through the channel being passed as parameter.
int XC::Newmark::sendData(CommParameters &cp)
  {
    int res= NewmarkBase2::sendData(cp);
    res+= cp.sendBool(displ,getDbTagData(),CommMetaData(14));
    res+= cp.sendMovable(Ut,getDbTagData(),CommMetaData(15));
    res+= cp.sendBool(determiningMass,getDbTagData(),CommMetaData(16));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Newmark::recvData(const CommParameters &cp)
  {
    int res= NewmarkBase2::recvData(cp);
    res+= cp.receiveBool(displ,getDbTagData(),CommMetaData(14));
    res+= cp.receiveMovable(Ut,getDbTagData(),CommMetaData(15));
    res+= cp.receiveBool(determiningMass,getDbTagData(),CommMetaData(16));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Newmark::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(25);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Newmark::recvSelf(const CommParameters &cp)
  {
    inicComm(25);
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


void XC::Newmark::Print(std::ostream &s, int flag)
  {
    NewmarkBase2::Print(s,flag);
    s << "  gamma: " << gamma << "  beta: " << beta << std::endl;
    s << " c1: " << c1 << " c2: " << c2 << " c3: " << c3 << std::endl;
  }


// AddingSensitivity:BEGIN //////////////////////////////
int XC::Newmark::revertToStart(void)
  {
    Ut.Zero();
    U.Zero();
    return 0;
  }
// AddingSensitivity:END ////////////////////////////////

