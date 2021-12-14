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

// $Revision: 1.1 $
// $Date: 2009/04/17 23:02:41 $
// $Source: element/special/frictionBearing/frictionModel/CoulombFriction.h,v $

#ifndef CoulombFriction_h
#define CoulombFriction_h         

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class definition for the CoulombFriction
// friction model. In the CoulombFriction model the friction force is given by
// mu*N, where mu is a constant coefficient of friction.
//
// What: "@(#) CoulombFriction.h, revA"

#include "FrictionModel.h"

namespace XC {
//! @ingroup FrictionModelGrp
//
//! @brief "Coulomb friction" model object. Coulomb's Law of Friction
//! states that kinetic friction is independent of the sliding velocity. 
class CoulombFriction: public FrictionModel
  {
  protected:
    double mu; //!< coefficient of friction

    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructor
    CoulombFriction(int classTag= FRN_TAG_CoulombFriction);
    CoulombFriction(int tag, double mu,int classTag= FRN_TAG_CoulombFriction);
    
    // public methods to set and obtain response
    int setTrial(double normalForce, double velocity = 0.0);
    double getFrictionForce(void);
    double getFrictionCoeff(void);
    double getDFFrcDNFrc(void);
    
    int commitState(void);
    int revertToLastCommit(void);
    
    FrictionModel *getCopy(void) const;
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag = 0) const;
};
} // end of XC namespace

#endif
