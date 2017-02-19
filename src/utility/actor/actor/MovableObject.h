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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:15 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/actor/MovableObject.h,v $
                                                                        
                                                                        
#ifndef MovableObject_h
#define MovableObject_h

// File: ~/actor/MovableObject.h
//
// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for MovableObject.
// MovableObject is meant to be an abstract base class and thus no objects 
// of it's type can be instantiated. A movable object is an object which
// can send/receive itself to/from a Channel object.
//
// What: "@(#) MovableObject.h, revA"

#include "DistributedBase.h"
#include "classTags.h"
#include <string>
#include <vector>
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include <iostream>
#include "CommParameters.h"

namespace XC {
class Channel;
class FEM_ObjectBroker;
class Parameter;
class Information;
class CommParameters;
class CommMetaData;

//! @ingroup Utils
//
//! @defgroup IPComm Inter process communication.
//
//! @ingroup IPComm
//
//! @brief Object that can move between processes.
class MovableObject: public DistributedBase
  {
  private:
    int classTag;
    int dbTag;
  public:
    MovableObject(int classTag, int dbTag);        
    MovableObject(int classTag);   
    MovableObject(const MovableObject &otro);   
    MovableObject &operator=(const MovableObject &otro);   

    int getClassTag(void) const;
    int getDbTag(void) const;
    int getDbTag(CommParameters &);
    void setDbTag(int dbTag);
    void setDbTag(CommParameters &);

    virtual int sendSelf(CommParameters &) =0;
    virtual int recvSelf(const CommParameters &) =0;

    // methods for sensitivity studies
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);

    virtual int setVariable(const std::string &variable, Information &);
    virtual int getVariable(const std::string &variable, Information &);
  };


template <class T>
T *getBrokedMovable(const int &dbTag,const int &classTag,const CommParameters &cp,T *(FEM_ObjectBroker::*ptrFunc)(int))
  {
    T *retval=cp.getBrokedPtr(classTag,ptrFunc);
  
    if(retval)
      retval->setDbTag(dbTag);
    else
      std::cerr << "getBrokedMovable; no se pudo crear el objeto con classTag: " 
                << classTag << std::endl;
    return retval;
  }

} // end of XC namespace

#endif
