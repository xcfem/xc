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
//!
//! @defgroup IPComm Inter process communication.
//
//! @ingroup IPComm
//
//! @brief Object that can move between processes.
//!
//! Objects which are able to send/receive themselves through
//! Channel objects. With each movable object is associated a
//! unique class identifier, it is this id which
//! will allow object brokers in remote processes to create an object of
//! the correct type. In addition when databases are being used, each
//! MovableObject will have a unique database tag, it is this integer
//! which will allow the objects to retrieve their own data from the database.
class MovableObject: public DistributedBase
  {
  private:
    int classTag; //!< Identifier of the object class.
    int dbTag; //!< Object identifier in the database.
  public:
    MovableObject(int classTag, int dbTag);        
    MovableObject(int classTag);   
    MovableObject(const MovableObject &);   
    MovableObject &operator=(const MovableObject &);   

    int getClassTag(void) const;
    int getDbTag(void) const;
    int getDbTag(CommParameters &);
    void setDbTag(int dbTag);
    void setDbTag(CommParameters &);

    //! @brief Send the object.
    //!
    //! Each object has to send the data needed to be able to reproduce
    //! that object in a remote process. The object uses the methods
    //! provided by \p cp object to send the data to another channel
    //! at the remote actor, the address of the channel is set before
    //! this method is called.
    //! An object of similar type at the remote actor is invoked with a
    //! receiveSelf() to receive the data. Returns 0 if successful
    //! (successful in that the data got to the channel), or a - if no
    //! data was sent.
    virtual int sendSelf(CommParameters &cp) =0;
    
    //! @brief Receive the object.
    //!
    //! Each object has to receive the data needed to be able to
    //! recreate itself in the new process after it
    //! has been sent through \p cp. If the object is an
    //! aggregation containing other objects, new objects of the correct type
    //! can be constructed using #theBroker. To return 0 if successful
    //! or a -1 if not.
    virtual int recvSelf(const CommParameters &cp) =0;

    // methods for sensitivity studies
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);

    virtual int setVariable(const std::string &variable, Information &);
    virtual int getVariable(const std::string &variable, Information &);
  };


//! @brief Returns an empty object of the class identified by classTag.
template <class T>
T *getBrokedMovable(const int &dbTag,const int &classTag,const CommParameters &cp,T *(FEM_ObjectBroker::*ptrFunc)(int))
  {
    T *retval=cp.getBrokedPtr(classTag,ptrFunc);
  
    if(retval)
      retval->setDbTag(dbTag);
    else
      std::cerr << "getBrokedMovable; can't create object con classTag: " 
                << classTag << std::endl;
    return retval;
  }

} // end of XC namespace

#endif
