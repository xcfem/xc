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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:57 $
// $Source: domain/load/groundMotion/InterpolatedGroundMotion.cpp,v $
                                                                        
                                                                        
// File: ~/earthquake/InterpolatedGroundMotion.C
// 
// Written: fmk 
// Created: 11/00
// Revision: A
//
// Description: This file contains the class definition for 
// InterpolatedGroundMotion. 
//
// What: "@(#) InterpolatedGroundMotion.C, revA"

#include "InterpolatedGroundMotion.h"
#include <classTags.h>
#include <utility/matrix/Vector.h>

XC::InterpolatedGroundMotion::InterpolatedGroundMotion()
:GroundMotion(GROUND_MOTION_TAG_InterpolatedGroundMotion),
 theMotions(0), factors(), deltaPeak(0.0) {}

XC::InterpolatedGroundMotion::InterpolatedGroundMotion(const InterpolatedGroundMotion &other)
  :GroundMotion(other),theMotions(other.theMotions), factors(other.factors), deltaPeak(other.deltaPeak)
  {}

XC::InterpolatedGroundMotion::InterpolatedGroundMotion(const vector_motions &groundMotions, const Vector &fact, double dT)
:GroundMotion(GROUND_MOTION_TAG_InterpolatedGroundMotion),
 theMotions(groundMotions), factors(fact), deltaPeak(dT)
  {
    assert(size_t(fact.Size()) == groundMotions.size());
  }

XC::InterpolatedGroundMotion &XC::InterpolatedGroundMotion::operator=(const InterpolatedGroundMotion &other)
  {
    GroundMotion::operator=(other);
    theMotions= other.theMotions;
    factors= other.factors;
    deltaPeak= other.deltaPeak;
    return *this;
  }

//! @brief Virtual constructor.
XC::GroundMotion *XC::InterpolatedGroundMotion::getCopy(void) const
  { return new InterpolatedGroundMotion(*this); }

//! @brief Destructor.
XC::InterpolatedGroundMotion::~InterpolatedGroundMotion(void)
  {}

//! @brief Return the duration of the motion history.
double XC::InterpolatedGroundMotion::getDuration(void) const
  {
    double value = 0.0;
    const int numMotions = factors.Size();
    for(int i=0; i<numMotions; i++)
      {
        const double motionValue= theMotions[i].getDuration();
        if(motionValue > value)
          value= motionValue;
      }
    return value;
  }

double XC::InterpolatedGroundMotion::getPeakAccel(void) const
  {
    double value = 0.0;
    const double duration = this->getDuration();
    double time = 0.0;
    while(time < duration)
      {
        const double accel = this->getAccel(time);
        if(accel > value)
          value = accel;
        time+= deltaPeak;
      }
    return value;
  }

double XC::InterpolatedGroundMotion::getPeakVel(void) const
  {
    double value = 0.0;
    const double duration = this->getDuration();
    double time = 0.0;
    while(time < duration)
      {
        const double vel = this->getVel(time);
        if(vel>value)
          value= vel;
        time+= deltaPeak;
      }
    return value;
  }

double XC::InterpolatedGroundMotion::getPeakDisp(void) const
  {
    double value= 0.0;
    const double duration = this->getDuration();
    double time= 0.0;
    while(time < duration)
      {
        const double disp= this->getDisp(time);
        if(disp > value)
          value= disp;
        time+= deltaPeak;
      }
    return value;
  }

double XC::InterpolatedGroundMotion::getAccel(double time) const
  {
    if(time < 0.0)
      return 0.0;

    double value= 0.0;
    const int numMotions = factors.Size();
    for(int i=0; i<numMotions; i++)
      { value += factors(i) * theMotions[i].getAccel(time); }
    return value;
  }

double XC::InterpolatedGroundMotion::getVel(double time) const
  {
    if(time < 0.0)
      return 0.0;
    double value = 0.0;
    const int numMotions = factors.Size();
    for(int i=0; i<numMotions; i++)
      { value += factors(i) * theMotions[i].getVel(time); }
    return value;
  }

double XC::InterpolatedGroundMotion::getDisp(double time) const
  {
    if(time < 0.0) return 0.0;

    double value = 0.0;
    const int numMotions = factors.Size();
    for(int i=0; i<numMotions; i++)
      { value += factors(i) * theMotions[i].getDisp(time); }
    return value;
  }

const XC::Vector &XC::InterpolatedGroundMotion::getDispVelAccel(double time) const
  {
    if(time < 0.0)
      {
        data(0) = 0.0;
        data(1) = 0.0;
        data(2) = 0.0;
        return data;
      }

    data.Zero();
    static XC::Vector motionData(3);

    const int numMotions = factors.Size();
    for(int i=0; i<numMotions; i++)
      {
        motionData= theMotions[i].getDispVelAccel(motionData,time);
        motionData*= factors(i);
        data+= motionData;
      }
    return data;
  }


int XC::InterpolatedGroundMotion::sendSelf(Communicator &comm)
  {
    std::cerr << "XC::InterpolatedGroundMotion::sendSelf() -- not yet implemented" << std::endl;
    return -1;
  }


int XC::InterpolatedGroundMotion::recvSelf(const Communicator &comm)
  {
    std::cerr << "XC::InterpolatedGroundMotion::recvSelf() -- not yet impelemented" << std::endl;
    return -1;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::InterpolatedGroundMotion::getPyDict(void) const
  {
    boost::python::dict retval= GroundMotion::getPyDict();
    boost::python::list vector_motions_lst;
    for(vector_motions::const_iterator i= theMotions.begin(); i!= theMotions.end(); i++)
      {
	MotionHistory mh= (*i);
	vector_motions_lst.append(mh.getPyDict());
      }
    retval["vector_motions"]= vector_motions_lst;
    retval["factors"]= factors.getPyList();
    retval["deltaPeak"]= deltaPeak;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::InterpolatedGroundMotion::setPyDict(const boost::python::dict &d)
  {
    GroundMotion::setPyDict(d);
    boost::python::list vector_motions_lst= boost::python::extract<boost::python::list>(d["vector_motions"]);
    const size_t sz= boost::python::len(vector_motions_lst);
    theMotions.resize(sz);
    for(size_t i= 0; i<sz; i++)
      {
	boost::python::dict tmp= boost::python::extract<boost::python::dict>(vector_motions_lst[i]);
	theMotions[i].setPyDict(tmp);
      }
    factors= Vector(boost::python::extract<boost::python::list>(d["factors"]));
    deltaPeak= boost::python::extract<double>(d["deltaPeak"]);
  }

// AddingSensitivity:BEGIN //////////////////////////////////////////
double XC::InterpolatedGroundMotion::getAccelSensitivity(double time)
  {
    double value= 0.0;
    const int numMotions = factors.Size();
    for(int i=0; i<numMotions; i++)
      { value += factors(i) * theMotions[i].getAccelSensitivity(time); }
    return value;
  }
int XC::InterpolatedGroundMotion::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= 0;
    const int numMotions = factors.Size();
    for(int i=0; i<numMotions; i++)
      retval= theMotions[i].setParameter(argv,param);
    return retval;
  }

int XC::InterpolatedGroundMotion::updateParameter(int parameterID, Information &info)
  {
    int retval= 0;
    const int numMotions = factors.Size();
    for(int i=0; i<numMotions; i++)
      retval= theMotions[i].updateParameter(parameterID,info);
    return retval;
  }

int XC::InterpolatedGroundMotion::activateParameter(int pparameterID)
  {
    int retval= 0;
    const int numMotions = factors.Size();
    for(int i=0; i<numMotions; i++)
      retval= theMotions[i].activateParameter(pparameterID);
    return retval;
  }

// AddingSensitivity:END ////////////////////////////////////


