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
// $Date: 2003/02/25 23:32:40 $
// $Source: domain/load/groundMotion/GroundMotionRecord.h,v $
                                                                        
                                                                        
// File: ~/earthquake/GroundMotionRecord.h
// 
// Written: MHS
// Created: 09/99
// Revision: A
//
// Description: This file contains the class definition for 
// a GroundMotionRecord, which creates a GroundMotion from
// user supplied data contained in an input file.
//
// What: "@(#) GroundMotionRecord.h, revA"

#ifndef GroundMotionRecord_h
#define GroundMotionRecord_h

#include "GroundMotion.h"
#include "MotionHistory.h"

namespace XC {
//! @ingroup GMotion
//
//! @brief Datos que definen un terremoto real.
class GroundMotionRecord : public GroundMotion
  {
  private:
    MotionHistory history;
  protected:

  public:
    GroundMotionRecord(void);
    GroundMotionRecord(const std::string &fileNameAccel, double timeStep, double fact = 1.0, double dTintegration = 0.01);
    GroundMotionRecord(const std::string &fileNameAccel, const std::string &fileNameTime, double fact = 1.0, double dTintegration = 0.01);
    // ADD CONSTRUCTORS FOR OTHER INPUT FORMATS
    virtual GroundMotion *getCopy(void) const;

    virtual double getDuration(void) const;

    virtual double getPeakAccel(void) const;
    virtual double getPeakVel(void) const;
    virtual double getPeakDisp(void) const;

    virtual double getAccel(double time) const;    
    virtual double getVel(double time) const;    
    virtual double getDisp(double time) const;

    inline MotionHistory &getHistory(void)
      { return history; }        
    
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
