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
//PseudoTimeTracker.cc

#include "PseudoTimeTracker.h"
#include <cassert>
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"

XC::PseudoTimeTracker::PseudoTimeTracker(void)
  : MovableObject(0), currentTime(0.0), committedTime(0.0), dT(0.0), eigenvalueTimeSet(0.0) {}

const double &XC::PseudoTimeTracker::getCurrentTime(void) const
  { return currentTime; }

const double &XC::PseudoTimeTracker::getCommittedTime(void) const
  { return committedTime; }

const double &XC::PseudoTimeTracker::getDt(void) const
  { return dT; }

const double &XC::PseudoTimeTracker::getEigenValueTimeSet(void) const
  { return eigenvalueTimeSet; }

void XC::PseudoTimeTracker::Inic(const double &newTime)
  {
    currentTime= newTime;
    committedTime= newTime;
    dT= 0.0;
  }

void XC::PseudoTimeTracker::setCurrentTime(const double &newTime)
  {
    currentTime= newTime;
    dT= currentTime - committedTime;
  }

void XC::PseudoTimeTracker::setCommittedTime(const double &newTime)
  {
    committedTime= newTime;
    dT= currentTime - committedTime;
    assert(dT>=0.0);
  }

void XC::PseudoTimeTracker::setEigenvalueTime(void)
  { eigenvalueTimeSet= currentTime; }

void XC::PseudoTimeTracker::Zero(void)
  { Inic(0.0); }

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::PseudoTimeTracker::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Send data through the channel being passed as parameter.
int XC::PseudoTimeTracker::sendData(CommParameters &cp)
  {
    int res= cp.sendDoubles(currentTime,committedTime,dT,eigenvalueTimeSet,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::PseudoTimeTracker::recvData(const CommParameters &cp)
  {
    int res= cp.receiveDoubles(currentTime,committedTime,dT,eigenvalueTimeSet,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PseudoTimeTracker::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "PseudoTimeTracker::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::PseudoTimeTracker::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "PseudoTimeTracker::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << "PseudoTimeTracker::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Imprime el objeto.
void XC::PseudoTimeTracker::Print(std::ostream &s, int flag) const
  {

    s << "Pseudo time Information\n";
    s << "\tCurrent Time: " << currentTime;
    s << "\ntCommitted Time: " << committedTime;
  }

std::ostream &XC::operator<<(std::ostream &s,const PseudoTimeTracker &tt)
  {
    tt.Print(s);
    return s;
  }
