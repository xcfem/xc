//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::LoadPath::sendData(CommParameters &cp)
  {
    int res= StaticIntegrator::sendData(cp);
    res+= cp.sendInt(currentStep,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(loadPath,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
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


