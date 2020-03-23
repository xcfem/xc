// -*-c++-*-
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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/03/04 00:48:11 $
// $Source: domain/load/groundMotion/GroundMotion.h,v $
                                                                        
                                                                        
// File: ~/earthquake/GroundMotion.h
// 
// Written: fmk 
// Created: 05/98
// Revision: A
//
// Description: This file contains the class definition for GroundMotion.
// GroundMotion is an abstract base class, i.e. no objects of it's
// type can be created. A GroundMotion describes the motion of the ground
// in a dynamic analysis.
//
// What: "@(#) GroundMotion.h, revA"

#ifndef GroundMotion_h
#define GroundMotion_h

#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"

namespace XC {
  class TimeSeries;
  class Information;

//! @ingroup Loads
//!
//!
//! @defgroup GMotions Ground motions (earthquake simulation).
//
//! @ingroup GMotion
//
//! @brief Base class for ground motions.
class GroundMotion: public MovableObject, public CommandEntity
  {
  protected:
    mutable Vector data;
  public:
    GroundMotion(int classTag);

    virtual ~GroundMotion(void)
      {}

    virtual GroundMotion *getCopy(void) const= 0;

    virtual double getDuration(void) const= 0;

    virtual double getPeakAccel() const= 0;
    virtual double getPeakVel() const= 0;
    virtual double getPeakDisp() const= 0;

    virtual double getAccel(double time) const= 0;
    virtual double getVel(double time) const= 0;
    virtual double getDisp(double time) const= 0;
    virtual const  Vector &getDispVelAccel(double time) const;
    
/*     void setIntegrator(TimeSeriesIntegrator *integrator); */
/*     TimeSeries *integrate(TimeSeries *theSeries, double delta = 0.01);  */

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    // AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual double getAccelSensitivity(double time)= 0;
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param)= 0;
    virtual int updateParameter(int parameterID, Information &info)= 0;
    virtual int activateParameter(int parameterID)= 0;
    // AddingSensitivity:END ///////////////////////////////////////////

  };

int sendGroundMotionPtr(GroundMotion *,DbTagData &,CommParameters &cp,const BrokedPtrCommMetaData &);
GroundMotion *receiveGroundMotionPtr(GroundMotion *,DbTagData &,const CommParameters &cp,const BrokedPtrCommMetaData &);


} // end of XC namespace

#endif

