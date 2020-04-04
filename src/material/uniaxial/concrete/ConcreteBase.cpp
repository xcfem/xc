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
//ConcreteBase.cpp

#include <material/uniaxial/concrete/ConcreteBase.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include <domain/mesh/element/utils/Information.h>
#include <cmath>
#include <cfloat>


//! @brief Constructor.
XC::ConcreteBase::ConcreteBase(int tag, int classTag, double FPC, double EPSC0, double EPSCU)
  :RawConcrete(tag, classTag,FPC,EPSC0,EPSCU) {}

//! @brief Constructor
XC::ConcreteBase::ConcreteBase(int tag, int classTag)
  :RawConcrete(tag, classTag) {}

//! @brief Returns the material stress.
double XC::ConcreteBase::getStress(void) const
  { return trialState.getStress(); }

//! @brief Returns material strain.
double XC::ConcreteBase::getStrain(void) const
  { return trialState.getStrain(); }

//! @brief Returns the tangent to stress-strain diagram.
double XC::ConcreteBase::getTangent(void) const
  { return trialState.getTangent(); }

//! @brief Send object members through the communicator argument.
int XC::ConcreteBase::sendData(Communicator &comm)
  {
    int res= RawConcrete::sendData(comm);
    res+= comm.sendMovable(convergedState,getDbTagData(),CommMetaData(2));
    res+= comm.sendMovable(trialState,getDbTagData(),CommMetaData(3));
    res+= comm.sendMovable(convergedHistory,getDbTagData(),CommMetaData(4));
    res+= comm.sendMovable(trialHistory,getDbTagData(),CommMetaData(5));
    res+= comm.sendDoubles(fpc,epsc0,epscu,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ConcreteBase::recvData(const Communicator &comm)
  {
    int res= RawConcrete::recvData(comm);
    res+= comm.receiveMovable(convergedState,getDbTagData(),CommMetaData(2));
    res+= comm.receiveMovable(trialState,getDbTagData(),CommMetaData(3));
    res+= comm.receiveMovable(convergedHistory,getDbTagData(),CommMetaData(4));
    res+= comm.receiveMovable(trialHistory,getDbTagData(),CommMetaData(5));
    res+= comm.receiveDoubles(fpc,epsc0,epscu,getDbTagData(),CommMetaData(6));
    return res;
  }

