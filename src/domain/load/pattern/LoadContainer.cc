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


#include "LoadContainer.h"
#include <cstdlib>
#include <utility/matrix/ID.h>
#include "domain/domain/Domain.h"
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <domain/load/ElementalLoad.h>
#include <domain/load/ElementalLoadIter.h>
#include <domain/load/NodalLoadIter.h>
#include <domain/load/NodalLoad.h>
#include "domain/load/elem_load.h"


#include "utility/actor/actor/ArrayCommMetaData.h"

#include "preprocessor/prep_handlers/LoadHandler.h"

//! @brief Frees memory.
void XC::LoadContainer::free_containers(void)
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

//! @brief Frees memory.
void XC::LoadContainer::free_iterators(void)
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
void XC::LoadContainer::alloc_containers(void)
  {
    free_containers();
    theNodalLoads = new ArrayOfTaggedObjects(this,32,"nodalLoad");
    theElementalLoads = new ArrayOfTaggedObjects(this,32,"elementLoad");

    if(!theNodalLoads || !theElementalLoads)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ran out of memory\n";
        exit(-1);
      }
  }

//! @brief Reserves memory for iterators.
void XC::LoadContainer::alloc_iterators(void)
  {
    free_iterators();
    theEleIter= new ElementalLoadIter(theElementalLoads);
    theNodIter= new NodalLoadIter(theNodalLoads);

    if(theEleIter == 0 || theNodIter == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ran out of memory\n";
        exit(-1);
      }
  }

//! @brief Frees memory.
void XC::LoadContainer::free(void)
  {
    free_containers();
    free_iterators();
  }

//! @brief Default constructor.
XC::LoadContainer::LoadContainer(CommandEntity *owr)
  :CommandEntity(owr), MovableObject(PATTERN_TAG_LoadContainer),
   theNodalLoads(nullptr), theElementalLoads(nullptr),
   theNodIter(nullptr), theEleIter(nullptr)
  {
    alloc_containers();
    alloc_iterators();
  }

//! @brief Virtual constructor.
XC::LoadContainer *XC::LoadContainer::getCopy(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return nullptr;
  }

//! @brief Destructor.
XC::LoadContainer::~LoadContainer(void)
  { free(); }


//! @brief Set the domain for the pattern loads.
void XC::LoadContainer::setDomain(Domain *theDomain)
  {
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

//! @brief Adds the nodal load being passed as parameter.
bool XC::LoadContainer::addNodalLoad(NodalLoad *load)
  {
    const bool result= theNodalLoads->addComponent(load);
    if(!result)
      std::cerr << "LoadContainer::" << __FUNCTION__
	        << "; WARNING: load could not be added\n";
    return result;
  }

//! @brief Adds the element load being passed as parameter.
bool XC::LoadContainer::addElementalLoad(ElementalLoad *load)
  {
    const bool result= theElementalLoads->addComponent(load);
    if(!result)
      std::cerr << "LoadContainer::" << __FUNCTION__
	        << "; WARNING: load could not be added\n";
    return result;
  }

//! @brief Returns an iterator to the nodal loads.
XC::NodalLoadIter &XC::LoadContainer::getNodalLoads(void)
  {
    theNodIter->reset();
    return *theNodIter;
  }

//! @brief Returns an iterator to the elemental loads.
XC::ElementalLoadIter &XC::LoadContainer::getElementalLoads(void)
  {
    theEleIter->reset();
    return *theEleIter;
  }

//! @brief Returns the number of nodal loadss.
int XC::LoadContainer::getNumNodalLoads(void) const
  {
    int retval= 0;
    if(theNodalLoads)
      retval= theNodalLoads->getNumComponents();
    return retval;
  }

//! @brief Returns the number of elemental loadss.
int XC::LoadContainer::getNumElementalLoads(void) const
  {
    int retval= 0;
    if(theElementalLoads)
      retval= theElementalLoads->getNumComponents();
    return retval;
  }

//! @brief Returns the total number of loads.
int XC::LoadContainer::getNumLoads(void) const
  { return getNumNodalLoads()+getNumElementalLoads(); }


//! @brief Deletes all loads.
void XC::LoadContainer::clearLoads(void)
  {
    theElementalLoads->clearAll();
    theNodalLoads->clearAll();
  }

//! @brief Deletes all loads, constraints AND pointer to time series.
void XC::LoadContainer::clearAll(void)
  {
    clearLoads();
  }

//! @brief To remove the nodal load whose identifier is given by \p tag from
//! the LoadPattern and sets the loads associated Domain object to
//! \f$0\f$. Returns a pointer to the load if successfully removed, otherwise
// \f$0\f$ is returned. 
bool XC::LoadContainer::removeNodalLoad(int tag)
  { return theNodalLoads->removeComponent(tag); }

//! @brief To remove the elemental load whose identifier is given by \p tag from
//! the LoadPattern and set the loads associated Domain object to
//! \f$0\f$. Returns a pointer to the load if successfully removed, otherwise
//! \f$0\f$ is returned. 
bool XC::LoadContainer::removeElementalLoad(int tag)
  { return theElementalLoads->removeComponent(tag); }

//! @brief Apply the load multiplied by the factor.
//! 
//! To apply the load for the pseudo time \p pseudoTime. From the
//! associated TimeSeries object the LoadPattern will obtain a current
//! load factor for the pseudo time. It will then invoke {\em
//! applyLoad(load factor)} on the loads and {\em applyConstraint(load
//! factor)} on the single-point constraints in the LoadPattern. If {\em
//! setLoadConstant()} has been invoked, the saved load factor is used and
//! no call is made to the TimeSeries object. If no TimeSeries is
//! associated with the object a load factor of \f$0.0\f$ is used.
void XC::LoadContainer::applyLoad(const double &factor)
  {
    NodalLoad *nodLoad= nullptr;
    NodalLoadIter &theNodalIter= getNodalLoads();
    while((nodLoad = theNodalIter()) != 0)
      nodLoad->applyLoad(factor);

    ElementalLoad *eleLoad= nullptr;
    ElementalLoadIter &theElementalIter= getElementalLoads();
    while((eleLoad = theElementalIter()) != 0)
      eleLoad->applyLoad(factor);
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::LoadContainer::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::LoadContainer::sendData(CommParameters &cp)
  {
    int res= cp.sendMovable(*theNodalLoads,getDbTagData(),CommMetaData(0));
    res+= cp.sendMovable(*theElementalLoads,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::LoadContainer::recvData(const CommParameters &cp)
  {
    clearAll();
    int res= theNodalLoads->receive<NodalLoad>(getDbTagDataPos(0),cp,&FEM_ObjectBroker::getNewNodalLoad);
    res+= theElementalLoads->receive<ElementalLoad>(getDbTagDataPos(1),cp,&FEM_ObjectBroker::getNewElementalLoad);
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::LoadContainer::sendSelf(CommParameters &cp)
  {
    inicComm(2);
    int res= sendData(cp);
    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data.\n";    
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::LoadContainer::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; data could not be received.\n" ;
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Prints load pattern information.
void XC::LoadContainer::Print(std::ostream &s, int flag) const
  {
    s << "  Load container\n";
    s << "    Nodal Loads: \n";
    theNodalLoads->Print(s,flag);
    s << "\n    Elemental Loads: \n";
    theElementalLoads->Print(s, flag);
  }

// AddingSensitivity:BEGIN ////////////////////////////////////
void XC::LoadContainer::applyLoadSensitivity(const double &factor)
  {
    NodalLoad *nodLoad;
    NodalLoadIter &theNodalIter = this->getNodalLoads();
    while((nodLoad = theNodalIter()) != 0)
      nodLoad->applyLoad(factor);


    // Don't include element loads for now
    /*
      ElementalLoad *eleLoad;
      ElementalLoadIter &theElementalIter = this->getElementalLoads();
      while((eleLoad = theElementalIter()) != 0)
      eleLoad->applyLoad(factor);
    */
  }

int XC::LoadContainer::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    //const int argc= argv.size();
    if(strstr(argv[0].c_str(),"loadAtNode") != 0)// Nodal load
      {
        if(argv.size() < 3)
          return -1;

        const int nodeNumber = atoi(argv[1]);
        NodalLoad *thePossibleNodalLoad= nullptr;
        NodalLoad *theNodalLoad= nullptr;
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
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; elementPointLoad not implemented." << std::endl;
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
    else // Unknown parameter
      return -1;
  }

int XC::LoadContainer::updateParameter(int parameterID, Information &info)
  {
    NodalLoad *thePossibleNodalLoad= nullptr;
    NodalLoad *theNodalLoad= nullptr;
    NodalLoadIter &theNodalIter= this->getNodalLoads();

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


int XC::LoadContainer::activateParameter(int parameterID)
  {
    NodalLoad *theNodalLoad= nullptr;
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
            const int nodeNumber = parameterID-1000;
            while((theNodalLoad = theNodalIter()) != 0)
              {
                if(nodeNumber == theNodalLoad->getNodeTag() )
                  { theNodalLoad->activateParameter(1); }
              }
          }
        else if(parameterID > 2000  &&  parameterID < 3000)
          {
            const int nodeNumber = parameterID-2000;
            while((theNodalLoad = theNodalIter()) != 0)
              {
                if(nodeNumber == theNodalLoad->getNodeTag() )
                  { theNodalLoad->activateParameter(2); }
              }
          }
        else if(parameterID > 3000  &&  parameterID < 4000)
          {
            const int nodeNumber = parameterID-3000;
            while((theNodalLoad = theNodalIter()) != 0)
              {
                if(nodeNumber == theNodalLoad->getNodeTag() )
                  { theNodalLoad->activateParameter(3); }
              }
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; error in identifier. " << std::endl;
      }
    return 0;
  }

XC::Vector XC::LoadContainer::getExternalForceSensitivity(int gradNumber)
  {
    // THIS METHOD IS CURRENTLY ONLY USED FOR THE STATIC CASE
    // IT SHOULD BE DELETED AND REPLACED BY THE DYNAMIC CASE

    // Initial declarations
    Vector tempRandomLoads(1);
    int sizeRandomLoads;

    // Start with a fresh return vector
    Vector randomLoads(1);

    // Prepare the vector identifying which loads are random.
    NodalLoad *theNodalLoad= nullptr;
    NodalLoadIter &theNodalIter = this->getNodalLoads();

    // Loop through the nodal loads to pick up possible contributions
    int nodeNumber;
    int dofNumber;
    while((theNodalLoad = theNodalIter()) != 0)
      {
        const Vector &gradientVector= theNodalLoad->getExternalForceSensitivity(gradNumber);
        if(gradientVector(0) != 0.0 )
          {
            // Found a random load! Get nodeNumber and dofNumber
            nodeNumber= theNodalLoad->getNodeTag();
            dofNumber= (int)gradientVector(0);

            // Update the randomLoads vector
            sizeRandomLoads= randomLoads.Size();
            if(sizeRandomLoads == 1)
              {
                randomLoads.resize(2);
                randomLoads(0)= (double)nodeNumber;
                randomLoads(1)= (double)dofNumber;
              }
            else
              {
                const Vector tempRandomLoads= randomLoads;
                randomLoads.resize(sizeRandomLoads+2);
                for(int i=0; i<sizeRandomLoads; i++)
                  { randomLoads(i) = tempRandomLoads(i); }
                randomLoads(sizeRandomLoads)= nodeNumber;
                randomLoads(sizeRandomLoads+1)= dofNumber;
              }
          }
      }
    return randomLoads;
  }

// AddingSensitivity:END //////////////////////////////////////
