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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:15 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/actor/MovableObject.h,v $
                                                                        
                                                                        
#ifndef MovableObject_h
#define MovableObject_h

// File: ~/actor/MovableObject.h
//
// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for MovableObject.
// MovableObject is meant to be an abstract base class and thus no objects 
// of it's type can be instantiated. A movable object is an object which
// can send/receive itself to/from a Channel object.
//
// What: "@(#) MovableObject.h, revA"

#include "DistributedBase.h"
#include "classTags.h"
#include <string>
#include <vector>
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include <iostream>
#include "CommParameters.h"

namespace XC {
class Channel;
class FEM_ObjectBroker;
class Parameter;
class Information;
class CommParameters;
class CommMetaData;

//! @ingroup ACTOR
//
//! @brief Objeto que puede enviarse a otro proceso.
class MovableObject: public DistributedBase
  {
  private:
    int classTag;
    int dbTag;
  public:
    MovableObject(int classTag, int dbTag);        
    MovableObject(int classTag);   
    MovableObject(const MovableObject &otro);   
    MovableObject &operator=(const MovableObject &otro);   

    int getClassTag(void) const;
    int getDbTag(void) const;
    int getDbTag(CommParameters &);
    void setDbTag(int dbTag);
    void setDbTag(CommParameters &);

    virtual int sendSelf(CommParameters &) =0;
    virtual int recvSelf(const CommParameters &) =0;

    // methods for sensitivity studies
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);

    virtual int setVariable(const std::string &variable, Information &);
    virtual int getVariable(const std::string &variable, Information &);
  };


template <class T>
T *getBrokedMovable(const int &dbTag,const int &classTag,const CommParameters &cp,T *(FEM_ObjectBroker::*ptrFunc)(int))
  {
    T *retval=cp.getBrokedPtr(classTag,ptrFunc);
  
    if(retval)
      retval->setDbTag(dbTag);
    else
      std::cerr << "getBrokedMovable; no se pudo crear el objeto con classTag: " 
                << classTag << std::endl;
    return retval;
  }

} // end of XC namespace

#endif
