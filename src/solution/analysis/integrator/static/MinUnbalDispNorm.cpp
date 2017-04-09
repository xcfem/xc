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
// $Date: 2005/10/19 21:53:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/MinUnbalDispNorm.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/MinUnbalDispNorm.C
// 
// Written: fmk 
// Created: 07/99
// Revision: A
//
// What: "@(#) MinUnbalDispNorm.C, revA"


#include <solution/analysis/integrator/static/MinUnbalDispNorm.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include <cmath>

//! @brief Constructor.
XC::MinUnbalDispNorm::MinUnbalDispNorm(SoluMethod *owr,double lambda1, int specNumIter,double min, double max, int signFirstStep)
  :DispBase(owr,INTEGRATOR_TAGS_MinUnbalDispNorm,specNumIter),
 dLambda1LastStep(lambda1), signLastDeltaLambdaStep(1),
 dLambda1min(min), dLambda1max(max), signLastDeterminant(1), signFirstStepMethod(signFirstStep)
  {}

//! @brief Returns the valor de dLambda para el método newStep
double XC::MinUnbalDispNorm::getDLambdaNewStep(void) const
  {
    // determine delta lambda(1) == dlambda
    double retval = dLambda1LastStep*factor();

    // check aaint min and max values specified in constructor
    if(retval < dLambda1min)
      retval= dLambda1min;
    else if(retval > dLambda1max)
      retval= dLambda1max;

    dLambda1LastStep= retval;


    if(signFirstStepMethod == SIGN_LAST_STEP)
      {
        if(vectores.getDeltaLambdaStep() < 0)
          signLastDeltaLambdaStep = -1;
        else
	  signLastDeltaLambdaStep = +1;
        retval*= signLastDeltaLambdaStep; // base sign of load change
                                        // on what was happening last step
      }
    else
      {
        //XXX Lo de eliminar la «constancia» no queda muy bien.
        LinearSOE *theLinSOE= const_cast<LinearSOE *>(getLinearSOEPtr());
        double det= theLinSOE->getDeterminant();
        int signDeterminant = 1;
        if(det < 0)
	  signDeterminant = -1;
        retval*= signDeterminant * signLastDeterminant;
        signLastDeterminant = signDeterminant;
      }
    return retval;
  }

int XC::MinUnbalDispNorm::newStep(void)
  {
    // get pointers to XC::AnalysisModel and XC::LinearSOE
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(theModel == 0 || theLinSOE == 0)
      {
	std::cerr << "WARNING XC::MinUnbalDispNorm::newStep() ";
	std::cerr << "No XC::AnalysisModel or XC::LinearSOE has been set\n";
	return -1;
      }

    // get the current load factor
    vectores.setCurrentLambda(getCurrentModelTime());


    // determine dUhat
    this->formTangent();
    vectores.determineUhat(*theLinSOE);

    const double dLambda= getDLambdaNewStep();

    /*
    double work = (*phat)^(dUhat);
    int signCurrentWork = 1;
    if (work < 0) signCurrentWork = -1;

    if (signCurrentWork != signLastDeltaStep)
    */

    vectores.newStep(dLambda,vectores.getDeltaUhat());
    numIncrLastStep= 0;

    // update model with delta lambda and delta U
    theModel->incrDisp(vectores.getDeltaU());    
    applyLoadModel(vectores.getCurrentLambda());    
    if(updateModel() < 0)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; update failed for the new dU\n";
        return -1;
      }
    return 0;
  }

//! @brief Returns the valor de dLambda para el método update.
double XC::MinUnbalDispNorm::getDLambdaUpdate(void) const
  {
    const Vector &dUhat= vectores.getDeltaUhat();
    const Vector &dUbar= vectores.getDeltaUbar();
    // determine delta lambda(i)
    double a = dUhat^dUbar;
    double b = dUhat^dUhat;
    if(b == 0)
      {
        std::cerr << "XC::MinUnbalDispNorm::getDLambdaUpdate() - zero denominator\n";
        return -1;
      }
    return -a/b;
  }

int XC::MinUnbalDispNorm::update(const Vector &dU)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if (theModel == 0 || theLinSOE == 0) {
	std::cerr << "WARNING XC::MinUnbalDispNorm::update() ";
	std::cerr << "No AnalysisModel or LinearSOE has been set\n";
	return -1;
    }

    vectores.setDeltaUbar(dU); // have to do this as the SOE is gonna change

    vectores.determineUhat(*theLinSOE);

    const double dLambda= getDLambdaUpdate();

    
    vectores.update(dLambda);

    // update the model
    theModel->incrDisp(vectores.getDeltaU());    
    applyLoadModel(vectores.getCurrentLambda());    

    if(updateModel() < 0)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
	          << "; update failed for the new dU\n";
        return -1;
      }

    // set the X soln in linearSOE to be deltaU for convergence Test
    theLinSOE->setX(vectores.getDeltaU());

    numIncrLastStep++;
    return 0;
  }



int XC::MinUnbalDispNorm::domainChanged(void)
  {
    // we first create the Vectors needed
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(theModel == 0 || theLinSOE == 0)
      {
	std::cerr << "WARNING XC::MinUnbalDispNorm::update() ";
	std::cerr << "No XC::AnalysisModel or XC::LinearSOE has been set\n";
	return -1;
      }    
    const size_t sz= theModel->getNumEqn(); // ask model in case N+1 space

    vectores.domainChanged(sz,*this,*theLinSOE);

    return 0;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::MinUnbalDispNorm::sendData(CommParameters &cp)
  {
    int res= DispBase::sendData(cp);
    res+= cp.sendDoubles(dLambda1LastStep,dLambda1min,dLambda1max,signLastDeterminant,getDbTagData(),CommMetaData(17));
    res+= cp.sendInts(signLastDeltaLambdaStep,signFirstStepMethod,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::MinUnbalDispNorm::recvData(const CommParameters &cp)
  {
    int res= DispBase::recvData(cp);
    res+= cp.receiveDoubles(dLambda1LastStep,dLambda1min,dLambda1max,signLastDeterminant,getDbTagData(),CommMetaData(17));
    res+= cp.receiveInts(signLastDeltaLambdaStep,signFirstStepMethod,getDbTagData(),CommMetaData(18));
    return res;
  }

int XC::MinUnbalDispNorm::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::MinUnbalDispNorm::recvSelf(const CommParameters &cp)
  {
    inicComm(19);
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
