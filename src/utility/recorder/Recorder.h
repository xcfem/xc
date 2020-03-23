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
// $Date: 2004/11/24 22:41:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/Recorder.h,v $
                                                                        
                                                                        
#ifndef Recorder_h
#define Recorder_h

// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for Recorder.
// Recorder is an abstract base class. An Recorder object is used
// in the program to store/restore information at each commit().
//
// What: "@(#) Recorder.h, revA"

#include <utility/actor/actor/MovableObject.h>
#include "xc_utils/src/kernel/CommandEntity.h"

namespace XC {
class Domain;

//! @ingroup Utils
//!
//! @defgroup Recorder retrieving information at each commit.
//
//! @ingroup Recorder
//
//! @brief An Recorder object is used in the program to
//! store/restore information at each commit().
//!
//! The Recorder class is an abstract class which is introduced to allow
//! information to be saved during the analysis. The interface defines two
//! pure virtual methods record()} and {\em playback(). {\em
//! record()} is a method which is called by the Domain object during a
//! commit()}. The {\em playback() method can be called by the analyst after
//! the analysis has been performed.
class Recorder: public MovableObject, public CommandEntity
  {
  public:
    Recorder(int classTag);
    virtual ~Recorder(void) {}

    //! Invoked by the Domain object after commit() has been invoked on all the
    //! domain component objects. What the Recorder records depends on the
    //! concrete subtype.
    virtual int record(int commitTag, double timeStamp) =0;
    virtual int playback(int commitTag);
    virtual int restart(void);
    virtual int setDomain(Domain &theDomain);
    virtual int sendSelf(CommParameters &);  
    virtual int recvSelf(const CommParameters &);
  };
} // end of XC namespace


#endif

