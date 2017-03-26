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
                                                                        
// $Revision: 1.8 $
// $Date: 2003/02/14 23:00:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/Newmark1.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/Newmark1.C
// 
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the implementation of the XC::Newmark1 class.
//
// What: "@(#) Newmark1.C, revA"

#include <solution/analysis/integrator/transient/newmark/Newmark1.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
XC::Newmark1::Newmark1(SoluMethod *owr)
  :NewmarkBase2(owr,INTEGRATOR_TAGS_Newmark1),
   c4(0.0) {}

//! @brief Constructor.
XC::Newmark1::Newmark1(SoluMethod *owr,double theGamma, double theBeta, bool dispFlag)
  :NewmarkBase2(owr,INTEGRATOR_TAGS_Newmark1,theGamma,theBeta),
   c4(0.0) {}

//! @brief Constructor.
XC::Newmark1::Newmark1(SoluMethod *owr,double theGamma, double theBeta,const RayleighDampingFactors &rF)
  :NewmarkBase2(owr,INTEGRATOR_TAGS_Newmark1,theGamma,theBeta,rF),
   c4(0.0) {}

int XC::Newmark1::initialize(void)
  { return 0; }

int XC::Newmark1::newStep(double deltaT)
  {

    if(beta == 0 || gamma == 0 )
      {
        std::cerr << "XC::Newton::newStep() - error in variable\n";
        std::cerr << "gamma = " << gamma << " beta= " << beta << std::endl;
        return -1;
      }

    if(deltaT <= 0.0)
      {
        std::cerr << "XC::Newmark1::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;	
      }

    // set the constants
    c1 = 1.0;
    c2 = gamma/(beta*deltaT);
    c3 = 1.0/(beta*deltaT*deltaT);
    c4 = gamma*deltaT;


    // set the new_ trial response quantities
    AnalysisModel *theModel = this->getAnalysisModelPtr();

    if(U.get().Size() == 0)
      {
        std::cerr << "XC::Newton::newStep() - domainChange() failed or hasn't been called\n";
        return -3;	
      }

    // determine predicted quantities at time t + delta t
    U.get().addVector(1.0, U.getDot(), deltaT);
    const double a1 = deltaT * deltaT * (.5 - beta);
    U.get().addVector(1.0, U.getDotDot(), a1);

    const double a2 = deltaT * (1 - gamma);
    U.getDot().addVector(1.0, U.getDotDot(),a2);

    U.getDotDot().Zero();

    Up= U;

    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());        

    // increment the time and apply the load
    double time = getCurrentModelTime();
    time +=deltaT;

    if(updateModel(time, deltaT) < 0)
      {
        std::cerr << "XC::Newmark1::newStep() - failed to update the domain\n";
        return -4;
      }
    return 0;
  }

int XC::Newmark1::revertToLastStep()
  { return this->domainChanged(); }

int XC::Newmark1::formEleTangent(FE_Element *theEle)
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


int XC::Newmark1::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();
    theDof->addMtoTang(c3);
    theDof->addCtoTang(c2);      
    return(0);
  }    



int XC::Newmark1::domainChanged(void)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const XC::Vector &x = theLinSOE->getX();
    int size = x.Size();

    setRayleighDampingFactors();
  
    if(U.get().Size() != size)
      {
        // create the new
        Up.resize(size);
        U.resize(size);
      }
    
    // now go through and populate U, Udot and Udotdot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel
    PopulateUs(myModel);
    return 0;
  }


int XC::Newmark1::update(const XC::Vector &deltaU)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(!theModel)
      {
        std::cerr << "WARNING XC::Newmark1::update() - no XC::AnalysisModel set\n";
        return -1;
      }	

    // check domainChanged() has been called, i.e. Ut will not be zero
    if(U.get().Size() == 0)
      {
        std::cerr << "WARNING XC::Newmark1::update() - domainChange() failed or not called\n";
        return -2;
      }	

    // check deltaU is of correct size
    if(deltaU.Size() != U.get().Size())
      {
        std::cerr << "WARNING XC::Newmark1::update() - Vectors of incompatable size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << deltaU.Size() << std::endl;
        return -3;
      }
    
    //  determine the response at t+delta t
    U.get()+= deltaU;

    (U.getDotDot())= (U.get());
    (U.getDotDot())-= (Up.get());
    (U.getDotDot())*= c3;

    (U.getDot())= (Up.getDot());
    U.getDot().addVector(1.0, U.getDotDot(),c4);
  
    // update the responses at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());        
    if(updateModel() < 0)
      {
        std::cerr << "XC::Newmark1::newStep() - failed to update the domain\n";
        return -4;
      }
    return 0;
  }    

//! @brief Send object members through the channel being passed as parameter.
int XC::Newmark1::sendData(CommParameters &cp)
  {
    int res= NewmarkBase2::sendData(cp);
    res+= cp.sendDouble(c4,getDbTagData(),CommMetaData(14));
    res+= cp.sendMovable(Up,getDbTagData(),CommMetaData(15));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Newmark1::recvData(const CommParameters &cp)
  {
    int res= NewmarkBase2::recvData(cp);
    res+= cp.receiveDouble(c4,getDbTagData(),CommMetaData(14));
    res+= cp.receiveMovable(Up,getDbTagData(),CommMetaData(15));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Newmark1::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(16);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Newmark1::recvSelf(const CommParameters &cp)
  {
    inicComm(16);
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

void XC::Newmark1::Print(std::ostream &s, int flag)
  {
    NewmarkBase2::Print(s,flag);
    s << "  gamma: " << gamma << "  beta: " << beta << std::endl;
    s << " c1: " << c1 << " c2: " << c2 << " c3: " << c3 << std::endl;
  }




