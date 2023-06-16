//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
// $Date: 2009-03-20 18:36:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/TRBDFBase.cpp,v $

// Written : fmk
// Created : 02/09
//
// Description: This file contains the implementation of the TRBDFBase class.
// ref: K.J.Bathe, "Conserving Energy and Momentum in Nonlinear Dynamics: A Simple
//      Implicit Time Integration Scheme", Computers and Structures 85(2007),437-445
//
// NOTE: In the implementation we use deltaT as opposed to deltaT/2 in the paper.
//
// What: "@(#) TRBDFBase.C, revA"

#include "TRBDFBase.h"
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>


XC::TRBDFBase::TRBDFBase(SolutionStrategy *owr, int classTag)
  : TransientIntegrator(owr, classTag),
    step(0), dt(0.0),
    c1(0.0), c2(0.0), c3(0.0), 
    Utm1(0), Ut(0), U(0) {}

int XC::TRBDFBase::revertToLastStep()
  {
    // set response at t+deltaT to be that at t .. for next newStep
    U= Ut;        
    step = 1;
    return 0;
  }


int XC::TRBDFBase::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();
    
    if (statusFlag == CURRENT_TANGENT)
      {
        theEle->addKtToTang(c1);
        theEle->addCtoTang(c2);
        theEle->addMtoTang(c3);
      }
    else if (statusFlag == INITIAL_TANGENT)
      {
        theEle->addKiToTang(c1);
        theEle->addCtoTang(c2);
        theEle->addMtoTang(c3);
      }
    else if (statusFlag == HALL_TANGENT)
      {
        theEle->addKtToTang(c1*cFactor);
        theEle->addKiToTang(c1*iFactor);
        theEle->addCtoTang(c2);
        theEle->addMtoTang(c3);
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; - unknown FLAG\n";
      }
    return 0;
  }    


int XC::TRBDFBase::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();

    theDof->addCtoTang(c2);      
    theDof->addMtoTang(c3);
    
    return 0;
  }    

//! @brief Resize vector data.
void XC::TRBDFBase::resize(const size_t &size)
  {  
    Utm1.resize(size);
    Ut.resize(size);
    U.resize(size);
  }        

int XC::TRBDFBase::domainChanged(void)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const Vector &x = theLinSOE->getX();
    const size_t size = x.Size();
    
    // resize the Vector objects
    if(Ut.size() != size)
      { resize(size); }
    
    // now go through and populate U, U.getDot() and U.getDotDot() by iterating through
    // the DOF_Groups and getting the last committed velocity and accel
    DOF_GrpIter &theDOFGroups = myModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;

    while((dofGroupPtr = theDOFGroups()) != 0)
      {
        const ID &id= dofGroupPtr->getID();
        
        const Vector &disp= dofGroupPtr->getCommittedDisp();
	U.setDisp(id, disp);
        
        const Vector &vel= dofGroupPtr->getCommittedVel();
	U.setVel(id, vel);
        
        const Vector &accel= dofGroupPtr->getCommittedAccel();
	U.setAccel(id, accel);
      }
    
    return 0;
  }


const XC::Vector &XC::TRBDFBase::getVel()
  { return U.getDot(); }

//! @brief Send object members through the communicator argument.
int XC::TRBDFBase::sendData(Communicator &comm)
  {
    int res= TransientIntegrator::sendData(comm);
    res+= comm.sendInt(step, getDbTagData(), CommMetaData(3));
    res+= comm.sendDoubles(dt,c1,c2,c3,getDbTagData(),CommMetaData(4));
    res+= comm.sendMovable(Utm1,getDbTagData(),CommMetaData(5));
    res+= comm.sendMovable(Ut,getDbTagData(),CommMetaData(6));
    res+= comm.sendMovable(U,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::TRBDFBase::recvData(const Communicator &comm)
  {
    int res= TransientIntegrator::recvData(comm);
    res+= comm.receiveInt(step, getDbTagData(), CommMetaData(3));
    res+= comm.receiveDoubles(dt,c1,c2,c3,getDbTagData(),CommMetaData(4));
    res+= comm.receiveMovable(Utm1,getDbTagData(),CommMetaData(5));
    res+= comm.receiveMovable(Ut,getDbTagData(),CommMetaData(6));
    res+= comm.receiveMovable(U,getDbTagData(),CommMetaData(7));
    return res;
  }


void XC::TRBDFBase::Print(std::ostream &s, int flag)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if (theModel != 0)
      {
        double currentTime = getCurrentModelTime();
        s << "\t " << getClassName() << " - currentTime: " << currentTime;
      }
    else
      s << "\t " << getClassName() << " - no associated AnalysisModel\n";
  }

