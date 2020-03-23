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
                                                                        
// $Revision: 1.1 $
// $Date: 2000/12/12 07:32:38 $
// $Source: domain/load/groundMotion/InterpolatedGroundMotion.h,v $
                                                                        
// File: ~/earthquake/InterpolatedGroundMotion.h
// 
// Written: fmk
// Created: 11/00
// Revision: A
//
// Description: This file contains the class definition for 
// an InterpolatedGroundMotion, which inetrploates between 2
// motions to determine a motion.
//
// What: "@(#) InterpolatedGroundMotion.h, revA"

#ifndef InterpolatedGroundMotion_h
#define InterpolatedGroundMotion_h

#include "GroundMotion.h"
#include "MotionHistory.h"
#include <vector>

namespace XC {
  class Vector;

//! @ingroup GMotion
//
//! @brief ??.
class InterpolatedGroundMotion: public GroundMotion
  {
  public:
    typedef std::vector<MotionHistory> vector_motions;
  private:
    vector_motions theMotions;
    Vector *factors;
    
    double deltaPeak;  // increment for determining a peak response
    void free_mem(void);
    void copy(const Vector *);
  public:
    InterpolatedGroundMotion(void);
    InterpolatedGroundMotion(const InterpolatedGroundMotion &);
    InterpolatedGroundMotion(const vector_motions &groundMotions, const Vector &fact, double deltaPeak = 0.01);

    virtual ~InterpolatedGroundMotion(void);
    InterpolatedGroundMotion &operator=(const InterpolatedGroundMotion &);
    virtual GroundMotion *getCopy(void) const;

    virtual double getDuration(void) const;

    virtual double getPeakAccel(void) const;
    virtual double getPeakVel(void) const;
    virtual double getPeakDisp(void) const;

    virtual double getAccel(double time) const;
    virtual double getVel(double time) const;
    virtual double getDisp(double time) const;
    virtual const  Vector &getDispVelAccel(double time) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual double getAccelSensitivity(double time);
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);
    // AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace

#endif
