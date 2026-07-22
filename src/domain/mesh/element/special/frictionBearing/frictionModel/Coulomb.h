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
// $Source: element/special/frictionBearing/frictionModel/Coulomb.h,v $

#ifndef Coulomb_h
#define Coulomb_h         

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class definition for the Coulomb
// friction model. In the Coulomb model the friction force is given by
// mu*N, where mu is a constant coefficient of friction.
//
// What: "@(#) Coulomb.h, revA"

#include "FrictionModel.h"

namespace XC {
//! @ingroup FrictionModelGrp
//
//! @brief "Coulomb friction" model object. Coulomb's Law of Friction
//! states that kinetic friction is independent of the sliding velocity. 
class Coulomb: public FrictionModel
  {
  protected:
    double mu; //!< coefficient of friction

    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructor
    Coulomb(int tag= 0, int classTag= FRN_TAG_Coulomb);
    Coulomb(int tag, double mu,int classTag= FRN_TAG_Coulomb);
    
    // public methods to set and obtain response
    int setTrial(double normalForce, double velocity = 0.0);
    double getFrictionForce(void) const;
    double getFrictionCoeff(void) const;
    double getDFFrcDNFrc(void) const;
    double getDFFrcDVel() const;

    inline double getMu() const
      { return this->mu; }
    void setMu(const double &d)
      { this->mu= d; }
    
    int commitState(void);
    int revertToLastCommit(void);
    
    FrictionModel *getCopy(void) const;
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag = 0) const;
};
} // end of XC namespace

#endif
