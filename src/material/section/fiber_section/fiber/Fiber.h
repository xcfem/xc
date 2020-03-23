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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/25 23:33:36 $
// $Source: material/section/fiber_section/fiber/Fiber.h,v $
                                                                        
                                                                        
// File: ~/fiber/Fiber.h
//
// Written: Remo Magalhaes de Souza
// Created: 10/98
// Revision: 
//
// Description: This file contains the class definition for 
// Fiber. Fiber is an abstract base class and thus no objects of 
// it's type can be instatiated. It has pure virtual functions which
// must be implemented in it's derived classes.
//
// What: "@(#) Fiber.h, revA"


#ifndef Fiber_h
#define Fiber_h

#include "utility/tagged/TaggedObject.h"
#include "utility/actor/actor/MovableObject.h"

class Pos2d;

namespace XC {
class Vector;
class Matrix;
class ResponseId;
class UniaxialMaterial;
class Information;
class Response;

//! @ingroup MATSCCFiberModel
//!
//! @defgroup MATSCCFibers Classes for fiber section representation.
//
//! @ingroup MATSCCFibers
//
//! @brief Section fiber.
class Fiber: public TaggedObject, public MovableObject
  {
    bool dead; //!< True if fiber is inactive.
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    Fiber(int tag, int classTag);

    virtual int setTrialFiberStrain(const Vector &vs)=0;
    virtual Vector &getFiberStressResultants(void) =0;
    virtual Matrix &getFiberTangentStiffContr(void) =0;
	
    virtual int commitState(void)=0;
    virtual int revertToLastCommit(void)=0;    
    virtual int revertToStart(void)=0;
    
    virtual Fiber *getCopy(void) const= 0;
    virtual int getOrder(void) const= 0;
    virtual const ResponseId &getType(void) const= 0;

    virtual const bool isDead(void) const
      { return dead; }
    virtual const bool isAlive(void) const
      { return !dead; }
    virtual void kill(void)
      { dead= true; }
    virtual void alive(void)
      { dead= false; }

    virtual Response *setResponse(const std::vector<std::string> &argv, Information &info);
    virtual int getResponse(int responseID, Information &info);

    virtual void getFiberLocation(double &y, double &z) const=0;
    virtual double getLocY(void) const= 0;
    virtual double getLocZ(void) const
      { return 0.0; }
    Pos2d getPos(void) const;

    virtual UniaxialMaterial *getMaterial(void) =0;
    virtual const UniaxialMaterial *getMaterial(void) const= 0;
    //! @brief Return the area of the fiber.
    virtual double getArea(void) const=0;
    double getStrain(void) const;
    double getForce(void) const;
    double getMz(const double &y0= 0.0) const;
    double getMy(const double &z0= 0.0) const;
  };

//! @brief Returns the moment of the force of the fiber
//! with respect to the axis parallel to "z" that passes through (y0,0).
inline double Fiber::getMz(const double &y0) const
  { return getForce()*(getLocY() - y0); }

//! @brief Returns the moment of the force of the fiber
//! with respect to the axis parallel to "y" that passes through (0,z0).
inline double Fiber::getMy(const double &z0) const
  { return getForce()*(getLocZ() - z0); }

} // end of XC namespace


#endif

