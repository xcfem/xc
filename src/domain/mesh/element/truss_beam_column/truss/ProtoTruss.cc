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
//ProtoTruss.cc

#include "ProtoTruss.h"
#include <utility/matrix/Matrix.h>

#include "utility/actor/actor/MatrixCommMetaData.h"

// initialise the class wide variables
 XC::Matrix XC::ProtoTruss::trussM2(2,2);
 XC::Matrix XC::ProtoTruss::trussM3(3,3);
 XC::Matrix XC::ProtoTruss::trussM4(4,4);
 XC::Matrix XC::ProtoTruss::trussM6(6,6);
 XC::Matrix XC::ProtoTruss::trussM12(12,12);
 XC::Vector XC::ProtoTruss::trussV2(2);
 XC::Vector XC::ProtoTruss::trussV3(3);
 XC::Vector XC::ProtoTruss::trussV4(4);
 XC::Vector XC::ProtoTruss::trussV6(6);
 XC::Vector XC::ProtoTruss::trussV12(12);

//! Default constructor.
XC::ProtoTruss::ProtoTruss(int tag, int classTag,int Nd1,int Nd2,int ndof,int ndim)
  : Element1D(tag,classTag,Nd1,Nd2),numDOF(ndof),dimSpace(ndim), theMatrix(nullptr), theVector(nullptr)
  {}


//! @brief Copy constructor.
XC::ProtoTruss::ProtoTruss(const ProtoTruss &other)
  : Element1D(other),numDOF(other.numDOF),dimSpace(other.dimSpace),theMatrix(other.theMatrix), theVector(other.theVector)
  {}

//! @brief Assignment operator.
XC::ProtoTruss &XC::ProtoTruss::operator=(const ProtoTruss &other)
  {
    Element1D::operator=(other);
    numDOF= other.numDOF;
    dimSpace= other.dimSpace;
    theMatrix= other.theMatrix;
    theVector= other.theVector;
    return *this;
  }

//! @brief Returns the number of DOFs.
int XC::ProtoTruss::getNumDOF(void) const 
  { return numDOF; }

//! @brief Return the dimension of the space on which the element
//! is defined (2D or 3D).
int XC::ProtoTruss::getNumDIM(void) const 
  { return dimSpace; }

//! @brief Returns a reference to element's material.
XC::Material &XC::ProtoTruss::getMaterialRef(void)
  {
    Material *ptr= getMaterial();
    assert(ptr);
    return *ptr;
  }

//! @brief Set the number of dof for element and set matrix and vector pointers.
void XC::ProtoTruss::setup_matrix_vector_ptrs(int dofNd1)
  {
    const int numDim= getNumDIM();
    if(numDim == 1 && dofNd1 == 1)
      {
        numDOF = 2;
        theMatrix = &trussM2;
        theVector = &trussV2;
      }
    else if(numDim == 2 && dofNd1 == 2)
      {
        numDOF = 4;
        theMatrix = &trussM4;
        theVector = &trussV4;
      }
    else if(numDim == 2 && dofNd1 == 3)
      {
        numDOF = 6;
        theMatrix = &trussM6;
        theVector = &trussV6;
      }
    else if(numDim == 3 && dofNd1 == 3)
      {
        numDOF = 6;
        theMatrix = &trussM6;
        theVector = &trussV6;
      }
    else if(numDim == 3 && dofNd1 == 6)
      {
        numDOF = 12;
        theMatrix = &trussM12;
        theVector = &trussV12;
      }
    else
      {
	if(numDim==0)
           std::cerr << getClassName() << "::" << __FUNCTION__
                     << "; WARNING dimension of the element space is " << numDim
	             << ". Have you set the dimElem property of the element?\n";
	else
	   std::cerr << getClassName() << "::" << __FUNCTION__
	             << "; WARNING dimension of the element space is " << numDim
	             << " which is not compatible with a "
		     << dofNd1  << " DOFs problem.\n";

        // fill this in so don't segment fault later
        numDOF = 6;
        theMatrix = &trussM6;
        theVector = &trussV6;
        return;
      }
  }

//! @brief Send members through the communicator argument.
int XC::ProtoTruss::sendData(Communicator &comm)
  {
    int res= Element1D::sendData(comm);
    res+= comm.sendInts(numDOF,dimSpace,getDbTagData(),CommMetaData(7));
    res+= comm.sendMatrixPtr(theMatrix,getDbTagData(),MatrixCommMetaData(8,9,10,11)); 
    res+= comm.sendVectorPtr(theVector,getDbTagData(),ArrayCommMetaData(12,13,14)); 
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ProtoTruss::recvData(const Communicator &comm)
  {
    int res= Element1D::recvData(comm);
    res+= comm.receiveInts(numDOF,dimSpace,getDbTagData(),CommMetaData(7));
    theMatrix= comm.receiveMatrixPtr(theMatrix,getDbTagData(),MatrixCommMetaData(8,9,10,11)); 
    theVector= comm.receiveVectorPtr(theVector,getDbTagData(),ArrayCommMetaData(12,13,14)); 
    return res;
  }
