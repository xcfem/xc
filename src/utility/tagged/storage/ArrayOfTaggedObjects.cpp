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
// $Source: /usr/local/cvs/OpenSees/SRC/utility/tagged/storage/ArrayOfTaggedObjects.cpp,v $


// File: ~/utility/tagged/storage/ArrayOfTaggedObjects.C
//
// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of the XC::ArrayOfTaggedObjects
// class.
//
// What: "@(#) ArrayOfTaggedObjects.C, revA"

#include <utility/tagged/TaggedObject.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"

//! @brief Libera la memoria ocupada por los componentes.
void XC::ArrayOfTaggedObjects::clearComponents(void)
  {
    const size_t sz= theComponents.size();
    for(size_t i=0; i<sz; i++)
      {
        if(theComponents[i])
          {
            delete theComponents[i];
            theComponents[i]= nullptr;
          }
      }
    theComponents.clear();
    positionLastEntry= 0;
    sizeComponentArray= 0; //Error if not coherent with theComponents.size()
  }

//! @brief Constructor.
XC::ArrayOfTaggedObjects::ArrayOfTaggedObjects(EntCmd *owr,int size,const std::string &containerName)
  :TaggedObjectStorage(owr,containerName), numComponents(0),sizeComponentArray(0), positionLastEntry(0),
   positionLastNoFitEntry(0),fitFlag(true), myIter(*this)
  {
    // get the components array from the heap
    theComponents= tagged_vector(size,static_cast<TaggedObject *>(nullptr));
    sizeComponentArray= size;

  }

//! @brief Constructor de copia.
XC::ArrayOfTaggedObjects::ArrayOfTaggedObjects(const ArrayOfTaggedObjects &otro)
  : TaggedObjectStorage(otro), numComponents(otro.numComponents),sizeComponentArray(otro.sizeComponentArray), positionLastEntry(otro.positionLastEntry), positionLastNoFitEntry(otro.positionLastNoFitEntry),fitFlag(otro.fitFlag), theComponents(otro.theComponents.size(),nullptr), myIter(*this)
  { copia(otro); }

//! @brief Operador asignación.
XC::ArrayOfTaggedObjects &XC::ArrayOfTaggedObjects::operator=(const ArrayOfTaggedObjects &otro)
  {
    TaggedObjectStorage::operator=(otro);
    numComponents= otro.numComponents;
    sizeComponentArray= otro.sizeComponentArray;
    positionLastEntry= otro.positionLastEntry;
    positionLastNoFitEntry= otro.positionLastNoFitEntry;
    fitFlag= otro.fitFlag;
    clearComponents();
    theComponents= tagged_vector(otro.theComponents.size(),nullptr);
    copia(otro);
    return *this;
  }


XC::ArrayOfTaggedObjects::~ArrayOfTaggedObjects(void)
  { clearComponents(); }



int XC::ArrayOfTaggedObjects::setSize(int newSize)
  {
    // check a valid size
    if(newSize < 0 && newSize > sizeComponentArray) //?? LCPT 10-2006.
      {
        std::cerr << "ArrayOfTaggedObjects::setSize - invalid size " << newSize << std::endl;
        return -1;
      }

    if(newSize < 2) newSize= 2; // make 2 the min size;


    // store the needed info of the old array
    theComponents.resize(newSize,nullptr);
    sizeComponentArray= theComponents.size();
    assert(newSize==sizeComponentArray);

    return 0;
  }

//! @brief Agrega un elemento al contenedor.
bool XC::ArrayOfTaggedObjects::addComponent(TaggedObject *newComponent)
  {
    // check to see that no other component already exists
    TaggedObject *other= getComponentPtr(newComponent->getTag());
    if(other)
      {
        std::cerr << "WARNING ArrayOfTaggedObjects::addComponent() - component" <<
                     " with tag already exists, not adding component with tag: " <<
        newComponent->getTag() << std::endl;
        return false;
      }

    // check to see if size of current array is big enough. if not resize.
    if(numComponents == sizeComponentArray)
      if(this->setSize(2*numComponents) < 0)
        {
          std::cerr << "XC::ArrayOfTaggedObjects::addComponent()- failed to enlarge the array with size" 
                    << 2*numComponents << std::endl;
          return false;
        }

    // we try to the Component in nicely, i.e. in
    // position given by the newComponents Tag

    int newComponentTag= newComponent->getTag();
    newComponent->set_owner(this);

    if((newComponentTag >= 0) && (newComponentTag < sizeComponentArray))
      {
        if(theComponents[newComponentTag]==nullptr)
          { // it will go in nicely
            theComponents[newComponentTag]= newComponent;
            numComponents++;
            if(newComponentTag > positionLastEntry)
              positionLastEntry= newComponentTag;
            return true;
          }
      }

    // it won't go in nicely, so put wherever we can get it in
    while(theComponents[positionLastNoFitEntry] != 0 && positionLastNoFitEntry < sizeComponentArray )
      positionLastNoFitEntry++;

    // just in case we don't get a location ..  though we should!!
    if(positionLastNoFitEntry == sizeComponentArray)
      {
        std::cerr << "ArrayOfTaggedObjects::addComponent() - could not - find a vacant spot after enlarging!!\n";
        return false;
      }
    theComponents[positionLastNoFitEntry]= newComponent;
    numComponents++;
    if(positionLastNoFitEntry > positionLastEntry)
      positionLastEntry= positionLastNoFitEntry;
    fitFlag= false;
    transmitIDs= true; //Se agrega un componente.
    return true;  // o.k.
  }

bool XC::ArrayOfTaggedObjects::removeComponent(int tag)
  {
    bool retval= false;

    // first check to see if object is located at pos given by tag
    if((tag >= 0) && (tag < sizeComponentArray))
      {
        if(fitFlag == true)// if all objects in nicely then has to be at location given by tag
          {
            // if the object is there decrement numComponents added and
            // check if need to reset positionLastEntry marker
            if(theComponents[tag])
              {
                delete theComponents[tag];
                theComponents[tag]= nullptr;
                retval= true;                
                numComponents--;
                if(positionLastEntry == tag)
                  {
                    for(int i= positionLastEntry; i>=0; i--)
                      if(theComponents[i])
                        {
                          positionLastEntry= i;
                          i= -1; // break out of loop
                        }
                  }
              }
          }
        else  // it still may be at nice location so do as above
          if(theComponents[tag])
            if((theComponents[tag]->getTag()) == tag)
              {
                delete theComponents[tag];
                theComponents[tag]= nullptr;
                if(positionLastEntry == tag)
                  {
                    for(int i= positionLastEntry;i>=0;i--)
                      if(theComponents[i])
                        {
                          positionLastEntry= i;
                          i= -1; // break out of loop
                        }
                  }
                positionLastNoFitEntry= 0;
                retval= true;
                numComponents--;
              }
      }
    else
      {
        // else we have to look through array until we find it or
        // reach lastPosition used
        for(int i=0; i<=positionLastEntry; i++)
          if(theComponents[i])
            if(theComponents[i]->getTag() == tag)
              {
                // we found the component so do as above again
                // and then return a pointer to the component
                delete theComponents[i];
                theComponents[i]= nullptr;
                if(positionLastEntry == i)
                  {
                    for(int j= positionLastEntry;j>=0;j--)
                      if(theComponents[j])
                        {
                          positionLastEntry= j;
                          j= -1; // break out of loop
                        }
                  }
                positionLastNoFitEntry= 0;
                retval=true;
                numComponents--;
              }
      }
    transmitIDs= retval; //Se elimina un componente.
    return retval;
  }

int XC::ArrayOfTaggedObjects::getNumComponents(void) const
  { return numComponents; }

//! @brief Devuelve un puntero al objeto cuyo índice se pasa como parámetro.
XC::TaggedObject *XC::ArrayOfTaggedObjects::getComponentPtr(int tag)
  {

    // first check it's not where we would like it
    if((tag >= 0) && (tag < sizeComponentArray))
      {
        if(fitFlag == true) // either its at nice position or not entered
          return theComponents[tag];
        else
          {                    // it still may be at nice location
            if(theComponents[tag])
              if((theComponents[tag]->getTag()) == tag)
                return theComponents[tag];
          }
      }
    // else we have to look through array until we find it or
    // reach lastPosition used
    for(int i=0; i<=positionLastEntry; i++)
      if(theComponents[i] != 0)
        if(theComponents[i]->getTag() == tag)
          return theComponents[i];
    // its not in the array
    return 0;
  }

//! @brief Devuelve un puntero al objeto cuyo índice se pasa como parámetro.
const XC::TaggedObject *XC::ArrayOfTaggedObjects::getComponentPtr(int tag) const
  {

    // first check it's not where we would like it
    if((tag >= 0) && (tag < sizeComponentArray))
      {
        if(fitFlag == true) // either its at nice position or not entered
          return theComponents[tag];
        else
          {                    // it still may be at nice location
            if(theComponents[tag])
              if((theComponents[tag]->getTag()) == tag)
                return theComponents[tag];
          }
      }
    // else we have to look through array until we find it or
    // reach lastPosition used
    for(int i=0; i<=positionLastEntry; i++)
      if(theComponents[i] != 0)
        if(theComponents[i]->getTag() == tag)
          return theComponents[i];
    // its not in the array
    return 0;
  }

//! @brief reset the iter to point to first component and then return a reference to the iter.
XC::TaggedObjectIter &XC::ArrayOfTaggedObjects::getComponents(void)
  {
    myIter.reset();
    return myIter;
  }


XC::ArrayOfTaggedObjectsIter XC::ArrayOfTaggedObjects::getIter(void)
  {
    // return a new iter to the components, more expensive to use
    // this iter as two memory calls to heap .. needed if user needs
    // to have multiple iters running in same code segment!
    return ArrayOfTaggedObjectsIter(*this);
  }


XC::TaggedObjectStorage *XC::ArrayOfTaggedObjects::getEmptyCopy(void)
  {
    ArrayOfTaggedObjects *theCopy= new ArrayOfTaggedObjects(Owner(), sizeComponentArray, containerName);
    if(!theCopy)
      { std::cerr << "XC::ArrayOfTaggedObjects::getEmptyCopy - out of memory\n"; }
    return theCopy;
  }

//! @brief Libera la memoria.
//XXX Averiguar si esto da lugar a un memory leak (con invokeDestructors= false).
void XC::ArrayOfTaggedObjects::clearAll(bool invokeDestructors)
  {
    transmitIDs= true;
    if(numComponents>0)
      {
        if(invokeDestructors)
          { clearComponents(); }
        else
          {
            //just set the array pointers to 0
            const size_t sz= theComponents.size();
            for(size_t i=0; i<sz; i++)
              {
                if(theComponents[i])
                  { theComponents[i]= nullptr; }
              }
          }
        positionLastEntry= 0;
        positionLastNoFitEntry= 0;
        fitFlag= true;
        numComponents= 0;
      }
  }

//! @brief method which invokes Print on all components
void XC::ArrayOfTaggedObjects::Print(std::ostream &s, int flag)
  {
    // go through the array invoking Print on non-zero entries
    for(int i=0; i<=positionLastEntry; i++)
      if(theComponents[i] != 0)
        theComponents[i]->Print(s, flag);
  }


