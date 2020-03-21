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
// $Source: /usr/local/cvs/OpenSees/SRC/utility/tagged/storage/ArrayOfTaggedObjects.h,v $


#ifndef ArrayOfTaggedObjects_h
#define ArrayOfTaggedObjects_h

// File: ~/utility/tagged/storage/ArrayOfTaggedObjects.h
//
// Written: fmk
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for
// ArrayOfTaggedObjects. 
//
// What: "@(#) ArrayOfTaggedObjects.h, revA"


#include <utility/tagged/storage/TaggedObjectStorage.h>
#include <utility/tagged/storage/ArrayOfTaggedObjectsIter.h>

namespace XC
  {
//! @ingroup Tagged
//
//! @brief ArrayOfTaggedObjects is a storage class. The class
//! is responsible for holding and providing access to objects of type
//! TaggedObject. The data structure used to hold the objects is a simple
//! array of pointers. As a one dimensional array is used certain ideas are
//! tried to improve performance: (1) if the array needs to be larger to hold
//! more components, the array size is doubled and (2) when adding/retrieving
//! components, the array location given by the components tag is first checked.
//!
//! ArrayOfTaggedObjects is used as a container object to store and
//! provide access to objects of type TaggedObject. A single one dimensional
//! array is used to store the pointers to the objects. As a one dimensional
//! array is used, certain ideas are tried to improve performance: (1) if
//! the array needs to be larger to hold more components, the array size
//! is doubled; (2) when adding/retrieving components, the array location
//! given by the components tag is first checked; and (3) a boolean flag is
//! used to keep track of whether all objects have been stored at the
//! location given by the tags.
class ArrayOfTaggedObjects: public TaggedObjectStorage
  {
    typedef std::vector<TaggedObject *> tagged_vector;
  public:
    typedef tagged_vector::iterator iterator;
    typedef tagged_vector::const_iterator const_iterator;
    typedef tagged_vector::reference reference;
    typedef tagged_vector::const_reference const_reference;
    typedef tagged_vector::value_type value_type;
  private:
    int numComponents; //!< num of components added
    int sizeComponentArray; //!< size of the array
    int positionLastEntry; //!< marker of last position used in the array
    int positionLastNoFitEntry; //!< marker of place array filled up to
    bool fitFlag; //!< flag indicating if all components in nicely

    tagged_vector theComponents; //!< the array
    ArrayOfTaggedObjectsIter  myIter; //!< an iter for accessing the objects
  protected:
    iterator begin(void)
      { return theComponents.begin(); }
    iterator end(void)
      { return theComponents.end(); }
    void clearComponents(void);

  public:
    ArrayOfTaggedObjects(CommandEntity *owr,int size,const std::string &containerName);
    ArrayOfTaggedObjects(const ArrayOfTaggedObjects &);
    ArrayOfTaggedObjects &operator=(const ArrayOfTaggedObjects &);
    ~ArrayOfTaggedObjects(void);

    // public methods to populate a domain
    int setSize(int newSize);
    bool addComponent(TaggedObject *newComponent);
    bool removeComponent(int tag);
    int getNumComponents(void) const;

    TaggedObject *getComponentPtr(int tag);
    const TaggedObject *getComponentPtr(int tag) const;
    TaggedObjectIter &getComponents(void);

    ArrayOfTaggedObjectsIter getIter(void);

    const_iterator begin(void) const
      { return theComponents.begin(); }
    const_iterator end(void) const
      { return theComponents.end(); }

    virtual TaggedObjectStorage *getEmptyCopy(void);
    virtual void clearAll(bool invokeDestructor = true);

    void Print(std::ostream &s, int flag =0) const;
    friend class ArrayOfTaggedObjectsIter;
  };

} // end of XC namespace

#endif


