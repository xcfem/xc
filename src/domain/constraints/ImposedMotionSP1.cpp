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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/22 19:41:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/ImposedMotionSP1.cpp,v $
                                                                        
// Written: fmk 
// Created: 11/00
// Revision: A
//
// Purpose: This file contains the implementation of class XC::ImposedMotionSP1.

#include <domain/constraints/ImposedMotionSP1.h>
#include <classTags.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/load/groundMotion/GroundMotion.h>
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include <domain/load/pattern/LoadPattern.h>
#include <utility/matrix/ID.h>

//! @brief constructor for FEM_ObjectBroker
XC::ImposedMotionSP1::ImposedMotionSP1(void)
  :ImposedMotionBase(CNSTRNT_TAG_ImposedMotionSP1),destroyMotion(0) {}

//! @brief constructor for a subclass to use
XC::ImposedMotionSP1::ImposedMotionSP1(int tag, int node, int ndof, int pattern, int motion)
 :ImposedMotionBase(CNSTRNT_TAG_ImposedMotionSP1,tag, node, ndof,pattern,motion) {}

//! @brief Returns true if it's an homogeneous constraint
//! (prescribed value for the DOF is zero).
bool XC::ImposedMotionSP1::isHomogeneous(void) const
  { return false; }

//! @brief Applies constraint at the pseudo-time being passed
//! as parameter.
int XC::ImposedMotionSP1::applyConstraint(double time)
  {
    // on first 
    if(theGroundMotion == 0 || theNode == 0)
      {
        int retval= getMotion();
        if(retval!=0)
          { return retval; }
      }

    // now get the response from the ground motion
    theGroundMotionResponse = theGroundMotion->getDispVelAccel(time);

    return 0;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::ImposedMotionSP1::sendData(CommParameters &cp)
  {
    int result= ImposedMotionBase::sendData(cp);
    result+= cp.sendInt(destroyMotion,getDbTagData(),CommMetaData(9));
    return result;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::ImposedMotionSP1::recvData(const CommParameters &cp)
  {
    int res= ImposedMotionBase::recvData(cp);
    res+= cp.receiveInt(destroyMotion,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::ImposedMotionSP1::sendSelf(CommParameters &cp)
  {
    static ID data(10);
    int result= sendData(cp);
    const int dbTag = this->getDbTag();
    result+= cp.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "ImposedMotionSP1::sendSelf() - failed to send extra data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ImposedMotionSP1::recvSelf(const CommParameters &cp)
  {
    static ID data(10);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ImposedMotionSP1::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Printing.
void XC::ImposedMotionSP1::Print(std::ostream &s, int flag) 
  {
    s << "ImposedMotionSP1: " << this->getTag();
    s << "\t XC::Node: " << this->getNodeTag();
    s << " DOF: " << this->getDOF_Number() << std::endl;    
  }
