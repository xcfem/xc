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
                                                                        
// $Revision: 1.4 $
// $Date: 2004/11/24 22:45:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/DatastoreRecorder.h,v $
                                                                        
                                                                        
// File: ~/DatastoreRecorder/DatastoreRecorder.h
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for DatastoreRecorder.
// A DatastoreRecorder object is used in the program to invoke commitState()/
// on an FE\_datastore object when commit() is invoked on a Domain. This means 
// we do not have to add additional methods in the Domain for FE_datastore objects.
//
// What: "@(#) DatastoreRecorder.h, revA"

#ifndef DatastoreRecorder_h
#define DatastoreRecorder_h

#include <utility/recorder/Recorder.h>

namespace XC {
class Domain;
class FE_Datastore;

//! @ingroup Recorder
//!
//! @brief A DatastoreRecorder object is used in the program to invoke
//! commitState() on an FE\_datastore object when commit() is invoked on
//! a Domain. 
class DatastoreRecorder: public Recorder
  {
  private:	
    FE_Datastore *theDatastore; //!< Link with the datastore object.
  public:
    DatastoreRecorder(void);
    DatastoreRecorder(FE_Datastore &theDatastore);

    int record(int commitTag, double timeStamp);
    int playback(int commitTag);
    int restart(void);
  };
} // end of XC namespace


#endif

