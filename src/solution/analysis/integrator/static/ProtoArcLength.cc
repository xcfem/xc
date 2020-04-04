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
//!
//! @param owr: set of objects used to perform the analysis.
//! @param classTag: class identifier.
//! @param arcLength: value for the arc length.
XC::ProtoArcLength::ProtoArcLength(AnalysisAggregation *owr,int classTag,double arcLength)
  :StaticIntegrator(owr,classTag), arcLength2(arcLength*arcLength), signLastDeltaLambdaStep(1) {}

//! Performs the first iteration, that is it solves for 
//! \f$\lambda_n^{(1)}\f$ and \f$\Delta  U_n^{(1)}\f$ and updates the model with
//! \f$\Delta  U_n^{(1)}\f$ and increments the load factor by
//! \f$\lambda_n^{(1)}\f$. To do this it must set the rhs of the LinearSOE to
//! \f$ P\f$, invoke formTangent() on itself and solve the LinearSOE to
//! get \f$\Delta  Uh_n^{(1)}\f$.
int XC::ProtoArcLength::newStep(void)
  {
    // get pointers to AnalysisModel and LinearSOE
    LinearSOE *theLinSOE= this->getLinearSOEPtr();
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl || !theLinSOE)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - no AnalysisModel or LinearSOE has been set\n";
        return -1;
      }

    // get the current load factor
    vectors.setCurrentLambda(getCurrentModelTime());

    if(vectors.getDeltaLambdaStep() < 0)
      signLastDeltaLambdaStep= -1;
    else
      signLastDeltaLambdaStep= +1;

    // determine dUhat
    this->formTangent();
    vectors.determineUhat(*theLinSOE);

    // determine delta lambda(1) == dlambda
    const double dLambda = getDLambdaNewStep();

    vectors.newStep(dLambda,vectors.getDeltaUhat());

    // update model with delta lambda and delta U
    mdl->incrDisp(vectors.getDeltaU());
    applyLoadModel(vectors.getCurrentLambda());
    if(updateModel() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; update failed for the new dU\n";
        return -1;
      }

    return 0;
  }

//! @brief Updates the model.
//!
//! Note the argument \f$\Delta U\f$ should be equal to \f$\Delta Ub_n^{(i)}\f$.
//! The object then determines \f$\Delta Uh_n^{(i)}\f$ by setting the rhs of
//! the linear system of equations to be \f$\P\f$ and then solving the
//! linearSOE. It then solves for
//! \f$\Delta \lambda_n^{(i)}\f$ and \f$\Delta  U_n^{(i)}\f$ and updates the
//! model with \f$\Delta  U_n^{(i)}\f$ and increments the load factor by
//! \f$\Delta \lambda_n^{(i)}\f$. Sets the vector \f$x\f$ in the LinearSOE
//! object to be equal to \f$\Delta  U_n^{(i)}\f$ before returning (this is
//! for the convergence test stuff.
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

    vectors.solve(dU,*theLinSOE);

    const double dLambda= getDLambdaUpdate();

    vectors.update(dLambda);

    // update the model
    mdl->incrDisp(vectors.getDeltaU());    
    applyLoadModel(vectors.getCurrentLambda());    
    if(updateModel() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; update failed for the new dU\n";
      }
    
    // set the X soln in linearSOE to be deltaU for convergence Test
    theLinSOE->setX(vectors.getDeltaU());

    return 0;
  }

//! @brief Response to a change in the domain.
//!
//! The object creates the Vector objects it needs. Vectors are created to
//! stor \f$ P\f$, \f$\Delta  Ub_n^{(i)}\f$, \f$\Delta  Uh_n^{(i)}\f$, \f$\Delta
//! Ub_n^{(i)}\f$, \f$dU^{(i)}\f$. To form \f$ P\f$, the current load factor is
//! obtained from the model, it is incremented by \f$1.0\f$, {\em
//! formUnbalance()} is invoked on the object, and the \f$b\f$ vector is
//! obtained from the linearSOE. This is \f$ P\f$, the load factor on the
//! model is then decremented by \f$1.0\f$.
int XC::ProtoArcLength::domainChanged(void)
  {
    // we first create the Vectors needed
    LinearSOE *theLinSOE= this->getLinearSOEPtr();    
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl || !theLinSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "WARNING - no AnalysisModel or LinearSOE has been set.\n";
	return -1;
      }    
    const size_t sz= mdl->getNumEqn(); // ask model in case N+1 space

    vectors.domainChanged(sz,*this,*theLinSOE);
    
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::ProtoArcLength::sendData(Communicator &comm)
  {
    int res= StaticIntegrator::sendData(comm);
    res+= comm.sendDouble(arcLength2,getDbTagData(),CommMetaData(1));
    res+= comm.sendMovable(vectors,getDbTagData(),CommMetaData(2));
    res+= comm.sendInt(signLastDeltaLambdaStep,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ProtoArcLength::recvData(const Communicator &comm)
  {
    int res= StaticIntegrator::recvData(comm);
    res+= comm.receiveDouble(arcLength2,getDbTagData(),CommMetaData(1));
    res+= comm.receiveMovable(vectors,getDbTagData(),CommMetaData(2));
    res+= comm.receiveInt(signLastDeltaLambdaStep,getDbTagData(),CommMetaData(7));
    return res;
  }

int XC::ProtoArcLength::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data\n";
    return res;
  }


int XC::ProtoArcLength::recvSelf(const Communicator &comm)
  {
    inicComm(8);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
           std::cerr << getClassName() << "::" << __FUNCTION__
                     << "; failed to receive ids.\n";
      }
    return res;
  }

