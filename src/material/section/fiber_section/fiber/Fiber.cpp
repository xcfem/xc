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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/25 23:33:36 $
// $Source: material/section/fiber_section/fiber/Fiber.cpp,v $
                                                                        
                                                                        
// File: ~/fiber/Fiber.C
//
// Written: Remo Magalhaes de Souza
// Created: 10/98
// Revision: 
//
// Description: This file contains the implementation for the
// Fiber class. Fiber provides the abstraction of a section fiber.
// 
// What: "@(#) Fiber.C, revA"


#include "Fiber.h"
#include "boost/any.hpp"
#include "material/uniaxial/UniaxialMaterial.h"

#include "utility/geom/pos_vec/Pos2d.h"

//! @brief Constructor.
XC::Fiber::Fiber(int tag, int classTag)
  : TaggedObject(tag), MovableObject(classTag), dead(false) {}

XC::Response *XC::Fiber::setResponse(const std::vector<std::string> &argv, Information &info)
  { return nullptr; }

int XC::Fiber::getResponse(int responseID, Information &info)
  { return -1; }

//! @brief Return true if the fiber material needs to update its internal
//! state even if the trial strains have not changed. This is the case
//! when the material deforms without load or under constant load, for
//! example by shrinkage or creep.
bool XC::Fiber::needsUpdate(void) const
  {
    bool retval= false;
    const UniaxialMaterial *mat= this->getMaterial();
    if(mat)
      retval= mat->needsUpdate();
    return retval;
  }

//! @brief Return the force response of the fiber.
double XC::Fiber::getForce(void) const
  { 
    double retval= 0.0;
    if(isAlive())
      retval= getArea()*getMaterial()->getStress();
    return retval;
  }

//! @brief Returns fiber strain.
double XC::Fiber::getStrain(void) const
  { return getMaterial()->getStrain(); }

//! @brief Returns fiber stress.
double XC::Fiber::getStress(void) const
  { 
    double retval= 0.0;
    if(isAlive())
      retval= getMaterial()->getStress();
    return retval;
  }

//! @brief Return the diameter of the circle that has the same area
//! of the fiber.
double XC::Fiber::getEquivalentDiameter(void) const
  { return 2.0*sqrt(this->getArea()/M_PI); }


//! @brief Returns fiber position.
Pos2d XC::Fiber::getPos(void) const
  { return Pos2d(getLocY(),getLocZ()); }

//! @brief Send data through the communicator argument.
int XC::Fiber::sendData(Communicator &comm)
  {
    setDbTagDataPos(0,getTag());
    setDbTagDataPos(1,dead);
    return 0;
  }

//! @brief Receive data through the communicator argument.
int XC::Fiber::recvData(const Communicator &comm)
  {    
    setTag(getDbTagDataPos(0));
    dead= static_cast<bool>(getDbTagDataPos(1));
    return 0;
  }

