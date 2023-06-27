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
                                                                        
// Written: Chris McGann, U.Washington
//          02.2011
//
// Description: This file contains the implementation for the SurfaceLoad class.

#include "SurfaceLoad.h"

//! @brief Constructor.
XC::SurfaceLoad::SurfaceLoad(int tag,const ID &tags_elems, const double &press)
  : ElementBodyLoad(tag, LOAD_TAG_SurfaceLoad, tags_elems), pressure(press)
  {}

//! @brief Default constructor.
XC::SurfaceLoad::SurfaceLoad(int tag, const double &press)
  : ElementBodyLoad(tag, LOAD_TAG_SurfaceLoad), pressure(press)
  {}

const XC::Vector &XC::SurfaceLoad::getData(int &type, const double &loadFactor) const
  {
    type = LOAD_TAG_SurfaceLoad;
    static Vector data(1);
    data(0)= pressure;
    return data;
  }

//! @brief Set the pressure.
double XC::SurfaceLoad::getPressure(void) const
  { return this->pressure; }

//! @brief Get the pressure.
void XC::SurfaceLoad::setPressure(const double &d)
  { this->pressure= d; }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::SurfaceLoad::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }

//! @brief Send data through the communicator argument.
int XC::SurfaceLoad::sendData(Communicator &comm)
  {
    int res= ElementBodyLoad::sendData(comm);
    res+= comm.sendDouble(pressure,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::SurfaceLoad::recvData(const Communicator &comm)
  {
    int res= ElementBodyLoad::recvData(comm);
    res+= comm.receiveDouble(pressure,getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::SurfaceLoad::sendSelf(Communicator &comm)
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

int XC::SurfaceLoad::recvSelf(const Communicator &comm)
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

void XC::SurfaceLoad::Print(std::ostream &os, int flag)
  {
    os << getClassName() <<"..."
       << "  element acted on: " << elemTags << std::endl;
  }

