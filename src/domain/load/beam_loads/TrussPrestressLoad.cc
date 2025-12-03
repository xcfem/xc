//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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


#include "TrussPrestressLoad.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::TrussPrestressLoad::TrussPrestressLoad(int tag,const double &t1, const double &t2,const ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_TrussPrestressLoad, theElementTags), sg1(t1),  sg2(t2) {}

//! @brief Constructor.
XC::TrussPrestressLoad::TrussPrestressLoad(int tag,const double &temp,const ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_TrussPrestressLoad, theElementTags), sg1(temp),  sg2(temp) {}

//! @brief Constructor.
XC::TrussPrestressLoad::TrussPrestressLoad(int tag, const ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_TrussPrestressLoad, theElementTags), sg1(0.0), sg2(0.0) {}

//! @brief Constructor.
XC::TrussPrestressLoad::TrussPrestressLoad(int tag)
  :ElementBodyLoad(tag, LOAD_TAG_TrussPrestressLoad), sg1(0.0), sg2(0.0) {}

//! @brief Default constructor.
XC::TrussPrestressLoad::TrussPrestressLoad(void)
  :ElementBodyLoad(LOAD_TAG_TrussPrestressLoad), sg1(0.0), sg2(0.0) {}

//! @brief Return the category of this kind of loads.
std::string XC::TrussPrestressLoad::Category(void) const
  { return "truss_prestress"; }

//! @brief Return the axial strains at both ends of the truss element.
std::vector<XC::Vector> XC::TrussPrestressLoad::getStresses(void) const
  {
    std::vector<Vector> retval(2);
    Vector tmp(1);
    tmp[0]= this->sg1;
    retval[0]= tmp;
    tmp[1]= this->sg2;
    retval[1]= tmp;
    return retval;
  }

//! @brief Return the axial strains at both ends of the given truss element.
XC::Matrix XC::TrussPrestressLoad::getElementStressesMatrix(const Element &e) const
  {
    Matrix retval(2, 1);
    const int elemTag= e.getTag();
    if(this->actsOnElement(elemTag))
      {
	// For now, we assume all loaded elements have the
	// same number of gauss points.
	retval(0,0)= this->sg1;
	retval(1,0)= this->sg2;
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << ": element with tag: " << elemTag
		  << " not loaded."
		  << std::endl;
      }
    return retval;
  }

const XC::Vector &XC::TrussPrestressLoad::getData(int &type, const double &loadFactor) const
  {
    type = getClassTag();
    static Vector data(2);
    data(0)= this->sg1;
    data(1)= this->sg2;
    return data;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::TrussPrestressLoad::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }

//! @brief Send data through the communicator argument.
int XC::TrussPrestressLoad::sendData(Communicator &comm)
  {
    int res= ElementBodyLoad::sendData(comm);
    res+= comm.sendDoubles(this->sg1, this->sg2,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::TrussPrestressLoad::recvData(const Communicator &comm)
  {
    int res= ElementBodyLoad::recvData(comm);
    res+= comm.receiveDoubles(this->sg1, this->sg2,getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::TrussPrestressLoad::sendSelf(Communicator &comm)
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

int XC::TrussPrestressLoad::recvSelf(const Communicator &comm)
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

void XC::TrussPrestressLoad::Print(std::ostream &s, int flag) const
  {
    s << "TrussPrestressLoad - reference load : "
      << " stress at node 1 : " << this->sg1
      << " stress at node 2 : " << this->sg2;
    ElementBodyLoad::Print(s,flag);
  }
