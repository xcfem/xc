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
#include "utility/geom/pos_vec/Vector3d.h"
#include "domain/mesh/element/Element.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Default constructor.
XC::SurfaceLoad::SurfaceLoad(int tag, const int &dm, const double &press)
  : ElementBodyLoad(tag, LOAD_TAG_SurfaceLoad), dim(dm), pressure(press)
  {}

//! @brief Constructor.
XC::SurfaceLoad::SurfaceLoad(int tag, const ID &tags_elems, const int &dm, const double &press)
  : ElementBodyLoad(tag, LOAD_TAG_SurfaceLoad, tags_elems), dim(dm), pressure(press)
  {}

const XC::Vector &XC::SurfaceLoad::getData(int &type, const double &loadFactor) const
  {
    type = LOAD_TAG_SurfaceLoad;
    static Vector data(1);
    data(0)= pressure;
    return data;
  }

std::string XC::SurfaceLoad::Category(void) const
  { return "uniform"; }


//! @brief Get the dimension of the space (2 or 3).
int XC::SurfaceLoad::getDim(void) const
  { return this->dim; }

//! @brief Set the dimension of the space (2 or 3).
void XC::SurfaceLoad::setDim(const int &i)
  { this->dim= i; }

//! @brief Set the pressure.
double XC::SurfaceLoad::getPressure(void) const
  { return this->pressure; }

//! @brief Get the pressure.
void XC::SurfaceLoad::setPressure(const double &d)
  { this->pressure= d; }

//! @brief Returns force expressed in local coordinates.
XC::Vector XC::SurfaceLoad::getLocalForce(void) const
  {
    Vector retval(dim);
    retval(0)= 0.0;
    retval(1)= 0.0;
    retval(dim-1)= pressure;
    return retval;
  }

//! @brief Return the local components of the
//! force in a Vector3d. 
Vector3d XC::SurfaceLoad::getVector3dLocalForce(void) const
  {
    Vector f= getLocalForce();
    Vector3d retval;
    if(f.Size()>2)
      retval= Vector3d(f[0],f[1],f[2]);
    else
      retval= Vector3d(f[0],f[1],0.0);
    return retval;
  }

//! @brief Applies the load to the elements.
void XC::SurfaceLoad::applyLoad(double loadFactor) 
  {
    const ElementPtrs &elemPtrs= this->getElementPtrs();
    const int sz= elemPtrs.size();
    for(int i=0; i<sz; i++)
      if(elemPtrs[i])
	{
	  const Element *elem= elemPtrs[i];
	  const int elemDim= elem->getDimension();
	  if(elemDim!=(this->dim-1))
	    {
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; - element with tag: "
		        << elem->getTag()
		        << " of class: "
			<< elem->getClassName()
		        << " has dimension: " << elemDim
		        << " and this surface load has dimension: "
		        << dim
		        << "; so it is not compatible."
		        << Color::def << std::endl;
	      exit(-1);
	    }
	}
    ElementBodyLoad::applyLoad(loadFactor);
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::SurfaceLoad::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Send data through the communicator argument.
int XC::SurfaceLoad::sendData(Communicator &comm)
  {
    int res= ElementBodyLoad::sendData(comm);
    res+= comm.sendInt(dim,getDbTagData(),CommMetaData(3));
    res+= comm.sendDouble(pressure,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::SurfaceLoad::recvData(const Communicator &comm)
  {
    int res= ElementBodyLoad::recvData(comm);
    res+= comm.receiveInt(dim,getDbTagData(),CommMetaData(3));
    res+= comm.receiveDouble(pressure,getDbTagData(),CommMetaData(4));
    return res;
  }

int XC::SurfaceLoad::sendSelf(Communicator &comm)
  {
    inicComm(5);
    int result= sendData(comm);
    const int dbTag= getDbTag();
    result+= comm.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; - failed to send data."
	        << Color::def << std::endl;
    return result;
  }

int XC::SurfaceLoad::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; data could not be received"
		<< Color::def << std::endl;
    else
      res+= recvData(comm);
    return res;
  }

void XC::SurfaceLoad::Print(std::ostream &os, int flag)
  {
    os << getClassName() <<"..."
       << "  element acted on: " << elemTags << std::endl;
  }

