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
//ElasticPlateBase.cc

#include <material/section/plate_section/ElasticPlateBase.h>

#include "utility/actor/actor/MovableVector.h"


//parameters
const double XC::ElasticPlateBase::five6 = 5.0/6.0 ; //shear correction

//! @brief Constructor.
XC::ElasticPlateBase::ElasticPlateBase(int tag,int classTag)
  : XC::PlateBase(tag, classTag), E(0.0), nu(0.0) {}

//! @brief Null constructor
XC::ElasticPlateBase::ElasticPlateBase(int classTag)
  :XC::PlateBase( 0, classTag),
   E(0.0), nu(0.0) { }

//! @brief Constructor.
XC::ElasticPlateBase::ElasticPlateBase(int tag, int classTag,
                                           double young,
                                           double poisson,
				           double thickness,
				           double rho)
  :XC::PlateBase(tag,classTag,thickness,rho), E(young), nu(poisson) {}

//! @brief swap history variables
int XC::ElasticPlateBase::commitState(void) 
  { return 0 ; }

//! @brief revert to last saved state
int XC::ElasticPlateBase::revertToLastCommit(void)
  { return 0 ; }

//! @brief revert to start
int XC::ElasticPlateBase::revertToStart(void)
  { return 0; }

//! @brief Send data through the communicator argument.
int XC::ElasticPlateBase::sendData(Communicator &comm)
  {
    int res= PlateBase::sendData(comm);
    res+= comm.sendDoubles(E,nu,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::ElasticPlateBase::recvData(const Communicator &comm)
  {
    int res= PlateBase::recvData(comm);
    res+= comm.receiveDoubles(E,nu,getDbTagData(),CommMetaData(5));
    return res;
  }
