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
//ProtoArcLength.cpp


#include <solution/analysis/integrator/static/ProtoArcLength.h>
#include "solution/analysis/model/AnalysisModel.h"
#include "solution/system_of_eqn/linearSOE/LinearSOE.h"

#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
XC::ProtoArcLength::ProtoArcLength(AnalysisAggregation *owr,int classTag,double arcLength)
  :StaticIntegrator(owr,classTag), arcLength2(arcLength*arcLength), signLastDeltaLambdaStep(1) {}

int XC::ProtoArcLength::newStep(void)
  {
    // get pointers to AnalysisModel and LinearSOE
    LinearSOE *theLinSOE= this->getLinearSOEPtr();
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl || !theLinSOE)
      {
        std::cerr << "WARNING ArcLength::newStep() ";
        std::cerr << "No AnalysisModel or LinearSOE has been set\n";
        return -1;
      }

    // get the current load factor
    vectores.setCurrentLambda(getCurrentModelTime());

    if(vectores.getDeltaLambdaStep() < 0)
      signLastDeltaLambdaStep= -1;
    else
      signLastDeltaLambdaStep= +1;

    // determine dUhat
    this->formTangent();
    vectores.determineUhat(*theLinSOE);

    // determine delta lambda(1) == dlambda
    const double dLambda = getDLambdaNewStep();

    vectores.newStep(dLambda,vectores.getDeltaUhat());

    // update model with delta lambda and delta U
    mdl->incrDisp(vectores.getDeltaU());
    applyLoadModel(vectores.getCurrentLambda());
    if(updateModel() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; update failed for the new dU\n";
        return -1;
      }

    return 0;
  }

//! @brief Actualiza el estado del sistema.
int XC::ProtoArcLength::update(const Vector &dU)
  {
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl || !theLinSOE)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no AnalysisModel or LinearSOE has been set\n";
	return -1;
      }

    vectores.solve(dU,*theLinSOE);

    const double dLambda= getDLambdaUpdate();

    vectores.update(dLambda);

    // update the model
    mdl->incrDisp(vectores.getDeltaU());    
    applyLoadModel(vectores.getCurrentLambda());    
    if(updateModel() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; update failed for the new dU\n";
      }
    
    // set the X soln in linearSOE to be deltaU for convergence Test
    theLinSOE->setX(vectores.getDeltaU());

    return 0;
  }

//! @brief Respuesta a un cambio en el domain.
int XC::ProtoArcLength::domainChanged(void)
  {
    // we first create the Vectors needed
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl || !theLinSOE)
      {
	std::cerr << "WARNING ArcLengthBase::domainChanged() ";
	std::cerr << "No AnalysisModel or LinearSOE has been set\n";
	return -1;
      }    
    const size_t sz= mdl->getNumEqn(); // ask model in case N+1 space

    vectores.domainChanged(sz,*this,*theLinSOE);
    
    return 0;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::ProtoArcLength::sendData(CommParameters &cp)
  {
    int res= StaticIntegrator::sendData(cp);
    res+= cp.sendDouble(arcLength2,getDbTagData(),CommMetaData(1));
    res+= cp.sendMovable(vectores,getDbTagData(),CommMetaData(2));
    res+= cp.sendInt(signLastDeltaLambdaStep,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::ProtoArcLength::recvData(const CommParameters &cp)
  {
    int res= StaticIntegrator::recvData(cp);
    res+= cp.receiveDouble(arcLength2,getDbTagData(),CommMetaData(1));
    res+= cp.receiveMovable(vectores,getDbTagData(),CommMetaData(2));
    res+= cp.receiveInt(signLastDeltaLambdaStep,getDbTagData(),CommMetaData(7));
    return res;
  }

int XC::ProtoArcLength::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data\n";
    return res;
  }


int XC::ProtoArcLength::recvSelf(const CommParameters &cp)
  {
    inicComm(8);
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
                     << "; failed to receive ids.\n";
      }
    return res;
  }

