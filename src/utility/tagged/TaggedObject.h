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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:02:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/tagged/TaggedObject.h,v $
                                                                        
                                                                        
#ifndef TaggedObject_h
#define TaggedObject_h

// File: ~/utility/tagged/TaggedObject.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for TaggedObject.
// A TaggedObject is an object with an integer identifier. It is used as
// a base class by DomainComponent, Graph and other classes in the framework.
//
// What: "@(#) TaggedObject.h, revA"

#include "utility/kernel/CommandEntity.h"

namespace XC {
class Domain;
class Vector;
class Matrix;
class FEM_ObjectBroker;

//! @ingroup Utils
//!
//! @defgroup Tagged Tagged object storage and convenience functions.
//
//! @ingroup Tagged
//
//! @brief Object idenfied by an integer (tag).
//!
//! TaggedObject is used as a base class to represent all classes
//! that may have a integer identifier, a tag, to identify the object. It
//! is used in the framework as a base class for many classes, for example
//! DomainComponent and Vertex. The class is provided so that container
//! classes can be written to store objects and provide access to
//! them. This saves us rewriting container classes for each type of 
//! object. (templates will be able to provide this functionality when
//! they are provided with all compilers). 
class TaggedObject: public CommandEntity
  {
  private:
    int theTag; //!< object identifier.
  protected:
    friend class ElementHandler;
    friend class Communicator;
    template <class T> friend T *getBrokedTagged(const int &,const int &,const int &,FEM_ObjectBroker &,T *(FEM_ObjectBroker::*ptrFunc)(int));
    void setTag(int newTag);  // CAUTION: this is a dangerous method to call

  public:
    TaggedObject(int tag,CommandEntity *owr= nullptr);
    virtual TaggedObject *getCopy(void) const;
    virtual ~TaggedObject(void) {}

    void assignTag(int newTag);
    int getTag(void) const;

    virtual void Print(std::ostream &, int flag =0) const;       
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);    
    friend std::ostream &operator<<(std::ostream &, const TaggedObject &);   
  };

std::ostream &operator<<(std::ostream &, const TaggedObject &);        

//! @brief Returns the tag associated with the object.
inline int TaggedObject::getTag(void) const
  { return theTag; }

} // end of XC namespace

#endif


