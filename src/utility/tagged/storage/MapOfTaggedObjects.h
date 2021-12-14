// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
// $Source: /usr/local/cvs/OpenSees/SRC/utility/tagged/storage/MapOfTaggedObjects.h,v $
                                                                        
                                                                        
#ifndef MapOfTaggedObjects_h
#define MapOfTaggedObjects_h

// File: ~/utility/tagged/storage/MapOfTaggedObjects.h
// 
// Written: fmk 
// Created: 02/00
// Revision: A
//
// Description: This file contains the class definition for 
// MapOfTaggedObjects. MapOfTaggedObjects is a storage class. 
//
// What: "@(#) MapOfTaggedObjects.h, revA"


#include <utility/tagged/storage/TaggedObjectStorage.h>
#include <utility/tagged/storage/MapOfTaggedObjectsIter.h>

namespace XC {
//! @ingroup Tagged
//
//! @brief The class is responsible for holding and providing access
//! to objects of type TaggedObject. A map template of the standard
//! template class is used to store the pointers to these objects.
//!
//! A MapOfTaggedObjects object is used as a container to store and
//! provide access to objects of type TaggedObject. A MapOfTaggedObjects
//! creates a map object to store the pointers to these objects. A map is
//! created using a template provided by the standard template
//! library. The key used to identify the pointers stored in the map
//! object is the TaggedObjects tag. Each MapOfTaggedObject object also contains
//! a MapOfTaggedObjectsIter object to iterate through the objects which
//! have been added. 
class MapOfTaggedObjects: public TaggedObjectStorage
  {
    typedef std::map<int, TaggedObject *> tagged_map;
    typedef tagged_map::iterator iterator;
    typedef tagged_map::const_iterator const_iterator;
    typedef tagged_map::reference reference;
    typedef tagged_map::const_reference const_reference;
    typedef tagged_map::value_type value_type;
  private:
    std::map<int, TaggedObject *> theMap; // the map container for storing the pointers
    MapOfTaggedObjectsIter  myIter;  // the iter for this object
  protected:
    inline iterator begin(void)
      { return theMap.begin(); }
    inline iterator end(void)
      { return theMap.end(); }
    void clearComponents(void);

  public:
    MapOfTaggedObjects(CommandEntity *owr,const std::string &containerName);
    MapOfTaggedObjects(const MapOfTaggedObjects &);
    MapOfTaggedObjects &operator=(const MapOfTaggedObjects &);
    ~MapOfTaggedObjects(void);

    inline const_iterator begin(void) const
      { return theMap.begin(); }
    inline const_iterator end(void) const
      { return theMap.end(); }

    // public methods to populate a domain
    int  setSize(int newSize);
    bool addComponent(TaggedObject *newComponent);

    bool removeComponent(int tag);    
    int getNumComponents(void) const;
    
    TaggedObject *getComponentPtr(int tag);
    const TaggedObject *getComponentPtr(int tag) const;
    TaggedObjectIter &getComponents();

    MapOfTaggedObjectsIter getIter();
    
    TaggedObjectStorage *getEmptyCopy(void);
    void clearAll(bool invokeDestructor = true);
    
    void Print(std::ostream &s, int flag =0) const;
    friend class MapOfTaggedObjectsIter;
  };
} // end of XC namespace

#endif



