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
          index= i;
          i= numMatrices;
        }
  }

//! @brief Appends a new matrix to the container.
void XC::MeshComponent::new_matrix(std::deque<Matrix> &matrices,const int &ndof) const
  {
    const size_t numMatrices= matrices.size();
    matrices.push_back(Matrix(ndof,ndof));
    index= numMatrices;
  }

//! @brief Initializes the matrix container.
void XC::MeshComponent::setup_matrices(std::deque<Matrix> &matrices,const int &ndof) const
  {
    check_matrices(matrices,ndof);
    if(index == -1)
      new_matrix(matrices,ndof);
  }

//! @brief Constructor.
XC::MeshComponent::MeshComponent(int classTag)
  : ContinuaReprComponent(0,classTag), index(-1){}

//! @brief Constructor.
XC::MeshComponent::MeshComponent(int tag, int classTag)
  : ContinuaReprComponent(tag,classTag), index(-1){}

//! @brief Send labelsthrough the channel being passed as parameter.
int XC::MeshComponent::sendIdsLabels(int posDbTag,CommParameters &cp)
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
        res+= cp.sendID(labelIds,getDbTagData(),CommMetaData(posDbTag));
      }
    else
      setDbTagDataPos(posDbTag,0);
    return res;
  }

//! @brief Receive labels through the channel being passed as parameter.
int XC::MeshComponent::recvIdsLabels(int posDbTag,const CommParameters &cp)
  {
    int res= 0;
    static ID labelIds;
    if(getDbTagDataPos(posDbTag)!= 0)
      {
        const LabelDictionary &dic= labels.getDictionary();
        res= cp.receiveID(labelIds,getDbTagData(),CommMetaData(posDbTag));
    
        const size_t sz= labelIds.Size();
        for(size_t i=0;i<sz;i++)
          labels.addLabel(dic(labelIds[i]));
      }
    return res;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::MeshComponent::sendData(CommParameters &cp)
  {
    int res= ContinuaReprComponent::sendData(cp);
    cp.sendInt(index,getDbTagData(),CommMetaData(2));
    res+= sendIdsLabels(3,cp);
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::MeshComponent::recvData(const CommParameters &cp)
  {
    int res= ContinuaReprComponent::recvData(cp);
    cp.receiveInt(index,getDbTagData(),CommMetaData(2));
    res+= recvIdsLabels(3,cp);
    return res;
  }

