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
		   
//! \ingroup Elem
//
//! @brief Información relativa a un elemento.
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
} // fin namespace XC

#endif

