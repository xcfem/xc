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
//ArcLengthBase.cpp


#include <solution/analysis/integrator/static/ArcLengthBase.h>
#include <utility/matrix/Vector.h>

#include "utility/actor/actor/CommMetaData.h"

//! @brief Constructor.
//!
//! @param owr: set of objects used to perform the analysis.
//! @param classTag: class identifier.
//! @param arcLength: value for the arc length.
//! @param alpha: scaling factor on the reference loads.
XC::ArcLengthBase::ArcLengthBase(SolutionStrategy *owr,int classTag,double arcLength, double alpha)
  :ProtoArcLength(owr,classTag,arcLength), alpha2(alpha*alpha) {}

//! @brief Returns the dLambda value for the newStep method.
double XC::ArcLengthBase::getDLambdaNewStep(void) const
  {
    const Vector &dUhat= vectors.getDeltaUhat();
    // determine delta lambda(1) == dlambda
    double retval = sqrt(arcLength2/((dUhat^dUhat)+alpha2));
    retval*= signLastDeltaLambdaStep; // base sign of load change
                                      // on what was happening last step
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::ArcLengthBase::sendData(Communicator &comm)
  {
    int res= ProtoArcLength::sendData(comm);
    res+= comm.sendDouble(alpha2,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ArcLengthBase::recvData(const Communicator &comm)
  {
    int res= ProtoArcLength::recvData(comm);
    res+= comm.receiveDouble(alpha2,getDbTagData(),CommMetaData(18));
    return res;
  }

int XC::ArcLengthBase::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }


int XC::ArcLengthBase::recvSelf(const Communicator &comm)
  {
    inicComm(19);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

