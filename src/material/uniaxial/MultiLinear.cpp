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

// Written: fmk 
// Created: 05/12
// Revision: A
//
// Description: This file contains the class implementation for multilinear material. 
//
// What: "@(#) MultiLinear.C, revA"

#include "material/uniaxial/MultiLinear.h"
#include "domain/component/Parameter.h"
#include "domain/mesh/element/utils/Information.h"

void XC::MultiLinear::setup_data(void)
  {
    
    data(0, 0)= -e0(0); // neg yield strain
    data(0, 1)= e0(0);  // pos yield strain
    data(0, 2)= -s0(0); // neg yield stress
    data(0, 3)= s0(0);  // pos yield stress
    data(0, 4)= s0(0) / e0(0);  // slope
    data(0, 5)= e0(0);  // dist - (0-1)/2
    
    for(int i = 1; i < numSlope; i++)
      {
        data(i, 0) = -e0(i);
        data(i, 1) = e0(i);
        data(i, 2) = -s0(i);
        data(i, 3) = s0(i);
        data(i, 4) = (s0(i) - s0(i - 1)) / (e0(i) - e0(i - 1));
        data(i, 5) = e0(i) - e0(i - 1);
      }
  }

void XC::MultiLinear::setup(const Vector &ss, const Vector &ee)
  {
    if(fabs(ee[0])<1e-8)
      {
	std::clog << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: (0, 0) is always the first point of"
		  << " the material response. Data ignored."
	          << std::endl;
	const int sz= ee.Size();
	this->e0.resize(sz-1);
	this->s0.resize(sz-1);
	for(int i= 1;i<sz;i++)
	  {
	    e0[i-1]= ee[i];
	    s0[i-1]= ss[i];
	  }
      }
    else
      {
        this->e0= ee; //strain
        this->s0= ss; //stress
      }
    this->numSlope = this->e0.Size();
    data.resize(numSlope, 6);
    
    for(int i = 1; i < numSlope; i++)
      {
        if (e0(i) < e0(i - 1))
	  {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; ERROR: MultiLinear strain_i+1 < strain_i\n"
		      << "Continuing with strain_i+1 = 1.2*strain_1\n";
          }
      }
    this->setup_data();

    // Trial values.
    tStrain = 0.0;
    tStress = 0.0;
    tTangent= data(0, 4);
    // Commited values.
    cStrain = 0.0;
    cStress = 0.0;
    cTangent= tTangent;
    
    tSlope = 0;
  }

//! @brief Set values from a python list of tuples.
void XC::MultiLinear::setupPyList(const boost::python::list &points)
  {
    const size_t sz= len(points);
    Vector ss(sz);
    Vector ee(sz);
    for(size_t i=0; i<sz; i++)
      {
	const boost::python::tuple t= boost::python::extract<boost::python::tuple>(points[i]);
	ee(i)= boost::python::extract<double>(t[0]);
	ss(i)= boost::python::extract<double>(t[1]);
      }
    setup(ss, ee);
  }

XC::MultiLinear::MultiLinear(int tag)
  :UniaxialMaterial(tag, MAT_TAG_MultiLinear), numSlope(0)
  {
    tStrain = 0;
    tStress = 0;
    tTangent = 0;
    
    cStrain = 0;
    cStress = 0;
    cTangent = 0;
    tSlope = 0;
  }

XC::MultiLinear::MultiLinear(int tag, const Vector& s, const Vector& e)
  :UniaxialMaterial(tag, MAT_TAG_MultiLinear), numSlope(0)
  { setup(s, e); }

//! @brief Destructor.
XC::MultiLinear::~MultiLinear(void)
  {
    // does nothing
  }

int XC::MultiLinear::setTrialStrain(double strain, double strainRate)
  {
    if (fabs(tStrain - strain) < DBL_EPSILON)
        return 0;
    
    tStrain = strain;
    tSlope = 0;
    if(tStrain >= data(0, 0) && tStrain <= data(0, 1))
      { // elastic
        tSlope = 0;
        tStress = data(0, 2) + (tStrain - data(0, 0)) * data(0, 4);
        tTangent = data(0, 4);
      }
    else if(tStrain < data(0, 0))
      { // search neg side of data
        tSlope = 1;
        while(tSlope < numSlope && tStrain < data(tSlope, 0))
            tSlope++;
        if(tSlope == numSlope)
            tSlope = numSlope - 1;
        tTangent = data(tSlope, 4);
        tStress = data(tSlope, 2) + (tStrain - data(tSlope, 0)) * tTangent;
      }
    else
      { // search pos side of data
        tSlope = 1;
        while(tSlope < numSlope && tStrain > data(tSlope, 1))
            tSlope++;
        if (tSlope == numSlope)
            tSlope = numSlope - 1;
        tTangent = data(tSlope, 4);
        tStress = data(tSlope, 3) + (tStrain - data(tSlope, 1)) * tTangent;
      }
    return 0;
  }

double XC::MultiLinear::getStrain(void) const
  { return tStrain; }

double XC::MultiLinear::getStress(void) const
  { return tStress; }

double XC::MultiLinear::getTangent(void) const
  { return tTangent; }

int XC::MultiLinear::commitState(void)
  {
    // if yielded we need to reset the values               
    if(tSlope != 0)
      { // yielded
        if (tStrain > data(0, 1))
	  {  // positive yield direction
            
            // set elastic bounds
            data(0, 1) = tStrain;
            data(0, 3) = tStress;
            data(0, 0) = tStrain - 2 * data(0, 5);
            data(0, 2) = tStress - 2 * data(0, 5) * data(0, 4);
            
            // reset bounds for all those pts before yield
            //  - pos & neg affected
            for(int i = 1; i < tSlope; i++)
	      {
                data(i, 1) = tStrain;
                data(i, 3) = tStress;
                data(i, 0) = data(i - 1, 0) - 2 * data(i, 5);
                data(i, 2) = data(i - 1, 2) - 2 * data(i, 5) * data(i, 4);
              }
            
            // reset bounds for all those pts after
            //  - neg affected 
            data(tSlope, 0) = data(tSlope - 1, 0) - 2 * data(tSlope, 5)
                            + data(tSlope, 1) - data(tSlope - 1, 1);
            data(tSlope, 2) = data(tSlope - 1, 2)
                            + (data(tSlope, 0) - data(tSlope - 1, 0)) * data(tSlope, 4);
            
            for (int i = tSlope + 1; i < numSlope; i++)
	      {
                data(i, 0) = data(i - 1, 0) - 2 * data(i, 5)
                           + data(i, 1) - data(i - 1, 1);
                data(i, 2) = data(i - 1, 2)
                           + (data(i, 0) - data(i - 1, 0)) * data(i, 4);
              }
	  }
        else
	  {  // neg yield direction

            // set elastic bounds
            data(0, 0) = tStrain;
            data(0, 2) = tStress;
            data(0, 1) = tStrain + 2 * data(0, 5);
            data(0, 3) = tStress + 2 * data(0, 5) * data(0, 4);
            
            // reset bounds for all those pts before yield slope
            //  - pos & neg affected 
            for (int i = 1; i < tSlope; i++)
	      {
                data(i, 0) = tStrain;
                data(i, 2) = tStress;
                data(i, 1) = data(i - 1, 1) + 2 * data(i, 5);
                data(i, 3) = data(i - 1, 3) + 2 * data(i, 5) * data(i, 4);
              }
            
            // reset bounds for all those pts after
            //  - pos pts affected 
            data(tSlope, 1) = data(tSlope - 1, 1) + 2 * data(tSlope, 5)
                            + data(tSlope, 0) - data(tSlope - 1, 0);
            data(tSlope, 3) = data(tSlope - 1, 3)
                            + (data(tSlope, 1) - data(tSlope - 1, 1)) * data(tSlope, 4);
            
            for(int i = tSlope + 1; i < numSlope; i++)
	      {
                data(i, 1) = data(i - 1, 1) + 2 * data(i, 5)
                           + data(i, 0) - data(i - 1, 0);
                data(i, 3) = data(i - 1, 3)
                           + (data(i, 1) - data(i - 1, 1)) * data(i, 4);
              }
	  }
      }
    
    cStress = tStress;
    cStrain = tStrain;
    cTangent = tTangent;
    return 0;
  }

int XC::MultiLinear::revertToLastCommit(void)
  {
    tStrain = cStrain;
    tTangent = cTangent;
    tStress = cStress;
    
    return 0;
  }

int XC::MultiLinear::revertToStart(void)
  {
    data(0, 1) = data(0, 5);
    data(0, 3) = data(0, 5) * data(0, 4);
    data(0, 0) = -data(0, 1);
    data(0, 2) = -data(0, 3);
    
    for(int i = 1; i < numSlope; i++)
      {
        data(i, 1) = data(i - 1, 1) + data(i, 5);
        data(i, 3) = data(i - 1, 3) + data(i, 5) * data(i, 4);
        data(i, 0) = -data(i, 1);
        data(i, 2) = -data(i, 3);
      }
    
    data(0, 0) = -e0(0);      // neg yield strain
    data(0, 1) = e0(0);       // pos yield strain
    data(0, 2) = -s0(0);      // neg yield stress
    data(0, 3) = s0(0);       // pos yield stress
    data(0, 4) = s0(0) / e0(0);  // slope
    data(0, 5) = e0(0);       // dist - (0-1)/2
    
    for (int i = 1; i < numSlope; i++) {
        data(i, 0) = -e0(i);
        data(i, 1) = e0(i);
        data(i, 2) = -s0(i);
        data(i, 3) = s0(i);
        data(i, 4) = (s0(i) - s0(i - 1)) / (e0(i) - e0(i - 1));
        data(i, 5) = e0(i) - e0(i - 1);
    }

    
    tStrain = cStrain = 0.0;
    tStress = cStress = 0.0;
    tTangent = data(0, 4);
    cTangent = tTangent;

    tSlope = 0;
    
    return 0;
  }

//! @brief Virtual constructor.
XC::UniaxialMaterial *XC::MultiLinear::getCopy(void) const
  { return new MultiLinear(*this); }

//! @brief Return the number of intervals used to define the material.
int XC::MultiLinear::getNumPoints(void) const
  { return numSlope; }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::MultiLinear::getDbTagData(void) const
  {
    static DbTagData retval(8);
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::MultiLinear::sendData(Communicator &comm)
  {
    int res= UniaxialMaterial::sendData(comm);
    res+= comm.sendVector(e0, getDbTagData(),CommMetaData(2));
    res+= comm.sendVector(s0, getDbTagData(),CommMetaData(3));
    res+= comm.sendMatrix(data, getDbTagData(),CommMetaData(4));
    res+= comm.sendInts(numSlope, tSlope, getDbTagData(),CommMetaData(5));
    res+= comm.sendDoubles(tStrain, tStress, tTangent, getDbTagData(),CommMetaData(6));
    res+= comm.sendDoubles(cStrain, cStress, cTangent, getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::MultiLinear::recvData(const Communicator &comm)
  {
    int res= UniaxialMaterial::recvData(comm);
    res+= comm.receiveVector(e0,getDbTagData(),CommMetaData(2));
    res+= comm.receiveVector(s0, getDbTagData(),CommMetaData(3));
    res+= comm.receiveMatrix(data, getDbTagData(),CommMetaData(4));
    res+= comm.receiveInts(numSlope, tSlope, getDbTagData(),CommMetaData(5));
    res+= comm.receiveDoubles(tStrain, tStress, tTangent, getDbTagData(),CommMetaData(6));
    res+= comm.receiveDoubles(cStrain, cStress, cTangent, getDbTagData(),CommMetaData(7));
    return res;
  }

int XC::MultiLinear::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data\n";
    return res;
  }

int XC::MultiLinear::recvSelf(const Communicator &comm)
  {
    inicComm(8);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        res= recvData(comm);
        if(res < 0)
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; - failed to receive data\n";
      }
    return res;
  }

void XC::MultiLinear::Print(std::ostream &s, int flag) const
  {
    s << "MultiLinear tag: " << this->getTag() << std::endl;
    s << "  stress: " << tStress << " tangent: " << tTangent << std::endl;
    s << "tSlope: " << tSlope << "numSlope: " << numSlope << std::endl;
    s << data;
  }

// AddingSensitivity:BEGIN ///////////////////////////////////
int XC::MultiLinear::setParameter(const std::vector<std::string>  &argv, Parameter &param)
  {
    const size_t argc= argv.size();
    // trying to make this a little more general for arbitrary numbers of points
    // expecting parameters named stress1, strain1, stress3, etc.
    int dindx = -1;

    if (argc < 2)
      { return -1; }
    
    dindx = atoi(argv[1]);
    if(dindx < 1 || dindx > numSlope)
      { return -1; }
    
    if(argv[0]=="stress")
      {
        param.setValue( data(dindx-1,3) );
        return param.addObject(100+dindx, this);
      }
    if(argv[0]=="strain")
      {
        param.setValue( data(dindx-1,1) );
        return param.addObject(200+dindx, this);
      }

    return -1;
  }


int XC::MultiLinear::updateParameter(int parameterID, Information &info)
 {
    int dindx = 0;
    double sprev = 0;
    double eprev = 0;
    
    if(parameterID == -1)
        return -1;
    else if (parameterID > 100 && parameterID <= 100+numSlope)
      {
        // stress terms
        dindx = parameterID-100;
        data(dindx-1,2) = -1.0*info.theDouble;
        data(dindx-1,3) = info.theDouble;
      }
    else if (parameterID > 200 && parameterID <= 200+numSlope)
      {
        // strain terms
        dindx = parameterID-200;
        data(dindx-1,0) = -1.0*info.theDouble;
        data(dindx-1,1) = info.theDouble;
      }
    else
      { return -1; }
    
    // update slopes
    if (dindx > 1)
      {
        sprev = data(dindx-2,3);
        eprev = data(dindx-2,1);
      }
    data(dindx-1,4) = (data(dindx-1,3) - sprev) / (data(dindx-1,1) - eprev);
    data(dindx-1,5) = data(dindx-1,1) - eprev;
    
    return 0;
  }
