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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/10/19 21:53:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/LoadControl.cpp,v $
                                                                        
                                                                        
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for XC::LoadControl.
// LoadControl is an algorithmic class for perfroming a static analysis
// using a load control integration scheme.
//
// What: "@(#) LoadControl.h, revA"



#include <solution/analysis/integrator/static/LoadControl.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param dLambda: load factor used in the first step.
//! @param numIncr: number of load increments.
//! @param min: minimum value for the load factor at each step.
//! @param max: maximum value for the load factor at each step.
XC::LoadControl::LoadControl(AnalysisAggregation *owr,double dLambda, int numIncr, double min, double max)
  :BaseControl(owr,INTEGRATOR_TAGS_LoadControl,numIncr), deltaLambda(dLambda), dLambdaMin(min), dLambdaMax(max) {}

//! @brief Perform a new analysis step.
//!
//! The object obtains the current value of \f$\lambda\f$ from the AnalysisModel
//! object. It increments this by \f$\delta \lambda_n \f$.
//!
//! \[ 
//! \delta \lambda_n = max \left( \delta \lambda_{min}, min \left(
//! \frac{Jd}{J_{n-1}} \delta \lambda_{n-1}, \delta \lambda_{max} \right)
//! \right) \]
//! 
//! \noindent It will then invoke
//! {\em applyLoadDomain(0.0, \f$\lambda\f$)} on the AnalysisModel
//! object. Returns \f$0\f$ if successful. A warning message is printed and a
//! \f$-1\f$ is returned if no AnalysisModel is associated with the object.
int XC::LoadControl::newStep(void)
  {
    int retval= 0;
    AnalysisModel *theModel = this->getAnalysisModelPtr();    
    if(theModel)
      {
        // determine delta lambda for this step based on dLambda and #iter of last step
        deltaLambda*=factor();

        if(deltaLambda < dLambdaMin)
          deltaLambda = dLambdaMin;
        else if (deltaLambda > dLambdaMax)
          deltaLambda = dLambdaMax;

        const double currentLambda= getCurrentModelTime() + deltaLambda;
        applyLoadModel(currentLambda);
        numIncrLastStep = 0;
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no associated AnalysisModel.\n";
	retval= -1;
      }
    return retval;
  }
    
//! @brief Update the model state.
//!
//! Invoked this causes the object to first increment the DOF\_Group
//! displacements with \f$\Delta U\f$, by invoking
//! {\em incrDisp(\f$\Delta U)\f$} on the AnalysisModel, and then to update
//! the domain, by invoking {\em updateDomain()} on the AnalysisModel. Returns
//! \f$0\f$ if successful, a warning message and a \f$-1\f$ is returned if no
//! AnalysisModel is associated with the object.
int XC::LoadControl::update(const Vector &deltaU)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theSOE = this->getLinearSOEPtr();
    if((!myModel) || (!theSOE))
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no AnalysisModel or LinearSOE has been set\n";
	return -1;
      }

    myModel->incrDisp(deltaU);    
    if(updateModel() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; update failed for the new dU\n";
        return -1;
      }

    // Set deltaU for the convergence test
    theSOE->setX(deltaU);
    numIncrLastStep++;
    return 0;
  }

//! @brief Sets deltaLambda and updates dLamdaMin and dLambdaMax values.
void XC::LoadControl::setDeltaLambda(const double &newValue)
  {
    // we set the #incr at last step = #incr so get newValue incr
    numIncrLastStep = specNumIncrStep;
    deltaLambda = newValue;
    if(deltaLambda<dLambdaMin) //Update extrema.
      dLambdaMin= deltaLambda;
    if(deltaLambda>dLambdaMax)
      dLambdaMax= deltaLambda;
  }

//! @brief Sets the value of the minimum increment factor.
void XC::LoadControl::setDeltaLambdaMin(const double &d)
  { dLambdaMin= d; }

//! @brief Sets the value of the maximum increment factor.
void XC::LoadControl::setDeltaLambdaMax(const double &d)
  { dLambdaMax= d; }

//! @brief Send object members through the channel being passed as parameter.
int XC::LoadControl::sendData(CommParameters &cp)
  {
    int res= BaseControl::sendData(cp);
    res+= cp.sendDoubles(deltaLambda,dLambdaMin, dLambdaMax,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::LoadControl::recvData(const CommParameters &cp)
  {
    int res= BaseControl::recvData(cp);
    res+= cp.receiveDoubles(deltaLambda,dLambdaMin, dLambdaMax,getDbTagData(),CommMetaData(2));
    return res;
  }

int XC::LoadControl::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; failed to send data\n";
    return res;
  }

int XC::LoadControl::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
                    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief The object sends to \f$os\f$ its type, the current value
//! of \f$\lambda\f$, and \f$\delta \lambda\f$. 
void XC::LoadControl::Print(std::ostream &os, int flag)
  {
    BaseControl::Print(os,flag);
    os << "  deltaLambda: " << deltaLambda << std::endl;
  }

