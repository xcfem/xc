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
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/frictionModel/VDependentFriction.h,v $

#ifndef VDependentFriction_h
#define VDependentFriction_h         

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class definition for the VDependentFriction
// friction model after Constantinou et al. (1990). In the velocity dependent
// friction model the friction force is given in terms of the friction coefficients
// at low and high velocities and a constant describing the rate of transition.
//
// What: "@(#) VDependentFriction.h, revA"

#include "CoulombFriction.h"

namespace XC {
//! @ingroup FrictionModelGrp
//
//! @brief Velocity dependent friction model. It is useful for modeling
//! the behavior of <a href="https://en.wikipedia.org/wiki/Polytetrafluoroethylene">"PTFE"</a> or PTFE-like materials sliding on a stainless
//! steel surface. For a detailed presentation on the velocity dependence
//! of such interfaces please refer to Constantinou et al. (1999).
//! <a href="https://www.researchgate.net/publication/276269723_Property_modification_factors_seismic_isolation_bearings">Property modification factors seismic isolation bearings.</a>
class VDependentFriction: public CoulombFriction
  {
  protected:
    double muSlow; //!< coefficient of friction at low velocity
    double muFast; //!< coefficient of friction at high velocity
    double transRate; //!< transition rate from low to high velocity
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    // constructor
    VDependentFriction(int classTag= FRN_TAG_VDependentFriction);
    VDependentFriction(int tag, double muSlow, double muFast, double transRate,int classTag= FRN_TAG_VDependentFriction);
    
    // public methods to set and obtain response
    int setTrial(double normalForce, double velocity = 0.0);
    double getDFFrcDNFrc(void);
    
    int revertToStart(void);
    
    FrictionModel *getCopy(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &, int flag = 0) const;
  };
} // end of XC namespace

#endif
