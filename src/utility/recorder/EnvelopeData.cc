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

#include <utility/recorder/EnvelopeData.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/actor/actor/MatrixCommMetaData.h"

//! @brief Constructor.
XC::EnvelopeData::EnvelopeData(void)
  :MovableObject(RECORDER_TAGS_EnvelopeData), data(nullptr), currentData(nullptr), first(true) {}

void XC::EnvelopeData::alloc(const size_t &numDbColumns)
  {
    free();
    data = new Matrix(3, numDbColumns);
    currentData = new Vector(numDbColumns);
    if(data == 0 || currentData == 0)
      {
        std::cerr << "EnvelopeData::alloc() - out of memory\n";
        exit(-1);
      }
    else
     data->Zero();
  }

void XC::EnvelopeData::free(void)
  {
    if(data) delete data;
    data= nullptr;
    if(currentData) delete currentData;
    currentData= nullptr;
  }

//! @brief Destructor.
XC::EnvelopeData::~EnvelopeData(void)
  { free(); }

int XC::EnvelopeData::restart(void)
  {
    data->Zero();
    first = true;
    return 0;
  }

//! @brief Send the object through the communicator
//! being passed as parameter.
int XC::EnvelopeData::sendData(Communicator &comm)
  {
    int res= comm.sendMatrixPtr(data,getDbTagData(),MatrixCommMetaData(0,1,2,3));
    res+= comm.sendVectorPtr(currentData,getDbTagData(),ArrayCommMetaData(4,5,6));
    setDbTagDataPos(7,first);
    return res;
  }

//! @brief Receive the object through the communicator
//! being passed as parameter.
int XC::EnvelopeData::receiveData(const Communicator &comm)
  {
    data= comm.receiveMatrixPtr(data,getDbTagData(),MatrixCommMetaData(0,1,2,3));
    currentData= comm.receiveVectorPtr(currentData,getDbTagData(),ArrayCommMetaData(4,5,6));
    first= getDbTagDataPos(7);
    return 0;
  }

  
int XC::EnvelopeData::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(comm);
    if(comm.sendIdData(getDbTagData(),dataTag)< 0)
      {
        std::cerr << "EnvelopeData::sendSelf() "
                  << "- failed to send data\n";
        return -1;
      }
    return res;
  }


int XC::EnvelopeData::recvSelf(const Communicator &comm)
  {
    inicComm(8);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "EnvelopeElementRecorder::recvSelf() - failed to recv data\n";
    else
      res= receiveData(comm);
    return res;
  }
