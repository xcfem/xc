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
// $Date: 2003/02/14 23:00:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/LoadPath.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/LoadPath.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for XC::LoadPath.
// LoadPath is an algorithmic class for perfroming a static analysis
// using a load control integration scheme.
//
// What: "@(#) LoadPath.h, revA"


#include <solution/analysis/integrator/static/LoadPath.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/ID.h>
#include <cstdlib>
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
XC::LoadPath::LoadPath(SoluMethod *owr,const Vector &theLoadPath)
  :StaticIntegrator(owr,INTEGRATOR_TAGS_LoadPath), loadPath(theLoadPath), currentStep(0) {}

//! @brief Constructor.
XC::LoadPath::LoadPath(SoluMethod *owr)
  :StaticIntegrator(owr,INTEGRATOR_TAGS_LoadPath), currentStep(0) {}

int XC::LoadPath::newStep(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();    
    if(theModel == 0)
      {
	std::cerr << "XC::LoadPath::newStep() - no associated XC::AnalysisModel\n";
	return -1;
      }
    

    double modelLambda = getCurrentModelTime();

    double currentLambda;
    if(currentStep < loadPath.Size())
      {
      
        if(currentStep > 0)
          {
	    if(modelLambda == loadPath(currentStep-1))
	      currentLambda= loadPath(currentStep);  
            else
	      currentLambda= loadPath(currentStep-1);  
          }
        else
	  currentLambda= loadPath(currentStep);  
      }      
    else
      {
	currentLambda = 0.0;
	std::cerr << "XC::LoadPath::newStep() - reached end of specified load path";
	std::cerr << " - setting lambda = 0.0 \n";
      }
    currentStep++;
    applyLoadModel(currentLambda);
    return 0;
  }
    
int XC::LoadPath::update(const XC::Vector &deltaU)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    if(myModel == 0)
      {
	std::cerr << "WARNING XC::LoadPath::update() ";
	std::cerr << "No AnalysisModel has been set\n";
	return -1;
      }

    myModel->incrDisp(deltaU);    
    updateModel();
    return 0;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::LoadPath::sendData(CommParameters &cp)
  {
    int res= StaticIntegrator::sendData(cp);
    res+= cp.sendInt(currentStep,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(loadPath,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::LoadPath::recvData(const CommParameters &cp)
  {
    int res= StaticIntegrator::recvData(cp);
    res+= cp.receiveInt(currentStep,getDbTagData(),CommMetaData(1));
    res+= cp.receiveVector(loadPath,getDbTagData(),CommMetaData(2));
    return res;
  }

int XC::LoadPath::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(5);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::LoadPath::recvSelf(const CommParameters &cp)
  {
    inicComm(5);
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


