// -*-c++-*-
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

// $Revision: 1.0 $
// $Date: 2019-01-28 17:53:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/damping/Damping.cpp,v $

// Written: Yuli Huang (yulee@berkeley.edu)
// Created: 02/2020
// Revision: A
//
// Description: This file contains the implementation for the Damping class.
// Damping provides the abstraction of an elemental damping imposition.
// It is an abstract base class and thus no objects of it's type can be instatiated
// It has pure virtual functions which  must be implemented in it's derived classes.
// Reference:
// Yuan Tian, Yuli Huang, Zhe Qu, Yifan Fei, Xinzheng Lu,
// High-performance uniform damping model for response history analysis in OpenSees,
// Journal of Earthquake Engineering,
// 2022,
// https://doi.org/10.1080/13632469.2022.2124557
//

#include "Damping.h"
#include "utility/tagged/storage/MapOfTaggedObjects.h"
#include "domain/load/pattern/TimeSeries.h"

//static XC::MapOfTaggedObjects theDampingObjects;


void XC::Damping::free(void)
  { setTimeSeries(nullptr); }

// constructor:
XC::Damping::Damping(int tag, int classTag,  double t1, double t2, TimeSeries *f)
  : TaggedObject(tag), MovableObject(classTag),
    theDomain(nullptr), fac(f),
    ta(t1), td(t2), qd(), q0(), q0C()
  {}

//! @brief Copy constructor.
XC::Damping::Damping(const Damping &other)
  : TaggedObject(other), MovableObject(other),
    theDomain(other.theDomain), fac(nullptr),
    ta(other.ta), td(other.td), qd(other.qd), q0(other.q0), q0C(other.q0C) 
  { setTimeSeries(other.fac); }

//! @brief Assignment operator.
XC::Damping &XC::Damping::operator=(const Damping &other)
  {
    TaggedObject::operator=(other);
    MovableObject::operator=(other);
    theDomain= other.theDomain;
    setTimeSeries(other.fac);
    ta= other.ta;
    td= other.td;
    qd= other.qd;
    q0= other.q0;
    q0C= other.q0C;
    return *this;
  }

// destructor:
XC::Damping::~Damping(void)
  {}

//! @brief Return the time series pointer (const version).
const XC::TimeSeries *XC::Damping::getTimeSeries(void) const
  { return fac; }

//! @brief Return the time series pointer.
XC::TimeSeries *XC::Damping::getTimeSeries(void)
  { return fac; }

//! @brief Set the time series for the pattern.
void XC::Damping::setTimeSeries(TimeSeries *theTimeSeries)
  {
    // invoke the destructor on the old TimeSeries
    if(fac)
      { 
        delete fac;
        fac= nullptr;
      }
    // set the pointer to the new series object
    if(theTimeSeries)
      fac= theTimeSeries->getCopy();
    else
      fac= nullptr;
  }

//! @brief Send object members through the communicator argument.
int XC::Damping::sendData(Communicator &comm)
  {
    int res=comm.sendDoubles(ta,td,getDbTagData(),CommMetaData(1));
    res+= comm.sendVector(qd,getDbTagData(),CommMetaData(2));
    res+= comm.sendVector(q0,getDbTagData(),CommMetaData(3));
    res+= comm.sendVector(q0C,getDbTagData(),CommMetaData(4));
    res+= sendTimeSeriesPtr(fac,5,6,getDbTagData(),comm);
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::Damping::recvData(const Communicator &comm)
  {
    int res= comm.receiveDoubles(ta, td, getDbTagData(),CommMetaData(1));
    res+= comm.receiveVector(qd,getDbTagData(),CommMetaData(2));
    res+= comm.receiveVector(q0,getDbTagData(),CommMetaData(3));
    res+= comm.receiveVector(q0C,getDbTagData(),CommMetaData(4));
    fac= receiveTimeSeriesPtr(fac,5,6,getDbTagData(),comm);
    return res;
  }

const XC::Vector &XC::Damping::getDampingForce(void) const
  { return qd; }

int XC::Damping::setDomain(Domain *domain, int nComp)
  {       
    theDomain = domain;
  
    qd = Vector(nComp);
    q0 = Vector(nComp);
    q0C = Vector(nComp);
    return 0;
  }

int XC::Damping::commitState(void)
  {
    q0C= q0;
    return 0;
  }

int XC::Damping::revertToLastCommit(void)
  {
    q0= q0C;
    return 0;
  }


int XC::Damping::revertToStart(void)
  {
    q0.Zero();
    q0C.Zero();
    return 0;
  }
