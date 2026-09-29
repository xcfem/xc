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
//MeshComponent.cc

#include "MeshComponent.h"
#include "utility/matrix/Matrix.h"


#include "utility/actor/actor/MovableID.h"


//! @brief Checks matrices number of rows.
void XC::MeshComponent::check_matrices(const std::deque<Matrix> &matrices,const int &ndof) const
  {
    const size_t numMatrices= matrices.size();
    for(size_t i=0; i<numMatrices; i++)
      if(matrices[i].noRows() == ndof)
        {
          this->matrixIndex= i;
          i= numMatrices;
        }
  }

//! @brief Appends a new matrix to the container.
void XC::MeshComponent::new_matrix(std::deque<Matrix> &matrices,const int &ndof) const
  {
    const size_t numMatrices= matrices.size();
    matrices.push_back(Matrix(ndof,ndof));
    this->matrixIndex= numMatrices;
  }

//! @brief Initializes the matrix container.
void XC::MeshComponent::setGlobalMatrices(std::deque<Matrix> &matrices,const int &ndof) const
  {
    if(this->matrixIndex == -1)
      check_matrices(matrices,ndof);
    if(this->matrixIndex == -1)
      new_matrix(matrices,ndof);
  }

//! @brief Constructor.
XC::MeshComponent::MeshComponent(int classTag)
  : ContinuaReprComponent(0,classTag), matrixIndex(-1) {}

//! @brief Constructor.
XC::MeshComponent::MeshComponent(int tag, int classTag)
  : ContinuaReprComponent(tag,classTag), matrixIndex(-1) {}

//! @brief Send labelsthrough the communicator argument.
int XC::MeshComponent::sendIdsLabels(int posDbTag,Communicator &comm)
  {
    int res= 0;
    static ID labelIds;
    const std::set<int> ids= labels.getIdsLabels();
    const size_t sz= ids.size();
    if(sz>0)
      {
        labelIds.resize(sz);
        int conta= 0;
        for(std::set<int>::const_iterator i= ids.begin();i!=ids.end();i++,conta++)
          labelIds[conta]= *i;
        res+= comm.sendID(labelIds,getDbTagData(),CommMetaData(posDbTag));
      }
    else
      setDbTagDataPos(posDbTag,0);
    return res;
  }

//! @brief Receive labels through the communicator argument.
int XC::MeshComponent::recvIdsLabels(int posDbTag,const Communicator &comm)
  {
    int res= 0;
    static ID labelIds;
    if(getDbTagDataPos(posDbTag)!= 0)
      {
        const LabelDictionary &dic= labels.getDictionary();
        res= comm.receiveID(labelIds,getDbTagData(),CommMetaData(posDbTag));
    
        const size_t sz= labelIds.Size();
        for(size_t i=0;i<sz;i++)
          labels.addLabel(dic(labelIds[i]));
      }
    return res;
  }

//! @brief Send members through the communicator argument.
int XC::MeshComponent::sendData(Communicator &comm)
  {
    int res= ContinuaReprComponent::sendData(comm);
    res+= comm.sendInt(matrixIndex, getDbTagData(),CommMetaData(2));
    res+= sendIdsLabels(3,comm);
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::MeshComponent::recvData(const Communicator &comm)
  {
    int res= ContinuaReprComponent::recvData(comm);
    res+= comm.receiveInt(matrixIndex, getDbTagData(),CommMetaData(2));
    res+= recvIdsLabels(3,comm);
    return res;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::MeshComponent::getPyDict(void) const
  {
    boost::python::dict retval= ContinuaReprComponent::getPyDict();
    retval["matrixIndex"]= matrixIndex;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::MeshComponent::setPyDict(const boost::python::dict &d)
  {
    ContinuaReprComponent::setPyDict(d);
    matrixIndex= boost::python::extract<int>(d["matrixIndex"]);
  }
