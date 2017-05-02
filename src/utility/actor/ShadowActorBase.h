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

#ifndef ShadowActorBase_h
#define ShadowActorBase_h

#include "ObjectWithObjBroker.h"

namespace XC {
class MachineBroker;
class Message;
class Channel;
class ChannelAddress;
class MovableObject;
class Matrix;
class Vector;
class ID;

//! @ingroup IPComm
//!
//! Base class for Actor and Shadow objects.
class ShadowActorBase: public ObjectWithObjBroker
  {
  private:
    int commitTag;
  protected:
    Channel *theChannel; //!< Communication channel to use.    
  public:
    ShadowActorBase(Channel &theChannel, FEM_ObjectBroker &);
    ShadowActorBase(int actorType, FEM_ObjectBroker &myBroker, MachineBroker &theMachineBroker, int compDemand);

    void setCommitTag(int commitTag);
    //! @brief Return the #commitTag member.
    inline int getCommitTag(void) const
      { return commitTag; }

    Channel *getChannelPtr(void) const;
    virtual int barrierCheck(int result);
  };
} // end of XC namespace

#endif
