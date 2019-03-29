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
//MovableMatrices.cc

#include "MovableMatrices.h"
#include "MovableID.h"
#include "MovableMatrix.h"
#include "utility/actor/channel/Channel.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "CommMetaData.h"

//! @brief Constructor.
XC::MovableMatrices::MovableMatrices(std::vector<Matrix> &v)
  : MovableObject(0), vectors(v) {}

//! @brief Envia el vector through the channel being passed as parameter.
int XC::MovableMatrices::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    const int sz= vectors.size();
    setDbTagDataPos(0,sz);

    int res= cp.sendMatrices(vectors,getDbTagData(),CommMetaData(1));

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "MovableMatrices::sendSelf() - failed to send ID.\n";
    return res;
  }

//! @brief Receive the vector through the channel being passed as parameter.
int XC::MovableMatrices::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag = getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "MovableMatrices::" << __FUNCTION__
	        << "; failed to receive ID data.\n";
    else
      res= cp.receiveMatrices(vectors,getDbTagData(),CommMetaData(1));
    if(res<0)
      {
        std::cerr << "MovableMatrices::" << __FUNCTION__
		  << "; failed to receive data.\n";
        return res;
      }
    return res;
  }

