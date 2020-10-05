// -*-c++-*-
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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/25 23:33:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/Material.h,v $
                                                                        
                                                                        
#ifndef Material_h
#define Material_h

// File: Material.h
//
// Written: fmk 
// Created: 05/98
// Revision: A
//
// Description: This file contains the class definition for Material.
// Material is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes. 
//
// What: "@(#) Material.h, revA"

#include "utility/tagged/TaggedObject.h"
#include "utility/actor/actor/MovableObject.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/BrokedPtrCommMetaData.h"

namespace XC {
class Information;
class Response;
class MaterialHandler;
class ID;

//!  @defgroup Mat Material models (constitutive equations).

//! @ingroup Mat
//
//! @brief Base class for materials.
//!
//! This class provides the interface that all subclasses must provide
//! when introducing new material types. A material object
//! is responsible for keeping track of stress, strain and the
//! state variables for a particular integration point (Gauss point,...)
//! in the domain. 
class Material: public TaggedObject, public MovableObject
  {
  public:
    Material(int tag, int classTag);

    const MaterialHandler *getMaterialHandler(void) const;
    MaterialHandler *getMaterialHandler(void);
    std::string getName(void) const;

    virtual int setVariable(const std::string &argv);
    virtual int getVariable(int variableID, double &info);

    // methods for sensitivity studies
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int responseID, Information &eleInformation);	

    virtual Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    virtual int getResponse(int responseID, Information &info);
    virtual Matrix getValues(const std::string &, bool silent= false) const;

    virtual void update(void);

    virtual const Vector &getGeneralizedStress(void) const= 0;
    virtual const Vector &getGeneralizedStrain(void) const= 0;
    virtual const Vector &getInitialGeneralizedStrain(void) const= 0;

    virtual void setInitialGeneralizedStrain(const Vector &)= 0;
    virtual void addInitialGeneralizedStrain(const Vector &);
    virtual void zeroInitialGeneralizedStrain(void);

    virtual int commitState(void) = 0;
    virtual int revertToLastCommit(void) = 0;
    virtual int revertToStart(void) = 0;

  };

int sendMaterialPtr(Material *,DbTagData &,Communicator &comm,const BrokedPtrCommMetaData &);
Material *receiveMaterialPtr(Material *,DbTagData &,const Communicator &comm,const BrokedPtrCommMetaData &);

} // end of XC namespace


#endif

