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
// $Date: 2003/02/14 23:01:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/utils/Information.cpp,v $
                                                                        
                                                                        
// File: ~/element/utils/Information.C
//
// Written: fmk 10/99
// Revised:
//
// Purpose: This file contains the class implementation for XC::Information.

#include <domain/mesh/element/utils/Information.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/nDarray/Tensor.h>
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "utility/actor/actor/TensorCommMetaData.h"

XC::Information::Information() 
  : MovableObject(0), theType(UnknownType),
   theID(nullptr), theVector(nullptr), theMatrix(nullptr), theTensor(nullptr) {}

XC::Information::Information(int val) 
  : MovableObject(0), theType(IntType), theInt(val),
  theID(nullptr), theVector(nullptr), theMatrix(nullptr), theTensor(nullptr) {}

XC::Information::Information(double val) 
  : MovableObject(0), theType(DoubleType), theDouble(val),
  theID(nullptr), theVector(nullptr), theMatrix(nullptr), theTensor(nullptr)
{
  // does nothing
}

XC::Information::Information(const XC::ID &val) 
  : MovableObject(0), theType(IdType),
    theID(nullptr), theVector(nullptr), theMatrix(nullptr), theTensor(nullptr)
  {
    // Make a copy
    theID = new ID(val);
    if(!theID)
      std::cerr << "XC::Information::Information -- failed to allocate\n";
  }

XC::Information::Information(const XC::Vector &val) 
  : MovableObject(0), theType(VectorType),
  theID(nullptr), theVector(nullptr), theMatrix(nullptr), theTensor(nullptr)
  {
    // Make a copy
    theVector = new Vector(val);
    if(!theVector)
      std::cerr << "Information::Information -- failed to allocate XC::Vector\n";
  }

XC::Information::Information(const XC::Matrix &val) 
  : MovableObject(0), theType(MatrixType),
    theID(nullptr), theVector(nullptr), theMatrix(nullptr), theTensor(nullptr)
  {
    // Make a copy
    theMatrix = new Matrix(val);
    if(!theMatrix)
      std::cerr << "XC::Information::Information -- failed to allocate XC::Matrix\n";
  }

XC::Information::Information(const Tensor &val) 
  : MovableObject(0), theType(TensorType),
    theID(nullptr), theVector(nullptr), theMatrix(nullptr), theTensor(nullptr)
  {
    // Make a copy
    theTensor= new Tensor(val);
    if(!theTensor)
      std::cerr << "XC::Information::Iformation -- failed to allocate Tensor\n";
  }

XC::Information::~Information() 
  {
    if(theID) delete theID;
    if(theVector) delete theVector;
    if(theMatrix) delete theMatrix;
    if(theTensor) delete theTensor;
  }

int XC::Information::setInt(int newInt)
  {
    theInt = newInt;
    return 0;
  }

int XC::Information::setDouble(double newDouble)
{
  theDouble = newDouble;
  
  return 0;
}

int XC::Information::setID(const XC::ID &newID)
{
  if(theID != 0) {
    *theID = newID;
    return 0;
  } else
    return -1;
}

int XC::Information::setVector(const XC::Vector &newVector)
{
  if(theVector != 0) {
    *theVector = newVector;
    return 0;
  }
  else
    return -1;
}

int XC::Information::setMatrix(const XC::Matrix &newMatrix)
{
  if(theMatrix != 0) {
    *theMatrix = newMatrix;
    return 0;
  }
  else
    return -1;
}

int XC::Information::setTensor(const Tensor &newTensor)
{
  if(theTensor != 0) {
    *theTensor = newTensor;
    return 0;
  }
  else
    return -1;
}

//! @brief Send object members through the channel being passed as parameter.
int XC::Information::sendData(CommParameters &cp)
  {
    //setDbTagDataPos(0,getTag());
    int IT= theType;
    int res= cp.sendInts(IT,theInt,getDbTagData(),CommMetaData(1));
    res+= cp.sendDouble(theDouble,getDbTagData(),CommMetaData(2));
    res+= cp.sendIDPtr(theID,getDbTagData(),ArrayCommMetaData(3,4,5));
    res+= cp.sendVectorPtr(theVector,getDbTagData(),ArrayCommMetaData(6,7,8));
    res+= cp.sendMatrixPtr(theMatrix,getDbTagData(),MatrixCommMetaData(9,10,11,12));
    res+= cp.sendTensorPtr(theTensor,getDbTagData(),TensorCommMetaData(13,14,15,16));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Information::recvData(const CommParameters &cp)
  {
    //setTag(getDbTagDataPos(0));
    int IT= theType;
    int res= cp.receiveInts(IT,theInt,getDbTagData(),CommMetaData(1));
    theType= InfoType(IT);
    res+= cp.receiveDouble(theDouble,getDbTagData(),CommMetaData(2));
    theID= cp.receiveIDPtr(theID,getDbTagData(),ArrayCommMetaData(3,4,5));
    theVector= cp.receiveVectorPtr(theVector,getDbTagData(),ArrayCommMetaData(6,7,8));
    theMatrix= cp.receiveMatrixPtr(theMatrix,getDbTagData(),MatrixCommMetaData(9,10,11,12));
    theTensor= cp.receiveTensorPtr(theTensor,getDbTagData(),TensorCommMetaData(13,14,15,16));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Information::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(11);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "Information::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Information::recvSelf(const CommParameters &cp)
  {
    inicComm(11);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "Information::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << "Information::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void 
XC::Information::Print(std::ostream &s, int flag)
{
  if(theType == IntType)
    s << theInt << " ";
  else if(theType == DoubleType)
    s << theDouble << " ";
  else if(theType == IdType && theID != 0)
    for(int i=0; i<theID->Size(); i++)
      s << (*theID)(i) << " ";
  else if(theType == VectorType && theVector != 0)
    for(int i=0; i<theVector->Size(); i++)
      s << (*theVector)(i) << " ";
  else if(theType == MatrixType && theMatrix != 0) {
    for(int i=0; i<theMatrix->noRows(); i++) {
      for(int j=0; j<theMatrix->noCols(); j++)
	s <<  (*theMatrix)(i,j) << " ";
	s << std::endl;
    }
  } else if(theType == TensorType && theTensor != 0)
    // No overloaded << for Tensors yet!
    //s << *theTensor;
    s << "No Tensor output";
  else
    return;
}


void XC::Information::Print(std::ofstream &s, int flag)
  {
    if(theType == IntType)
      s << theInt << " ";
    else if(theType == DoubleType)
      s << theDouble << " ";
    else if(theType == IdType && theID != 0)
      for(int i=0; i<theID->Size(); i++)
        s << (*theID)(i) << " ";
    else if(theType == VectorType && theVector != 0)
      for(int i=0; i<theVector->Size(); i++)
        s << (*theVector)(i) << " ";
    else if(theType == MatrixType && theMatrix != 0)
      {
        for(int i=0; i<theMatrix->noRows(); i++)
          {
            for(int j=0; j<theMatrix->noCols(); j++)
	      s <<  (*theMatrix)(i,j) << " ";
	    s << std::endl;
          }
      }
    else if(theType == TensorType && theTensor != 0)
      // No overloaded << for Tensors yet!
      //s << *theTensor;
      s << "No Tensor output";
    else
      return;
  }

const XC::Vector &
XC::Information::getData(void) 
{
  if(theType == IntType) {
    if(theVector == 0) 
      theVector = new Vector(1);
    (*theVector)(0) = theInt;
  } else if(theType == DoubleType) {
    if(theVector == 0) 
      theVector = new Vector(1);
    (*theVector)(0) = theDouble;
  } else if(theType == IdType && theID != 0) {
    if(theVector == 0) 
      theVector = new Vector(theID->Size());
    for(int i=0; i<theID->Size(); i++)
      (*theVector)(i) =  (*theID)(i);
  } else if(theType == MatrixType && theMatrix != 0) {
    int noRows = theMatrix->noRows();
    int noCols = theMatrix->noCols();
    if(theVector == 0) 
      theVector = new Vector(noRows * noCols);
    for(int i=0; i<noRows; i++)
      for(int j=0; j<noCols; j++)
	(*theVector)(i) = (*theMatrix)(i,j);
  }
  
  return *theVector;
}
