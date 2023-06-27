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
// Description: This file contains the implementation for the SurfaceLoader class.

#include "SurfaceLoader.h"

XC::Vector XC::SurfaceLoader::data(1);

//! @brief Constructor.
XC::SurfaceLoader::SurfaceLoader(int tag,const ID &tags_elems)
  : ElementBodyLoad(tag, LOAD_TAG_SurfaceLoader, tags_elems)
  {}

//! @brief Default constructor.
XC::SurfaceLoader::SurfaceLoader(int tag)
  : ElementBodyLoad(tag, LOAD_TAG_SurfaceLoader)
  {}

const XC::Vector &XC::SurfaceLoader::getData(int &type, const double &loadFactor) const
  {
    type = LOAD_TAG_SurfaceLoader;
    return data;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::SurfaceLoader::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }

//! @brief Send data through the communicator argument.
int XC::SurfaceLoader::sendData(Communicator &comm)
  {
    int res= ElementBodyLoad::sendData(comm);
    res+= comm.sendDouble(this->data[0],getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::SurfaceLoader::recvData(const Communicator &comm)
  {
    int res= ElementBodyLoad::recvData(comm);
    res+= comm.receiveDouble(this->data[0],getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::SurfaceLoader::sendSelf(Communicator &comm)
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

int XC::SurfaceLoader::recvSelf(const Communicator &comm)
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

void XC::SurfaceLoader::Print(std::ostream &os, int flag)
  {
    os << getClassName() <<"..."
       << "  element acted on: " << elemTags << std::endl;
  }

