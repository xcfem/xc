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


#include "TrussStrainLoad.h"
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

#include "utility/actor/actor/MovableVector.h"

XC::TrussStrainLoad::TrussStrainLoad(int tag,const double &t1, const double &t2,const ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_TrussStrainLoad, theElementTags), e1(t1),  e2(t2) {}
XC::TrussStrainLoad::TrussStrainLoad(int tag,const double &temp,const ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_TrussStrainLoad, theElementTags), e1(temp),  e2(temp) {}

XC::TrussStrainLoad::TrussStrainLoad(int tag, const ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_TrussStrainLoad, theElementTags), e1(0.0), e2(0.0) {}

XC::TrussStrainLoad::TrussStrainLoad(int tag)
  :ElementBodyLoad(tag, LOAD_TAG_TrussStrainLoad), e1(0.0), e2(0.0) {}

XC::TrussStrainLoad::TrussStrainLoad(void)
  :ElementBodyLoad(LOAD_TAG_TrussStrainLoad), e1(0.0), e2(0.0) {}

std::vector<XC::Vector> XC::TrussStrainLoad::getStrains(void) const
  {
    std::vector<Vector> retval(2);
    Vector tmp(1);
    tmp[0]= e1;
    retval[0]= tmp;
    tmp[1]= e2;
    retval[1]= tmp;
    return retval;
  }

const XC::Vector &XC::TrussStrainLoad::getData(int &type, const double &loadFactor) const
  {
    type = getClassTag();
    static Vector data(2);
    data(0)= e1;
    data(1)= e2;
    return data;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::TrussStrainLoad::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }

//! @brief Send data through the communicator argument.
int XC::TrussStrainLoad::sendData(Communicator &comm)
  {
    int res= ElementBodyLoad::sendData(comm);
    res+= comm.sendDoubles(e1,e2,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::TrussStrainLoad::recvData(const Communicator &comm)
  {
    int res= ElementBodyLoad::recvData(comm);
    res+= comm.receiveDoubles(e1,e2,getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::TrussStrainLoad::sendSelf(Communicator &comm)
  {
    inicComm(4);
    int result= sendData(comm);
    const int dbTag= getDbTag();
    result+= comm.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; - failed to send extra data\n";
    return result;
  }

int XC::TrussStrainLoad::recvSelf(const Communicator &comm)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; data could not be received\n" ;
    else
      res+= recvData(comm);
    return res;
  }

void XC::TrussStrainLoad::Print(std::ostream &s, int flag) const
  {
    s << "TrussStrainLoad - reference load : " << e1
      << " strain at node 1 : " << e1 << " strain at node 1\n";
    s <<  e2 << " strain at node 2\n";
    ElementBodyLoad::Print(s,flag);
  }
