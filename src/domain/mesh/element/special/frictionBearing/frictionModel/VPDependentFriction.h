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
// $Date: 2009/04/17 23:02:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/frictionModel/VPDependentFriction.h,v $

#ifndef VPDependentFriction_h
#define VPDependentFriction_h         

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class definition for the VPDependentFriction
// friction model after Constantinou et al. (1996). In the velocity and pressure
// dependent friction model the friction force is given in terms of the friction
// coefficients at low and high velocities with the latter one being a function of
// pressure.
//
// What: "@(#) VPDependentFriction.h, revA"

#include "VDependentFriction.h"

namespace XC {
//! @ingroup FrictionModelGrp
//
//! @brief Velocity an pressure dependent friction model.
//!
//! References:
//! - Tsopelas P., and Constantinou M. C. (1996). "Experimental Study of FPS System in Bridge Seismic Isolation." Earthquake Eng. and Structural Dynamics, VOL. 25, 65-78.
//! - Constantinou M. C., Tsopelas P., Kasalanati A., and Wolff E. D. (1999). Property Modification Factors for Seismic Isolation Bearings. Technical Report MCEER-99-0012, University of Buffalo, Buffalo, New York. 
class VPDependentFriction: public VDependentFriction
  {
  private:
    double A; //!< nominal contact area
    double deltaMu; //!< pressure parameter
    double alpha; //!< pressure parameter
    inline const double &muFast0(void) const
      { return muFast; }
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructor
    VPDependentFriction(void);
    VPDependentFriction(int tag, double muSlow, double muFast0, double A,
        double deltaMu, double alpha, double transRate);
    
    // public methods to set and obtain response
    int setTrial(double normalForce, double velocity = 0.0);
    double getDFFrcDNFrc(void);
    
    FrictionModel *getCopy(void) const;
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag = 0) const;
  };
} // end of XC namespace

#endif
