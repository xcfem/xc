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

// $Revision: 1.13 $
// $Date: 2005/11/22 19:44:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/LoadPattern.cpp,v $

// Written: fmk 07/99
// Revised:
//
// Purpose: This file contains the method definitions for class XC::LoadPattern.
// LoadPattern is a container class.
//


#include "LoadPattern.h"
#include <cstdlib>
#include <utility/matrix/ID.h>
#include "domain/domain/Domain.h"
#include <domain/constraints/SFreedom_Constraint.h>
#include <domain/load/pattern/TimeSeries.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <domain/load/ElementalLoadIter.h>
#include <domain/load/NodalLoadIter.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/load/groundMotion/GroundMotion.h>

#include <domain/load/NodalLoad.h>
#include "domain/load/elem_load.h"


#include "utility/actor/actor/ArrayCommMetaData.h"

#include "preprocessor/loaders/LoadLoader.h"

void XC::LoadPattern::libera_contenedores(void)
  {
    if(theNodalLoads)
      {
        delete theNodalLoads;
        theNodalLoads= nullptr;
      }
    
    if(theElementalLoads)
      {
        delete theElementalLoads;
        theElementalLoads= nullptr;
      }
  }

void XC::LoadPattern::libera_iteradores(void)
  {
    if(theEleIter)
      {
        delete theEleIter;
        theEleIter= nullptr;
      }
    if(theNodIter)
      {
        delete theNodIter;
        theNodIter= nullptr;
      }
  }

//! @brief Memory allocation.
void XC::LoadPattern::alloc_contenedores(void)
  {
    libera_contenedores();
    theNodalLoads = new ArrayOfTaggedObjects(this,32,"nodalLoad");
    theElementalLoads = new ArrayOfTaggedObjects(this,32,"elementLoad");

    if(!theNodalLoads || !theElementalLoads)
      {
        std::cerr << "LoadPattern::LoadPattern() - ran out of memory\n";
        exit(-1);
      }
  }

//! @brief Reserva memoria para almacenar los iteradores.
void XC::LoadPattern::alloc_iteradores(void)
  {
    libera_iteradores();
    theEleIter = new ElementalLoadIter(theElementalLoads);
    theNodIter = new NodalLoadIter(theNodalLoads);

    if(theEleIter == 0 || theNodIter == 0)
      {
        std::cerr << "LoadPattern::LoadPattern() - ran out of memory\n";
        exit(-1);
      }
  }

void XC::LoadPattern::libera(void)
  {
    setTimeSeries(nullptr);

    libera_contenedores();
    libera_iteradores();

    // AddingSensitivity:BEGIN /////////////////////////////
    if(randomLoads)
      {
        delete randomLoads;
        randomLoads= nullptr;
      }
    // AddingSensitivity:END ///////////////////////////////
  }

//! @brief Constructor
XC::LoadPattern::LoadPattern(int tag, int clasTag)
  :NodeLocker(tag,clasTag), loadFactor(0.0), gamma_f(1.0),
   theSeries(nullptr), theNodalLoads(nullptr), theElementalLoads(nullptr), 
   theNodIter(nullptr), theEleIter(nullptr), isConstant(1)
  {
    // constructor for subclass
    alloc_contenedores();
    alloc_iteradores();

    // AddingSensitivity:BEGIN /////////////////////////////
    randomLoads= nullptr;
    // AddingSensitivity:END ///////////////////////////////
  }


//! @brief Default constructor.
XC::LoadPattern::LoadPattern(void)
  :NodeLocker(0,PATTERN_TAG_LoadPattern), loadFactor(0.0), gamma_f(1.0),
   theSeries(nullptr), theNodalLoads(nullptr), theElementalLoads(nullptr),
   theNodIter(nullptr), theEleIter(nullptr), isConstant(1)
  {
    alloc_contenedores();
    alloc_iteradores();

    // AddingSensitivity:BEGIN /////////////////////////////
    randomLoads= nullptr;
    // AddingSensitivity:END ///////////////////////////////
  }

//! @brief Constructor.
XC::LoadPattern::LoadPattern(int tag)
  :NodeLocker(tag,PATTERN_TAG_LoadPattern),loadFactor(0.0), gamma_f(1.0),
   theSeries(nullptr), theNodalLoads(nullptr), theElementalLoads(nullptr),
   theNodIter(nullptr), theEleIter(nullptr), isConstant(1)
  {
    alloc_contenedores();
    alloc_iteradores();

    // AddingSensitivity:BEGIN /////////////////////////////
    randomLoads = nullptr;
    // AddingSensitivity:END ///////////////////////////////
  }


XC::LoadPattern *XC::LoadPattern::getCopy(void)
  {
    std::cerr << "LoadPattern::getCopy, not implemented." << std::endl;
    return nullptr;
  }

//! @brief Destructor.
XC::LoadPattern::~LoadPattern(void)
  { libera(); }


void XC::LoadPattern::setTimeSeries(TimeSeries *theTimeSeries)
  {
    // invoke the destructor on the old TimeSeries
    if(theSeries)
      { 
        delete theSeries;
        theSeries= nullptr;
      }
    // set the pointer to the new series object
    if(theTimeSeries)
      theSeries= theTimeSeries->getCopy();
    else
      theSeries= nullptr;
  }


//! @brief Set the domain for the pattern loads.
void XC::LoadPattern::setDomain(Domain *theDomain)
  {
    NodeLocker::setDomain(theDomain);

    // if subclass does not implement .. check for 0 pointer
    if(theNodalLoads)
      {
        NodalLoad *nodLoad= nullptr;
        NodalLoadIter &theNodalIter= getNodalLoads();
        while((nodLoad = theNodalIter()) != 0)
          nodLoad->setDomain(theDomain);
      }
    if(theElementalLoads)
      {
        ElementalLoad *eleLoad= nullptr;
        ElementalLoadIter &theElementalIter= getElementalLoads();
        while((eleLoad = theElementalIter()) != 0)
          eleLoad->setDomain(theDomain);
      }
  }

//! @brief Apply load.
bool XC::LoadPattern::addToDomain(void)
  {
    bool retval= false;
    Domain *theDomain = this->getDomain();
    if(theDomain)
      retval= theDomain->addLoadPattern(this);
    else
      std::cerr << "WARNING: LoadPattern::addToDomain() - null pointer to the domain\n";
    return retval;
  }

//! @brief Removes load.
void XC::LoadPattern::removeFromDomain(void)
  {
    Domain *theDomain = this->getDomain();
    if(theDomain)
      theDomain->removeLoadPattern(this);
    else
      std::cerr << "WARNING: LoadPattern::removeFromDomain() - null pointer to the domain\n";
  }

//! @brief Agrega the load sobre nodo being passed as parameter.
bool XC::LoadPattern::addNodalLoad(NodalLoad *load)
  {
    Domain *theDomain = this->getDomain();

    bool result = theNodalLoads->addComponent(load);
    if(result == true)
      {
        if(theDomain)
          load->setDomain(theDomain);
        load->setLoadPatternTag(this->getTag());
        load->set_owner(this);
        currentGeoTag++;
      }
    else
      std::cerr << "WARNING: LoadPattern::addNodalLoad() - load could not be added\n";
    return result;
  }

//! @brief Creates a nodal load.
//! @param tag_nodo: node identifier.
//! @param f: load vector.
XC::NodalLoad *XC::LoadPattern::newNodalLoad(const int &tag_nodo,const Vector &f)
  {
    MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(Owner());
    assert(map);
    int nextTag= map->getCurrentNodeLoadTag();
    NodalLoad *retval= new NodalLoad(nextTag,tag_nodo,f);
    if(addNodalLoad(retval))
      map->setCurrentNodeLoadTag(++nextTag);
    return retval;
  }

//! @brief Crea una load over elements.
XC::ElementalLoad *XC::LoadPattern::newElementalLoad(const std::string &cmd)
  {
    MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(Owner());
    assert(map);
    int nextTag= map->getCurrentElementLoadTag();
    ElementalLoad *retval= procesa_element_load(this,nextTag,cmd);
    if(retval)
      map->setCurrentElementLoadTag(++nextTag);
    return retval;
  }

//! @brief Agrega the load over element being passed as parameter.
bool XC::LoadPattern::addElementalLoad(ElementalLoad *load)
  {
    Domain *theDomain= getDomain();

    const bool result= theElementalLoads->addComponent(load);
    if(result)
      {
        if(theDomain)
          load->setDomain(theDomain);
        load->setLoadPatternTag(this->getTag());
        load->set_owner(this);
        currentGeoTag++;
      }
    else
      std::cerr << "WARNING: LoadPattern::addElementalLoad() - load could not be added\n";
    return result;
  }

//! @brief Appends the elemental load being passed as parameter.
bool XC::LoadPattern::newElementalLoad(ElementalLoad *load)
  {
    MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(Owner());
    int nextTag= map->getCurrentElementLoadTag();
    bool result= false;
    if(load)
      {
        result= addElementalLoad(load);
        if(result)
          map->setCurrentElementLoadTag(++nextTag); //increments by one the corresponding tag
      }
    else
      std::cerr  << "MapLoadPatterns::newElementalLoad; the pointer a the load es nulo." << std::endl;
    return result;
  }

// //! @brief Crea the load over element being passed as parameter.
// XC::NodalLoad *XC::MapLoadPatterns::newElementLoad(const int &tag_elem,const Vector &f)
//   {
//     MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(owner());
//     int nextTag= map->getCurrentNodeLoadTag();
//     NodalLoad *retval= new NodalLoad(nextTag,tag_nodo,f);
//     bool retval= addNodalLoad(theLoad);
//     if(addNodalLoad(theLoad))
//       map->setCurrentNodeLoadTag(++nextTag);
//     return retval;
//   }

//! @brief Agrega la coacción being passed as parameter.
bool XC::LoadPattern::addSFreedom_Constraint(SFreedom_Constraint *theSp)
  { return NodeLocker::addSFreedom_Constraint(theSp); }

//! @brief Returns an iterator a las nodal loads.
XC::NodalLoadIter &XC::LoadPattern::getNodalLoads(void)
  {
    theNodIter->reset();
    return *theNodIter;
  }

//! @brief Returns an iterator a las elemental loads.
XC::ElementalLoadIter &XC::LoadPattern::getElementalLoads(void)
  {
    theEleIter->reset();
    return *theEleIter;
  }

//! @brief Returns the número de nodal loadss.
int XC::LoadPattern::getNumNodalLoads(void) const
  {
    int retval= 0;
    if(theNodalLoads)
      retval= theNodalLoads->getNumComponents();
    return retval;
  }

//! @brief Returns the número de elemental loadss.
int XC::LoadPattern::getNumElementalLoads(void) const
  {
    int retval= 0;
    if(theElementalLoads)
      retval= theElementalLoads->getNumComponents();
    return retval;
  }

//! @brief Returns the total number of loads.
int XC::LoadPattern::getNumLoads(void) const
  { return getNumNodalLoads()+getNumElementalLoads(); }


//! @brief Deletes all loads.
void XC::LoadPattern::clearLoads(void)
  {
    theElementalLoads->clearAll();
    theNodalLoads->clearAll();
  }

//! @brief Deletes all loads, constraints AND pointer to time series.
void XC::LoadPattern::clearAll(void)
  {
    clearLoads();
    NodeLocker::clearAll();
    setTimeSeries(nullptr);
  }

//! @brief Elimina the load sobre nodo cuyo tag being passed as parameter.
bool XC::LoadPattern::removeNodalLoad(int tag)
  {
    bool result= theNodalLoads->removeComponent(tag);
    if(result) currentGeoTag++;
    return result;
  }

//! @brief Elimina the load over element cuyo tag being passed as parameter.
bool XC::LoadPattern::removeElementalLoad(int tag)
  {
    bool result= theElementalLoads->removeComponent(tag);
    if(result) currentGeoTag++;
    return result;
  }

//! @brief Apply the load for pseudo-time being passed as parameter.
void XC::LoadPattern::applyLoad(double pseudoTime)
  {
    if(theSeries)  // first determine the load factor
      {
        if(isConstant != 0)
          loadFactor= theSeries->getFactor(pseudoTime);
      }
    else
      std::cerr << "ERROR in applyLoad: Time series not defined. Using load factor: "
	        << loadFactor << std::endl;
    const double factor= loadFactor*gamma_f; //Ponderación de la hipótesis.

    NodalLoad *nodLoad= nullptr;
    NodalLoadIter &theNodalIter= getNodalLoads();
    while((nodLoad = theNodalIter()) != 0)
      nodLoad->applyLoad(factor);

    ElementalLoad *eleLoad= nullptr;
    ElementalLoadIter &theElementalIter= getElementalLoads();
    while((eleLoad = theElementalIter()) != 0)
      eleLoad->applyLoad(factor);

    NodeLocker::applyLoad(pseudoTime,factor);
  }

void XC::LoadPattern::setLoadConstant(void)
  { isConstant = 0; }

//! @brief Returns the factor de ponderación obtenido of the TimeSeries object.
const double &XC::LoadPattern::getLoadFactor(void) const
  { return loadFactor; }

//! @brief Returns the factor de ponderación impuesto por la combinación de hipótesis.
const double &XC::LoadPattern::GammaF(void) const
  { return gamma_f; }
//! @brief Returns the factor de ponderación impuesto por la combinación de hipótesis.
double &XC::LoadPattern::GammaF(void)
  { return gamma_f; }

//! @brief Asigna el factor de ponderación impuesto por la combinación de hipótesis.
void XC::LoadPattern::setGammaF(const double &g)
  { gamma_f= g; }


//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::LoadPattern::getDbTagData(void) const
  {
    static DbTagData retval(16);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::LoadPattern::sendData(CommParameters &cp)
  {
    int res= NodeLocker::sendData(cp);
    res+= cp.sendDoubles(loadFactor,gamma_f,getDbTagData(),CommMetaData(7));
    res+= sendTimeSeriesPtr(theSeries,8,9,getDbTagData(),cp);
    res+= cp.sendMovable(*theNodalLoads,getDbTagData(),CommMetaData(10));
    res+= cp.sendMovable(*theElementalLoads,getDbTagData(),CommMetaData(11));

    res+= cp.sendVectorPtr(randomLoads,getDbTagData(),ArrayCommMetaData(12,13,14));
    res+= cp.sendBool(RVisRandomProcessDiscretizer,getDbTagData(),CommMetaData(15));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::LoadPattern::recvData(const CommParameters &cp)
  {
    int res= NodeLocker::recvData(cp);
    res+= cp.receiveDoubles(loadFactor,gamma_f,getDbTagData(),CommMetaData(7));
    theSeries= receiveTimeSeriesPtr(theSeries,8,9,getDbTagData(),cp);

    theNodalLoads->clearAll();
    res+= theNodalLoads->recibe<NodalLoad>(getDbTagDataPos(10),cp,&FEM_ObjectBroker::getNewNodalLoad);

    theElementalLoads->clearAll();
    res+= theElementalLoads->recibe<ElementalLoad>(getDbTagDataPos(11),cp,&FEM_ObjectBroker::getNewElementalLoad);

    randomLoads= cp.receiveVectorPtr(randomLoads,getDbTagData(),ArrayCommMetaData(12,13,14));
    res+= cp.receiveBool(RVisRandomProcessDiscretizer,getDbTagData(),CommMetaData(15));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::LoadPattern::sendSelf(CommParameters &cp)
  {
    inicComm(16);
    int res= sendData(cp);
    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "LoadPattern::sendSelf() - failed to send data.\n";    
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::LoadPattern::recvSelf(const CommParameters &cp)
  {
    inicComm(16);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "LoadPattern::recvSelf() - data could not be received.\n" ;
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Prints load pattern information.
void XC::LoadPattern::Print(std::ostream &s, int flag)
  {
    s << "Load Pattern: " << this->getTag() << "\n";
    s << "  load factor: " << loadFactor << "\n";
    s << "  gamma_f: " << gamma_f << "\n";
    if(theSeries)
      theSeries->Print(s,flag);
    std::cerr << "  Nodal Loads: \n";
    theNodalLoads->Print(s,flag);
    std::cerr << "\n  Elemental Loads: \n";
    theElementalLoads->Print(s, flag);
    NodeLocker::Print(s,flag);
  }

int XC::LoadPattern::addMotion(GroundMotion &theMotion, int tag)
  {
    std::cerr << "LoadPattern::addMotion() - cannot add GroundMotion - use MultiSupport Pattern instead\n";
    return -1;
  }

XC::GroundMotion *XC::LoadPattern::getMotion(int tag)
  { return 0; }

// AddingSensitivity:BEGIN ////////////////////////////////////
void XC::LoadPattern::applyLoadSensitivity(double pseudoTime)
  {
    if(theSeries && isConstant != 0)
      loadFactor= theSeries->getFactorSensitivity(pseudoTime);

    NodalLoad *nodLoad;
    NodalLoadIter &theNodalIter = this->getNodalLoads();
    const double factor= loadFactor*gamma_f;
    while((nodLoad = theNodalIter()) != 0)
      nodLoad->applyLoad(factor);


    // Don't inlude element loads and sp constraints for now
    /*
      ElementalLoad *eleLoad;
      ElementalLoadIter &theElementalIter = this->getElementalLoads();
      while((eleLoad = theElementalIter()) != 0)
      eleLoad->applyLoad(factor);

      SFreedom_Constraint *sp;
      SFreedom_ConstraintIter &theIter = this->getSPs();
      while((sp = theIter()) != 0)
      sp->applyConstraint(factor);
    */
  }

int XC::LoadPattern::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(!theSeries)
      { std::cerr << "set/update/activate parameter is illegaly called in LoadPattern " << std::endl; }

    //const int argc= argv.size();
    // Nodal load
    if(strstr(argv[0].c_str(),"loadAtNode") != 0)
      {
        if(argv.size() < 3)
          return -1;

        RVisRandomProcessDiscretizer = false;

        int nodeNumber = atoi(argv[1]);
        NodalLoad *thePossibleNodalLoad;
        NodalLoad *theNodalLoad = 0;
        NodalLoadIter &theNodalIter = this->getNodalLoads();

        while((thePossibleNodalLoad = theNodalIter()) != 0)
          {
            if( nodeNumber == thePossibleNodalLoad->getNodeTag() )
              { theNodalLoad = thePossibleNodalLoad; }
          }

        if(theNodalLoad)
          {
	    std::vector<std::string> argv2(argv);
            argv2.erase(argv2.begin(),argv2.begin()+2);
            return theNodalLoad->setParameter(argv2, param);
          }
        else
          return -1;
    }

    else if(strstr(argv[0].c_str(),"elementPointLoad") != 0)
      {
        if(argv.size() < 3)
          return -1;
        std::cerr << "elementPointLoad not implemented." << std::endl;
//       RVisRandomProcessDiscretizer = false;

//       int eleNumber = atoi(argv[1]);
//       ElementalLoad *theEleLoad = 0;
//       ElementalLoadIter &theEleLoadIter = this->getElementalLoads();
//       while ((theEleLoad = theEleLoadIter()) != 0) {
//         int eleTag = theEleLoad->getElementTag();
//         if (eleNumber == eleTag) {
//           return theEleLoad->setParameter(&argv[2], argc-2, param);
//         }
//       }

      return -1;
    }

    else if(strstr(argv[0].c_str(),"randomProcessDiscretizer") != 0)
      {
        if(argv.size() < 2)
          return -1;
        RVisRandomProcessDiscretizer = true;
        std::vector<std::string> argv2(argv);
        argv2.erase(argv2.begin(),argv2.begin()+1);
        return theSeries->setParameter(argv2, param);
      }

    // Unknown parameter
    else
        return -1;
  }

int XC::LoadPattern::updateParameter(int parameterID, Information &info)
  {
    if(!theSeries)
      { std::cerr << "set/update/activate parameter is illegaly called in LoadPattern " << std::endl; }


    if(RVisRandomProcessDiscretizer)
      { return theSeries->updateParameter(parameterID,info); }
    else
      {
        NodalLoad *thePossibleNodalLoad = 0;
        NodalLoad *theNodalLoad = 0;
        NodalLoadIter &theNodalIter = this->getNodalLoads();

        switch (parameterID)
          {
          case 1: case -1:  // Not implemented.
            return -1;
          default:
            if(parameterID > 1000  &&  parameterID < 2000)
              {
                int nodeNumber = parameterID-1000;
                while((thePossibleNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == thePossibleNodalLoad->getNodeTag() )
                      { theNodalLoad = thePossibleNodalLoad; }
                  }
                return theNodalLoad->updateParameter(1, info);
              }
            else if(parameterID > 2000  &&  parameterID < 3000)
              {
                int nodeNumber = parameterID-2000;
                while((thePossibleNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == thePossibleNodalLoad->getNodeTag() )
                      { theNodalLoad = thePossibleNodalLoad; }
                  }
                return theNodalLoad->updateParameter(2, info);
              }
            else if(parameterID > 3000  &&  parameterID < 4000)
              {
                int nodeNumber = parameterID-3000;
                while((thePossibleNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == thePossibleNodalLoad->getNodeTag() )
                      { theNodalLoad = thePossibleNodalLoad; }
                  }
                return theNodalLoad->updateParameter(3, info);
              }
            else
              return -1;
          }
      }
  }


int XC::LoadPattern::activateParameter(int parameterID)
  {
    if(!theSeries)
      std::cerr << "set/update/activate parameter is illegaly called in LoadPattern " << std::endl;

    if(RVisRandomProcessDiscretizer)
      { return theSeries->activateParameter(parameterID); }
    else
      {
        // Don't set flag here in the load pattern itself.
        // (Assume there always may be random loads)

        NodalLoad *theNodalLoad = 0;
        NodalLoadIter &theNodalIter = this->getNodalLoads();

        if(parameterID == 0)
          {
            // Go through all nodal loads and zero out gradientIdentifier
            // (Remember: the identifier is only zero if we are in
            // the process of zeroing out all sensitivity flags).
            while((theNodalLoad = theNodalIter()) != 0)
              { theNodalLoad->activateParameter(parameterID); }
          }
        else
          {
            // Find the right nodal load and set the flag
            if(parameterID > 1000  &&  parameterID < 2000)
              {
                int nodeNumber = parameterID-1000;
                while((theNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == theNodalLoad->getNodeTag() )
                      { theNodalLoad->activateParameter(1); }
                  }
              }
            else if(parameterID > 2000  &&  parameterID < 3000)
              {
                int nodeNumber = parameterID-2000;
                while((theNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == theNodalLoad->getNodeTag() )
                      { theNodalLoad->activateParameter(2); }
                  }
              }
            else if(parameterID > 3000  &&  parameterID < 4000)
              {
                int nodeNumber = parameterID-3000;
                while((theNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == theNodalLoad->getNodeTag() )
                      { theNodalLoad->activateParameter(3); }
                  }
              }
            else
              { std::cerr << "XC::LoadPattern::gradient() -- error in identifier. " << std::endl; }
          }
      }
    return 0;
  }

const XC::Vector &XC::LoadPattern::getExternalForceSensitivity(int gradNumber)
  {

    // THIS METHOD IS CURRENTLY ONLY USED FOR THE STATIC CASE
    // IT SHOULD BE DELETED AND REPLACED BY THE DYNAMIC CASE

    // Initial declarations
    Vector tempRandomLoads(1);
    int sizeRandomLoads;

    // Start with a fresh return vector
    if(randomLoads == 0)
      { randomLoads = new XC::Vector(1); }
    else
      {
        delete randomLoads;
        randomLoads = new XC::Vector(1);
      }

    // Prepare the vector identifying which loads are random.
    NodalLoad *theNodalLoad = 0;
    NodalLoadIter &theNodalIter = this->getNodalLoads();
    int i;

    // Loop through the nodal loads to pick up possible contributions
    int nodeNumber;
    int dofNumber;
    while((theNodalLoad = theNodalIter()) != 0)
      {
        const XC::Vector &gradientVector = theNodalLoad->getExternalForceSensitivity(gradNumber);
        if(gradientVector(0) != 0.0 )
          {
            // Found a random load! Get nodeNumber and dofNumber
            nodeNumber = theNodalLoad->getNodeTag();
            dofNumber = (int)gradientVector(0);

            // Update the randomLoads vector
            sizeRandomLoads = randomLoads->Size();
            if(sizeRandomLoads == 1)
              {
                delete randomLoads;
                randomLoads = new XC::Vector(2);
                (*randomLoads)(0) = (double)nodeNumber;
                (*randomLoads)(1) = (double)dofNumber;
              }
            else
              {
                tempRandomLoads = (*randomLoads);
                delete randomLoads;
                randomLoads = new XC::Vector(sizeRandomLoads+2);
                for(i=0; i<sizeRandomLoads; i++)
                  { (*randomLoads)(i) = tempRandomLoads(i); }
                (*randomLoads)(sizeRandomLoads) = nodeNumber;
                (*randomLoads)(sizeRandomLoads+1) = dofNumber;
              }
          }
      }
    return (*randomLoads);
  }

// AddingSensitivity:END //////////////////////////////////////
