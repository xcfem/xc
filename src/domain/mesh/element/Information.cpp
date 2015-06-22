//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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
// $Source: /usr/local/cvs/OpenSees/SRC/element/Information.cpp,v $
                                                                        
                                                                        
// File: ~/element/Information.C
//
// Written: fmk 10/99
// Revised:
//
// Purpose: This file contains the class implementation for XC::Information.

#include <domain/mesh/element/Information.h>
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

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
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

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
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

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
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

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
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
      theVector = new XC::Vector(1);
    (*theVector)(0) = theInt;
  } else if(theType == DoubleType) {
    if(theVector == 0) 
      theVector = new XC::Vector(1);
    (*theVector)(0) = theDouble;
  } else if(theType == IdType && theID != 0) {
    if(theVector == 0) 
      theVector = new XC::Vector(theID->Size());
    for(int i=0; i<theID->Size(); i++)
      (*theVector)(i) =  (*theID)(i);
  } else if(theType == MatrixType && theMatrix != 0) {
    int noRows = theMatrix->noRows();
    int noCols = theMatrix->noCols();
    if(theVector == 0) 
      theVector = new XC::Vector(noRows * noCols);
    for(int i=0; i<noRows; i++)
      for(int j=0; j<noCols; j++)
	(*theVector)(i) = (*theMatrix)(i,j);
  }
  
  return *theVector;
}
