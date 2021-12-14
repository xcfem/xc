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
                                                                        
// $Revision: 1.5 $
// $Date: 2004/11/25 00:53:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/DatastoreRecorder.cpp,v $
                                                                        
                                                                        
// File: ~/DatastoreRecorder/DatastoreRecorder.h
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for XC::DatastoreRecorder.
// A XC::DatastoreRecorder object is used in the program to store/restore the
// Domain information in a XC::FE_Datastore object.
//
// What: "@(#) DatastoreRecorder.h, revA"

#include <utility/recorder/DatastoreRecorder.h>
#include <domain/domain/Domain.h>
#include <utility/database/FE_Datastore.h>

//! @brief Default constructor.
XC::DatastoreRecorder::DatastoreRecorder(void)
  :Recorder(RECORDER_TAGS_DatastoreRecorder), theDatastore(nullptr) {}

//! @brief Constructor.
//!
//! @param theDb: reference to the datastore object.
XC::DatastoreRecorder::DatastoreRecorder(FE_Datastore &theDb)
  :Recorder(RECORDER_TAGS_DatastoreRecorder), theDatastore(&theDb) {}


//! @brief Return the result of invoking {\em commitState(commitTag)} on {\em
//! theDatastore} object. 
int XC::DatastoreRecorder::record(int commitTag, double timeStamp)
  { return theDatastore->commitState(commitTag); }


//! brief Return the result of invoking {\em restoreState(commitTag)} on {\em
//! theDatastore} object. 
int XC::DatastoreRecorder::playback(int commitTag)
  { return theDatastore->restoreState(commitTag); }

//! @brief Do nothing.
int XC::DatastoreRecorder::restart(void)
  { return 0; }
