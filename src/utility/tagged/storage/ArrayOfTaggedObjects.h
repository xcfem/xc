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
// ArrayOfTaggedObjects. ArrayOfTaggedObjects is a storage class. The class
// is responsible for holding and providing access to objects of type
// TaggedObject. The data structure used to hold the objects is a simple
// array of pointers. As a one dimensional array is used certain ideas are tried
// to improve performance: (1) if the array needs to be larger to hold more
// components, the array size is doubled and (2) when adding/retrieving components,
// the array location given by the components tag is first checked.
//
// What: "@(#) ArrayOfTaggedObjects.h, revA"


#include <utility/tagged/storage/TaggedObjectStorage.h>
#include <utility/tagged/storage/ArrayOfTaggedObjectsIter.h>

namespace XC
  {
class ArrayOfTaggedObjects : public TaggedObjectStorage
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

    tagged_vector theComponents; // the array
    ArrayOfTaggedObjectsIter  myIter; // an iter for accessing the objects
  protected:
    iterator begin(void)
      { return theComponents.begin(); }
    iterator end(void)
      { return theComponents.end(); }
    void clearComponents(void);
    bool procesa_comando(CmdStatus &status);
  public:
    ArrayOfTaggedObjects(EntCmd *owr,int size,const std::string &containerName);
    ArrayOfTaggedObjects(const ArrayOfTaggedObjects &otro);
    ArrayOfTaggedObjects &operator=(const ArrayOfTaggedObjects &otro);
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

    virtual any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &s, int flag =0);
    friend class ArrayOfTaggedObjectsIter;
  };

} // end of XC namespace

#endif


