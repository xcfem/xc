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

// $Revision$
// $Date$
// $URL$

#ifndef VelDepMultiLinear_h
#define VelDepMultiLinear_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 04/12
// Revision: A
//
// Description: This file contains the class definition for the VelDepMultiLinear
// friction model. In the velocity dependent multi-linear friction model the
// friction force is given in terms of a multi-linear curve that is defined by
// a number of non-negative velocity/friction pairs. If the normal force N is
// negative the friction force is zero.

#include "Coulomb.h"
#include "utility/matrix/Vector.h"

namespace XC {
//! @ingroup FrictionModelGrp
//
class VelDepMultiLinear : public Coulomb
  {
  private:
    Vector velocityPoints; //!< velocity points on multi-linear curve
    Vector frictionPoints; //!< friction points on multi-linear curve
    int trialID; //!< trial ID into velocity, friction arrays
    int trialIDmin; //!< minimum of trial ID
    int trialIDmax; //!< maximum of trial ID
    
    double DmuDvel; //!< derivative of COF wrt to velocity
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructor
    VelDepMultiLinear(int tag= 0);
    VelDepMultiLinear(int tag,
		      const Vector &velocityPoints,
		      const Vector &frictionPoints);

    boost::python::list getVelocityFrictionPoints() const;
    void setVelocityFrictionPoints(const boost::python::list &);
    inline double getTrialID() const
      { return this->trialID; }
    void setTrialID(const double &d)
      { this->trialID= d; }
    inline double getTrialIDmin() const
      { return this->trialIDmin; }
    void setTrialIDmin(const double &d)
      { this->trialIDmin= d; }
    inline double getTrialIDmax() const
      { return this->trialIDmax; }
    void setTrialIDmax(const double &d)
      { this->trialIDmax= d; }
    
    // public methods to set and obtain response
    int setTrial(double normalForce, double velocity = 0.0);
    double getDFFrcDVel() const;
    
    int revertToStart();
    
    FrictionModel *getCopy() const;
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &, int flag = 0) const;

  };
} // end of XC namespace

#endif
