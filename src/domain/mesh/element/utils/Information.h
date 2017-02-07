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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/25 23:32:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/Information.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 10/99
// Revision: A
//
// Description: This file contains the class definition for Information.
// Information is a class in which all data members are public, i.e. basically
// a struct.
//
// What: "@(#) Information.h, revA"

#ifndef Information_h
#define Information_h

#include "utility/actor/actor/MovableObject.h"
#include <fstream>

namespace XC {
class Matrix;
class Vector;
class BJtensor;
class ID;

enum InfoType {UnknownType, IntType, DoubleType, 
	       IdType, VectorType, MatrixType, TensorType};
		   
//! \ingroup FEMisc
//
//! @brief Information about an element.
class Information: public MovableObject
  {
  public:
    // data that is stored in the information object
    InfoType	theType;   // information about data type
    int		theInt;    // an integer value
    double	theDouble; // a double value
    ID		*theID;    // pointer to an ID object, created elsewhere
    Vector 	*theVector;// pointer to a Vector object, created elsewhere
    Matrix	*theMatrix;// pointer to a Matrix object, created elsewhere
    BJtensor    *theTensor;// pointer to a Tensor object, created elsewhere

    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    Information(void);
    Information(int val);
    Information(double val);
    Information(const ID &val);
    Information(const Vector &val);
    Information(const Matrix &val);
    Information(const BJtensor &val);
    
    virtual ~Information();
    
    virtual int setInt(int newInt);
    virtual int setDouble(double newDouble);
    virtual int setID(const ID &newID);
    virtual int setVector(const Vector &newVector);
    virtual int setMatrix(const Matrix &newMatrix);
    virtual int setTensor(const BJtensor &newTensor);
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    virtual void Print(std::ostream &s, int flag = 0);
    virtual void Print(std::ofstream &s, int flag = 0);
    virtual const Vector &getData(void);
  };
} // end of XC namespace

#endif

