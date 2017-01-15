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
XC::LoadControl::LoadControl(SoluMethod *owr,double dLambda, int numIncr, double min, double max)
  :BaseControl(owr,INTEGRATOR_TAGS_LoadControl,numIncr), deltaLambda(dLambda), dLambdaMin(min), dLambdaMax(max) {}

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
	std::cerr << "LoadControl::newStep() - no associated AnalysisModel\n";
	retval= -1;
      }
    return retval;
  }
    
int XC::LoadControl::update(const Vector &deltaU)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theSOE = this->getLinearSOEPtr();
    if((!myModel) || (!theSOE))
      {
	std::cerr << "WARNING XC::LoadControl::update() ";
	std::cerr << "No XC::AnalysisModel or LinearSOE has been set\n";
	return -1;
      }

    myModel->incrDisp(deltaU);    
    if(updateModel() < 0)
      {
        std::cerr << "LoadControl::update - fallo al actualizar el modelo"
                  << " para el nuevo dU\n";
        return -1;
      }

    // Set deltaU for the convergence test
    theSOE->setX(deltaU);
    numIncrLastStep++;
    return 0;
  }

//! @brief Assings deltaLambda and updates dLamdaMin and dLambdaMax values.
void XC::LoadControl::setDeltaLambda(const double &newValue)
  {
    // we set the #incr at last step = #incr so get newValue incr
    numIncrLastStep = specNumIncrStep;
    deltaLambda = newValue;
    if(deltaLambda<dLambdaMin) //Actualizamos valores extremos.
      dLambdaMin= deltaLambda;
    if(deltaLambda>dLambdaMax)
      dLambdaMax= deltaLambda;
  }

void XC::LoadControl::setDeltaLambdaMin(const double &d)
  { dLambdaMin= d; }

void XC::LoadControl::setDeltaLambdaMax(const double &d)
  { dLambdaMax= d; }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::LoadControl::sendData(CommParameters &cp)
  {
    int res= BaseControl::sendData(cp);
    res+= cp.sendDoubles(deltaLambda,dLambdaMin, dLambdaMax,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
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
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::LoadControl::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
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


void XC::LoadControl::Print(std::ostream &s, int flag)
  {
    BaseControl::Print(s,flag);
    s << "  deltaLambda: " << deltaLambda << std::endl;
  }

