// -*-c++-*-
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
//ElementRecorderBase.h
                                                                        
                                                                        
#ifndef ElementRecorderBase_h
#define ElementRecorderBase_h


// What: "@(#) ElementRecorderBase.h, revA"

#include <utility/recorder/MeshCompRecorder.h>
#include <utility/matrix/ID.h>

namespace XC {
class Response;
class DataOutputHandler;

//! @ingroup Recorder
//
//! @brief Base class for the finite element response recorders.
class ElementRecorderBase: public MeshCompRecorder
  {
  protected:        
    ID eleID;

    std::vector<Response *> theResponses;
    std::vector<std::string> responseArgs;

    int sendData(Communicator &);  
    int receiveData(const Communicator &);
    void setup_responses(const std::string &);

  public:
    ElementRecorderBase(int classTag);
    ElementRecorderBase(int classTag, const ID &eleID, 
                        const std::vector<std::string> &, 
                        bool echoTime, 
                        Domain &theDomain, 
                        DataOutputHandler &theOutputHandler,
                        double deltaT = 0.0);
    ~ElementRecorderBase(void);
    inline size_t getNumArgs(void) const
      { return responseArgs.size(); }
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
  };
} // end of XC namespace


#endif
