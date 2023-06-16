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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/TRBDF2.cpp,v $

// Written : fmk
// Created : 02/09
//
// Description: This file contains the implementation of the TRBDF2 class.
// ref: K.J.Bathe, "Conserving Energy and Momentum in Nonlinear Dynamics: A Simple
//      Implicit Time Integration Scheme", Computers and Structures 85(2007),437-445
//
// NOTE: In the implementation we use deltaT as opposed to deltaT/2 in the paper.
//
// What: "@(#) TRBDF2.C, revA"

#include "TRBDF2.h"
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>


//! @ Constructor.
XC::TRBDF2::TRBDF2(SolutionStrategy *owr)
  : TRBDFBase(owr, INTEGRATOR_TAGS_TRBDF2) {}

//! @brief Virtual constructor.
XC::Integrator *XC::TRBDF2::getCopy(void) const
  { return new TRBDF2(*this); }

int XC::TRBDF2::newStep(double deltaT)
  {
    // check the vectors have been created
    if(U.size() == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; domainChange() failed or hasn't been called.\n";
        return -3;
      }

    // mark step as Trapezoidal (=0) or Backward Euler (=1)

    if(deltaT != dt || step == 1)
      { step = 0; }
    else
      { step = 1; }

    // get a pointer to the AnalysisModel
    AnalysisModel *theModel = this->getAnalysisModelPtr();

    // set response at t to be that at t+deltaT of previous step
    dt = deltaT;

    Utm1= Ut;
    
    Ut= U;        

    // set the constants
    if(step == 0)
      { // trapezoidal
        c1 = 1.0;
        c2 = 2.0/deltaT;
        c3 = 4.0/(deltaT*deltaT);

        U.getDot()*= -1.0;

        const double a3= -4.0/deltaT;
        const double a4= -1;
        U.getDotDot().addVector(a4, Ut.getDot(), a3);

      }
    else
      {  // backward euler
	c1 = 1.0;
	c2 = 1.5/deltaT;
	c3 = 2.25/(deltaT*deltaT);

	U.setDot(Utm1.get());
	U.getDot().addVector(0.5/deltaT, Ut.get(), -1/(2.0*deltaT));

	U.setDotDot(Utm1.getDot());
	U.getDotDot().addVector(0.5/deltaT, Ut.getDot(), -4.0/(2.0*deltaT));    
	U.getDotDot().addVector(1.0, U.getDot(), 3.0/(2.0*deltaT));    
      }
    
    // set the trial response quantities
    theModel->setVel(U.getDot());
    theModel->setAccel(U.getDotDot());    

    // increment the time to t+deltaT and apply the load
    const double time = getCurrentModelTime() + deltaT;
    if (updateModel(time, deltaT) < 0)
      {
        std::cerr << "TRBDF2::newStep() - failed to update the domain\n";
        return -4;
      }

    return 0;
  }


int XC::TRBDF2::update(const Vector &deltaU)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if (theModel == 0)  {
        std::cerr << "WARNING TRBDF2::update() - no AnalysisModel set\n";
        return -1;
    }	
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.size() == 0)
      {
        std::cerr << "WARNING TRBDF2::update() - domainChange() failed or not called\n";
        return -2;
      }	
    
    // check deltaU is of correct size
    if(deltaU.Size() != U.get().Size())
      {
        std::cerr << "WARNING TRBDF2::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << deltaU.Size() << std::endl;
        return -3;
      }
    
    //  determine the response at t+deltaT
    if(step == 0)
      {
        U.get()+= deltaU;
        U.getDot().addVector(1.0, deltaU, c2);
        U.getDotDot().addVector(1.0, deltaU, c3);
      }
    else
      {
        U.get()+= deltaU;
        U.getDot().addVector(1.0, deltaU, c2);
        U.getDotDot().addVector(1.0, deltaU, c3);
      }

    // update the response at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());
    if(updateModel() < 0)
      {
        std::cerr << "TRBDF2::update() - failed to update the domain\n";
        return -4;
      }
    
    return 0;
  }    


int XC::TRBDF2::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << " - failed to send data\n";
    return res;
  }

int XC::TRBDF2::recvSelf(const Communicator &comm)
  {
    inicComm(8);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr <<  getClassName() << "::" << __FUNCTION__
	        << "- failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr <<  getClassName() << "::" << __FUNCTION__
		    << " - failed to receive data.\n";
      }
    return res;
  }
