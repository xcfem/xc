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
// MapOfTaggedObjects. MapOfTaggedObjects is a storage class. The class 
// is responsible for holding and providing access to objects of type 
// TaggedObject. A map template of the standard template class is used to store
// the pointers to these objects.
//
// What: "@(#) MapOfTaggedObjects.h, revA"


#include <utility/tagged/storage/TaggedObjectStorage.h>
#include <utility/tagged/storage/MapOfTaggedObjectsIter.h>

namespace XC {
class MapOfTaggedObjects : public TaggedObjectStorage
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
    bool procesa_comando(CmdStatus &status);
  public:
    MapOfTaggedObjects(EntCmd *owr,const std::string &containerName);
    MapOfTaggedObjects(const MapOfTaggedObjects &otro);
    MapOfTaggedObjects &operator=(const MapOfTaggedObjects &otro);
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
    
    void Print(std::ostream &s, int flag =0);
    friend class MapOfTaggedObjectsIter;
  };
} // fin namespace XC

#endif



