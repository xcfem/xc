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
//IntegratorVectors.cpp


#include "IntegratorVectors.h"
#include "solution/analysis/integrator/IncrementalIntegrator.h"

#include "solution/system_of_eqn/linearSOE/LinearSOE.h"
#include "utility/actor/actor/CommMetaData.h"

//! @brief Constructor.
XC::IntegratorVectors::IntegratorVectors(void)
  :MovableObject(0), deltaLambdaStep(0.0), currentLambda(0.0) {}


//! @brief Changes vectors size.
void XC::IntegratorVectors::resize(const int &size)
  {
    if(deltaUhat.Size() != size)
      deltaUhat.resize(size);

    if(deltaUbar.Size() != size)
      deltaUbar.resize(size);

    if(deltaU.Size() != size)
      deltaU.resize(size);

    if(deltaUstep.Size() != size)
      deltaUstep.resize(size);

    if(phat.Size() != size)
      phat.resize(size);
  }

//! @brief Performs a new step.
void XC::IntegratorVectors::newStep(const double &dLambda,const Vector &dUhat)
  {
    deltaLambdaStep= dLambda;
    currentLambda+= dLambda;

    // determine delta U(1) == dU
    deltaU= dUhat;
    deltaU*= dLambda;
    deltaUstep= deltaU;
  }

//! @brief Computes the solution for the new load vector using parallel processing.
void XC::IntegratorVectors::distribDetermineUhat(const int &pID,LinearSOE &theLinSOE)
  {
    if(pID == 0)
      theLinSOE.setB(phat);
    else
      theLinSOE.zeroB();

    theLinSOE.solve();
    deltaUhat= theLinSOE.getX();
  }

//! @brief Computes the solution for the new load vector.
void XC::IntegratorVectors::determineUhat(LinearSOE &theLinSOE)
  {
    // determine dUhat    
    theLinSOE.setB(phat);
    if(theLinSOE.solve() < 0)
      std::cerr << "IntegratorVectors::determineUhat - failed in solver\n";
    deltaUhat= theLinSOE.getX();
  }

//! @brief Computes the solution for the new load vector.
void XC::IntegratorVectors::solve(const Vector &dU,LinearSOE &theLinSOE)
  {
    deltaUbar= dU; // have to do this as the SOE is gonna change
    determineUhat(theLinSOE);
  }

//! @brief Updates vectors.
void XC::IntegratorVectors::update(const double &dLambda)
  {
    // determine delta U(i)
    deltaU= deltaUbar;    
    deltaU.addVector(1.0, deltaUhat,dLambda);
    
    // update dU and dlambda
    deltaUstep+= deltaU;
    deltaLambdaStep+= dLambda;
    currentLambda+= dLambda;
  }

//! @brief Respuesta a un cambio en el domain.
void XC::IntegratorVectors::domainChanged(const size_t &sz,IncrementalIntegrator &I,LinearSOE &theLinSOE)
  {
    resize(sz);

    // now we have to determine phat
    // do this by incrementing lambda by 1, applying load
    // and getting phat from unbalance.
    currentLambda= I.getCurrentModelTime()+1.0;
    I.applyLoadModel(currentLambda);
    I.formUnbalance(); // NOTE: this assumes unbalance at last was 0
    phat= theLinSOE.getB();
    currentLambda-= 1.0;
    I.setCurrentModelTime(currentLambda);

    // check there is a reference load
    bool haveLoad= false;
    for(size_t i=0; i<sz; i++)
      if(phat(i)!=0.0)
        {
          haveLoad= true;
          break;
        }
    if(!haveLoad)
      {
        std::cerr << "WARNING IntegratorVectors::domainChanged() - zero reference load";
      }
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::IntegratorVectors::sendData(Communicator &comm)
  {
    int res= comm.sendDoubles(deltaLambdaStep,currentLambda,getDbTagData(),CommMetaData(1));
    res+= comm.sendVector(deltaUhat,getDbTagData(),CommMetaData(2));
    res+= comm.sendVector(deltaUbar,getDbTagData(),CommMetaData(3));
    res+= comm.sendVector(deltaU,getDbTagData(),CommMetaData(4));
    res+= comm.sendVector(deltaUstep,getDbTagData(),CommMetaData(5));
    res+= comm.sendVector(phat,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::IntegratorVectors::recvData(const Communicator &comm)
  {
    int res= comm.receiveDoubles(deltaLambdaStep,currentLambda,getDbTagData(),CommMetaData(1));
    res+= comm.receiveVector(deltaUhat,getDbTagData(),CommMetaData(2));
    res+= comm.receiveVector(deltaUbar,getDbTagData(),CommMetaData(3));
    res+= comm.receiveVector(deltaU,getDbTagData(),CommMetaData(4));
    res+= comm.receiveVector(deltaUstep,getDbTagData(),CommMetaData(5));
    res+= comm.receiveVector(phat,getDbTagData(),CommMetaData(6));
    return res;
  }

int XC::IntegratorVectors::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "IntegratorVectors::sendSelf() - failed to send data\n";
    return res;
  }


int XC::IntegratorVectors::recvSelf(const Communicator &comm)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "IntegratorVectors::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << "IntegratorVectors::recvSelf - failed to receive data.\n";
      }
    return res;
  }


