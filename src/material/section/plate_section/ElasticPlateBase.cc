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

//! @brief Constuctor.
XC::ElasticPlateBase::ElasticPlateBase(int tag,int classTag)
  : XC::PlateBase(tag, classTag), E(0.0), nu(0.0) {}

//null constructor
XC::ElasticPlateBase::ElasticPlateBase(int classTag)
  :XC::PlateBase( 0, classTag),
   E(0.0), nu(0.0) { }

//full constructor
XC::ElasticPlateBase::ElasticPlateBase(int tag, int classTag,
                                           double young,
                                           double poisson,
                                           double thickness)
  :XC::PlateBase(tag,classTag,h), E(young), nu(poisson) {}

//swap history variables
int XC::ElasticPlateBase::commitState(void) 
  { return 0 ; }

//revert to last saved state
int XC::ElasticPlateBase::revertToLastCommit(void)
  { return 0 ; }

//revert to start
int XC::ElasticPlateBase::revertToStart(void)
  { return 0; }

//! @brief Envía los datos through the channel being passed as parameter.
int XC::ElasticPlateBase::sendData(CommParameters &cp)
  {
    int res= PlateBase::sendData(cp);
    res+= cp.sendDoubles(E,nu,h,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Recibe los datos through the channel being passed as parameter.
int XC::ElasticPlateBase::recvData(const CommParameters &cp)
  {
    int res= PlateBase::recvData(cp);
    res+= cp.receiveDoubles(E,nu,h,getDbTagData(),CommMetaData(5));
    return res;
  }
