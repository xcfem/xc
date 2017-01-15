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
// $Date: 2006/01/04 21:59:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/MultiSupportPattern.cpp,v $
                                                                        
// Written: fmk 11/00
// Revised:
//
// Purpose: This file contains the class definition for XC::MultiSupportPattern.
// MultiSupportPattern is an abstract class.

#include <domain/load/pattern/load_patterns/MultiSupportPattern.h>
#include <domain/load/groundMotion/GroundMotion.h>

#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/domain/Domain.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <domain/constraints/SFreedom_ConstraintIter.h>
#include <cstdlib>
#include <utility/handler/ErrorHandler.h>

XC::MultiSupportPattern::MultiSupportPattern(int tag, int _classTag)
  :EQBasePattern(tag, _classTag), theMotionTags(0,16), dbMotions(0)
  {}


XC::MultiSupportPattern::MultiSupportPattern(int tag)
  :EQBasePattern(tag, PATTERN_TAG_MultiSupportPattern), theMotionTags(0,16), dbMotions(0)
  {}


XC::MultiSupportPattern::MultiSupportPattern(void)
  :EQBasePattern(0, PATTERN_TAG_MultiSupportPattern), theMotionTags(0,16), dbMotions(0)
  {}


void  XC::MultiSupportPattern::applyLoad(double time)
  {
    SFreedom_Constraint *sp;
    SFreedom_ConstraintIter &theIter = this->getSPs();
    while ((sp = theIter()) != 0)
      sp->applyConstraint(time);
  }
    

int XC::MultiSupportPattern::addMotion(GroundMotion &theMotion, int tag)
  {
    // ensure no motion with given tag already added
    if(theMotionTags.getLocation(tag) >= 0)
      {
        std::cerr << "XC::MultiSupportPattern::addMotion - could not add new, motion wih same tag exists\n";
        return -1;
      }

    theMotions.addMotion(theMotion);
    return 0;
  }



XC::GroundMotion *XC::MultiSupportPattern::getMotion(int tag)
  {
    int loc = theMotionTags.getLocation(tag);
    if(loc < 0)
      return nullptr;
    else
      return theMotions[loc];
  }


bool XC::MultiSupportPattern::addNodalLoad(NodalLoad *)
  {
    std::cerr << "XC::MultiSupportPattern::addNodalLoad() - cannot add XC::NodalLoad to EQ pattern\n";  
    return false;
  }

bool XC::MultiSupportPattern::addElementalLoad(ElementalLoad *)
  {
    std::cerr << "XC::MultiSupportPattern::addElementalLoad() - cannot add XC::ElementalLoad to EQ pattern\n";    
    return false;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::MultiSupportPattern::sendData(CommParameters &cp)
  {
    int res= EQBasePattern::sendData(cp);
    res+= cp.sendID(theMotionTags,getDbTagData(),CommMetaData(17));
    res+= cp.sendInt(dbMotions,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::MultiSupportPattern::recvData(const CommParameters &cp)
  {
    int res= EQBasePattern::recvData(cp);
    res+= cp.receiveID(theMotionTags,getDbTagData(),CommMetaData(17));
    res+= cp.receiveInt(dbMotions,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::MultiSupportPattern::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::MultiSupportPattern::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::MultiSupportPattern::Print(std::ostream &s, int flag)
  {
    s << "MultiSupportPattern  tag: " << this->getTag() << std::endl;
    SFreedom_Constraint *sp;
    SFreedom_ConstraintIter &theIter = this->getSPs();
    while ((sp = theIter()) != 0)
      sp->Print(s, flag);
  }
