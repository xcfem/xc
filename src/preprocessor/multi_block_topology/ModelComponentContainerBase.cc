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
//ModelComponentContainerBase.cc

#include "ModelComponentContainerBase.h"

#include "preprocessor/multi_block_topology/MultiBlockTopology.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::ModelComponentContainerBase::ModelComponentContainerBase(MultiBlockTopology *mbt)
  : CommandEntity(mbt), MovableObject(0), tag(0) {}

//! @brief Return a pointer to MultiBlockTopology.
const XC::MultiBlockTopology *XC::ModelComponentContainerBase::getMultiBlockTopology(void) const
  {
    const MultiBlockTopology *retval= dynamic_cast<const MultiBlockTopology *>(Owner());
    return retval;
  }

//! @brief Return a pointer to MultiBlockTopology.
XC::MultiBlockTopology *XC::ModelComponentContainerBase::getMultiBlockTopology(void)
  { 
    MultiBlockTopology *retval= const_cast<MultiBlockTopology *>(dynamic_cast<const MultiBlockTopology *>(Owner()));
    return retval;
  }

//! @brief Return a pointer to preprocessor.
const XC::Preprocessor *XC::ModelComponentContainerBase::getPreprocessor(void) const
  { return getMultiBlockTopology()->getPreprocessor(); }

//! @brief Return a pointer to preprocessor.
XC::Preprocessor *XC::ModelComponentContainerBase::getPreprocessor(void)
  { return getMultiBlockTopology()->getPreprocessor(); }

//! @brief Send data through the communicator argument.
int XC::ModelComponentContainerBase::sendData(Communicator &comm)
  {
    int res= comm.sendSzt(tag,getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::ModelComponentContainerBase::recvData(const Communicator &comm)
  {
    int res= comm.receiveSzt(tag,getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Send object through the communicator argument.
int XC::ModelComponentContainerBase::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(getDbTagData().Size());
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receive object through the communicator argument.
int XC::ModelComponentContainerBase::recvSelf(const Communicator &comm)
  {
    inicComm(getDbTagData().Size());
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
