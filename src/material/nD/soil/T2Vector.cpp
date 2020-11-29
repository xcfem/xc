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
// $Revision: 1.10 $
// $Date: 2003/02/14 23:01:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/XC::T2Vector.cpp,v $
                                                                        
// Written: ZHY
// Created: August 2000

//
// T2Vector.cpp
// ----------
//
#include <cmath>
#include <cstdlib>
#include "T2Vector.h"


XC::Vector XC::T2Vector::engrgStrain(6);

//! @brief scalar product of two second order tensor vectors
double XC::operator && (const XC::Vector & a, const XC::Vector & b)
  {
    if(a.Size() !=6 || b.Size() !=6)
      {
        std::cerr << "T2Vector::" << __FUNCTION__
	          << "; FATAL:operator && (Vector &, Vector &): "
	          << " vector size not equal 6" << std::endl;
        exit(-1);
      }

    double result = 0.;  

    for(int i=0; i<3; i++)
      result += a[i]*b[i] + 2*a[i+3]*b[i+3];
    return result;
  }


// XC::T2Vector class methods
XC::T2Vector::T2Vector(void) 
  : MovableObject(0), theT2Vector(6), theDeviator(6), theVolume(0.0) {}


XC::T2Vector::T2Vector(const Vector &init, int isEngrgStrain)
  :MovableObject(0),theT2Vector(6), theDeviator(6), theVolume(0)
  {
    if(init.Size() != 6)
      {
        std::cerr << "T2Vector::" << __FUNCTION__
                  << "; FATAL: vector size not equal to 6"
		  << std::endl;
        exit(-1);
    }
    theT2Vector = init;

    theVolume = (theT2Vector[0]+theT2Vector[1]+theT2Vector[2])/3.0;
    for(int i=0; i<3; i++)
      {
        theDeviator[i] = theT2Vector[i] - theVolume;
        theDeviator[i+3] = theT2Vector[i+3];
        if(isEngrgStrain==1)
	  {
	    theDeviator[i+3] /= 2.;
	    theT2Vector[i+3] /= 2.;
          }
      }
  }


XC::T2Vector::T2Vector(const XC::Vector & deviat_init, double volume_init)
 :MovableObject(0), theT2Vector(6), theDeviator(6), theVolume(volume_init)
  {
    if(deviat_init.Size() != 6)
      {
        std::cerr << "T2Vector::" << __FUNCTION__
                  << "; FATAL: vector size not equal to 6"
		  << std::endl;
        exit(-1);
      }

    //make sure the deviator has truly volume=0 
    double devolum = (deviat_init[0]+deviat_init[1]+deviat_init[2])/3.;

    for(int i=0; i<3; i++)
      {
	theDeviator[i] = deviat_init[i] - devolum;
	theDeviator[i+3] = deviat_init[i+3];
	theT2Vector[i] = theDeviator[i] + theVolume;
	theT2Vector[i+3] = theDeviator[i+3]; 
      }
  }


void XC::T2Vector::setData(const Vector &init, int isEngrgStrain)
  {
    if(init.Size() != 6)
      {
        std::cerr << "T2Vector::" << __FUNCTION__
                  << "; FATAL: vector size not equal to 6"
		  << std::endl;
        exit(-1);
      }

    theT2Vector = init;

    theVolume = (theT2Vector[0]+theT2Vector[1]+theT2Vector[2])/3.0;
    for(int i=0; i<3; i++)
      {
        theDeviator[i] = theT2Vector[i] - theVolume;
        theDeviator[i+3] = theT2Vector[i+3];
        if(isEngrgStrain==1)
	  {
	    theDeviator[i+3] /= 2.;
	    theT2Vector[i+3] /= 2.;
          }
      }
  }

void XC::T2Vector::setData(const Vector &deviat, double volume)
  {
    theVolume = volume;

    if (deviat.Size() != 6)
      {
	std::cerr << "T2Vector::" << __FUNCTION__
		  << "; FATAL: vector size not equal to 6"
		  << std::endl;
	exit(-1);
      }

    //make sure the deviator has truly volume=0 
    double devolum = (deviat[0]+deviat[1]+deviat[2])/3.;

    for(int i=0; i<3; i++)
      {
        theDeviator[i] = deviat[i] - devolum;
        theDeviator[i+3] = deviat[i+3];
        theT2Vector[i] = theDeviator[i] + theVolume;
        theT2Vector[i+3] = theDeviator[i+3]; 
      }
  }

const XC::Vector &XC::T2Vector::t2Vector(int isEngrgStrain) const
  {
    if (isEngrgStrain==0) return theT2Vector;

    engrgStrain = theT2Vector;
    for(int i=0; i<3; i++)
      { engrgStrain[i+3] *= 2.; }
    return engrgStrain;
  }


const XC::Vector & XC::T2Vector::deviator(int isEngrgStrain) const
  {
    if (isEngrgStrain==0) return theDeviator;

    engrgStrain = theDeviator;
    for(int i=0; i<3; i++)
      { engrgStrain[i+3] *= 2.; }
    return engrgStrain;
  }


double XC::T2Vector::t2VectorLength() const
  { return sqrt(theT2Vector && theT2Vector); }


double XC::T2Vector::deviatorLength() const
  { return sqrt(theDeviator && theDeviator); }


double XC::T2Vector::octahedralShear(int isEngrgStain) const
  {
    if (isEngrgStain) 
      return 2.* sqrt(1. / 3.) * deviatorLength();
    else
      return sqrt(1. / 3.) * deviatorLength();
  }


double XC::T2Vector::deviatorRatio(double residualPress) const
  {
    if((fabs(theVolume)+fabs(residualPress)) <= LOW_LIMIT)
      {
	  std::cerr << "T2Vector::" << __FUNCTION__
		    << "; FATAL: volume <=" << LOW_LIMIT << std::endl;
	  exit(-1);
      }
    return sqrt(3./2.* (theDeviator && theDeviator)) / (fabs(theVolume)+fabs(residualPress));
  }


const XC::Vector &XC::T2Vector::unitT2Vector(void) const
  {
    engrgStrain = theT2Vector;	
    double length = this->t2VectorLength();
    if(length <= LOW_LIMIT)
      {
	std::cerr << "T2Vector::" << __FUNCTION__
		  << "; WARNING: vector length <=" << LOW_LIMIT
		  << std::endl;
	engrgStrain /= LOW_LIMIT;
      }
    else
      engrgStrain /= length;

    return engrgStrain;
  }


const XC::Vector &XC::T2Vector::unitDeviator(void) const
  {
    engrgStrain = theDeviator;	
    double length = this->deviatorLength();
    if(length <= LOW_LIMIT)
      {
	std::cerr << "T2Vector::" << __FUNCTION__
		  << "; WARNING: vector length <="
		  << LOW_LIMIT << std::endl;
        engrgStrain /= LOW_LIMIT;
      }
    else
      engrgStrain /= length;
    return engrgStrain;
  }


double XC::T2Vector::angleBetweenT2Vector(const T2Vector & a) const
  {
    if (t2VectorLength() <= LOW_LIMIT || a.t2VectorLength() <= LOW_LIMIT)
      {
	std::cerr << "T2Vector::" << __FUNCTION__
		  << "; FATAL: vector length <=" << LOW_LIMIT
		  << std::endl;
	exit(-1);
      }

    double angle = (theT2Vector && a.theT2Vector) / (t2VectorLength() * a.t2VectorLength());
    if(angle > 1.) angle = 1.;
    if(angle < -1.) angle = -1.;

    return acos(angle);
  }


double XC::T2Vector::angleBetweenDeviator(const T2Vector & a) const
  {
    if (deviatorLength() <= LOW_LIMIT || a.deviatorLength() <= LOW_LIMIT)
      {
	std::cerr << "T2Vector::" << __FUNCTION__
		  << "; FATAL: vector length <="
		  << LOW_LIMIT << std::endl;
	exit(-1);
      }

    double angle = (theDeviator && a.theDeviator) / (deviatorLength() * a.deviatorLength());
    if(angle > 1.) angle = 1.;
    if(angle < -1.) angle = -1.;

    return acos(angle);
  }


int XC::T2Vector::operator == (const T2Vector & a) const
  {
    for(int i=0; i<6; i++)
    if(theT2Vector[i] != a.theT2Vector[i])
      return 0;
    return 1;
  }

int XC::T2Vector::isZero(void) const
  {
    for(int i=0; i<6; i++)
      if(theT2Vector[i] != 0.0) return 0;

    return 1;
  }

//! @brief Send object members through the communicator argument.
int XC::T2Vector::sendData(Communicator &comm)
  {
    //setDbTagDataPos(0,getTag());
    int res= comm.sendDouble(theVolume,getDbTagData(),CommMetaData(0));
    res+= comm.sendVector(theT2Vector,getDbTagData(),CommMetaData(1));
    res+= comm.sendVector(theDeviator,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::T2Vector::recvData(const Communicator &comm)
  {
    //setTag(getDbTagDataPos(0));
    int res= comm.receiveDouble(theVolume,getDbTagData(),CommMetaData(0));
    res+= comm.receiveVector(theT2Vector,getDbTagData(),CommMetaData(1));
    res+= comm.receiveVector(theDeviator,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::T2Vector::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "T2Vector::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::T2Vector::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "T2Vector::" << __FUNCTION__
                << "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << "T2Vector::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }



/*********************
ostream & operator<< (ostream & os, const XC::T2Vector & a)
{
  os.precision(16);
  os.setf(ios::showpoint);

  os << "theT2Vector = " << a.t2Vector() << std::endl;
  os << "theDeviator = " << a.deviator() << std::endl;
  os << "theVolume = " << a.volume() << std::endl;

  return os;
}


istream & operator>> (istream & is, XC::T2Vector & a)
{
  Vector temp;

  is >> temp;
  a = XC::T2Vector(temp);

  return is;
}
*/








