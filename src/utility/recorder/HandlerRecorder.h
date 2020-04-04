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
//HandlerRecorder.h
                                                                        
#ifndef HandlerRecorder_h
#define HandlerRecorder_h

#include <utility/recorder/DomainRecorderBase.h>

namespace XC {
class Domain;
class DataOutputHandler;

//! @ingroup Recorder
//
//! @brief Base class for recorders that get
//! the response of one or more nodes during the analysis.
class HandlerRecorder: public DomainRecorderBase
  {
  protected:
    DataOutputHandler *theHandler; //!< Output handler (TO DEPRECATE).
    bool initializationDone;
    bool echoTimeFlag;   // flag indicating whether time to be included in o/p
  protected:
    int sendData(Communicator &);  
    int receiveData(const Communicator &);

  public:
    HandlerRecorder(int classTag);
    HandlerRecorder(int classTag, Domain &theDomain, DataOutputHandler &theOutputHandler,bool timeFlag);
    void SetOutputHandler(DataOutputHandler *tH);

  };
} // end of XC namespace

#endif
