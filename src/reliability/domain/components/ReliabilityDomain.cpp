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
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.5 $
// $Date: 2003/10/27 23:04:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/components/ReliabilityDomain.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/components/ReliabilityDomain.h>

#include <reliability/domain/components/CorrelationCoefficient.h>
#include <reliability/domain/components/RandomVariable.h>
#include <reliability/domain/components/LimitStateFunction.h>
#include <reliability/domain/components/RandomVariablePositioner.h>
#include <reliability/domain/components/ParameterPositioner.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <reliability/domain/modulatingFunction/ModulatingFunction.h>
#include <reliability/domain/filter/Filter.h>
#include <reliability/domain/spectrum/Spectrum.h>


XC::ReliabilityDomain::ReliabilityDomain()
  {
    theRandomVariablesPtr = new ArrayOfTaggedObjects(nullptr,256,"randomVar");
    theCorrelationCoefficientsPtr = new ArrayOfTaggedObjects(nullptr,256,"correlCoef");
    theLimitStateFunctionsPtr = new ArrayOfTaggedObjects(nullptr,256,"limitStateFunc");
    theRandomVariablePositionersPtr = new ArrayOfTaggedObjects(nullptr,256,"varPos");
    theParameterPositionersPtr = new ArrayOfTaggedObjects(nullptr,256,"parameterPos");
    theModulatingFunctionsPtr = new ArrayOfTaggedObjects(nullptr,256,"modFunc");
    theFiltersPtr = new ArrayOfTaggedObjects(nullptr,256,"filter");
    theSpectraPtr = new ArrayOfTaggedObjects(nullptr,256,"spectra");
    tagOfActiveLimitStateFunction = 1;
  }

//! @brief Destructor.
XC::ReliabilityDomain::~ReliabilityDomain(void)
  {
    if(theRandomVariablesPtr) delete theRandomVariablesPtr;
    if(theCorrelationCoefficientsPtr) delete theCorrelationCoefficientsPtr;
    if(theLimitStateFunctionsPtr) delete theLimitStateFunctionsPtr;
    if(theRandomVariablePositionersPtr) delete theRandomVariablePositionersPtr;
    if(theParameterPositionersPtr) delete theParameterPositionersPtr;
    if(theModulatingFunctionsPtr) delete theModulatingFunctionsPtr;
    if(theSpectraPtr) delete theSpectraPtr;
    if(theFiltersPtr) delete theFiltersPtr;
  }

//! @brief Clear object members.
void XC::ReliabilityDomain::clearAll(void)
  {
    if(theRandomVariablesPtr)
      theRandomVariablesPtr->clearAll();
    // Need to do the other stuff -- MHS
  }


bool XC::ReliabilityDomain::addRandomVariable(RandomVariable *theRandomVariable)
  {
    bool result = theRandomVariablesPtr->addComponent(theRandomVariable);
    return result;
  }

bool XC::ReliabilityDomain::addCorrelationCoefficient(CorrelationCoefficient *theCorrelationCoefficient)
  {
    bool result = theCorrelationCoefficientsPtr->addComponent(theCorrelationCoefficient);
    return result;
  }

bool XC::ReliabilityDomain::addLimitStateFunction(LimitStateFunction *theLimitStateFunction)
  {
    bool result = theLimitStateFunctionsPtr->addComponent(theLimitStateFunction);
    return result;
  }

bool XC::ReliabilityDomain::addRandomVariablePositioner(RandomVariablePositioner *theRandomVariablePositioner)
  {
    bool result = theRandomVariablePositionersPtr->addComponent(theRandomVariablePositioner);
    return result;
  }

bool XC::ReliabilityDomain::addParameterPositioner(ParameterPositioner *theParameterPositioner)
  {
    bool result = theParameterPositionersPtr->addComponent(theParameterPositioner);
    return result;
  }

bool XC::ReliabilityDomain::addModulatingFunction(ModulatingFunction *theModulatingFunction)
  {
    bool result = theModulatingFunctionsPtr->addComponent(theModulatingFunction);
    return result;
  }

bool XC::ReliabilityDomain::addSpectrum(Spectrum *theSpectrum)
  {
    bool result = theSpectraPtr->addComponent(theSpectrum);
    return result;
  }

bool XC::ReliabilityDomain::addFilter(Filter *theFilter)
  {
    bool result = theFiltersPtr->addComponent(theFilter);
    return result;
  }




XC::RandomVariable *XC::ReliabilityDomain::getRandomVariablePtr(int tag)
  {
    TaggedObject *theComponent = theRandomVariablesPtr->getComponentPtr(tag);
//	if ( *theComponent == 0 )
//		return 0;
    RandomVariable *result = (RandomVariable *) theComponent;
    return result;
  }


XC::CorrelationCoefficient *XC::ReliabilityDomain::getCorrelationCoefficientPtr(int tag)
  {
    TaggedObject *theComponent = theCorrelationCoefficientsPtr->getComponentPtr(tag);
//	if ( *theComponent == 0 )
//		return 0;
    CorrelationCoefficient *result = (CorrelationCoefficient *) theComponent;
    return result;
  }


XC::LimitStateFunction *XC::ReliabilityDomain::getLimitStateFunctionPtr(int tag)
  {
    TaggedObject *theComponent = theLimitStateFunctionsPtr->getComponentPtr(tag);
//	if ( *theComponent == 0 )
//		return 0;
    LimitStateFunction *result = (LimitStateFunction *) theComponent;
    return result;
  }


XC::RandomVariablePositioner *XC::ReliabilityDomain::getRandomVariablePositionerPtr(int tag)
  {
    TaggedObject *theComponent = theRandomVariablePositionersPtr->getComponentPtr(tag);
//	if ( *theComponent == 0 )
//		return 0;
    RandomVariablePositioner *result = (RandomVariablePositioner *) theComponent;
    return result;
  }

XC::ParameterPositioner *XC::ReliabilityDomain::getParameterPositionerPtr(int tag)
  {
    TaggedObject *theComponent = theParameterPositionersPtr->getComponentPtr(tag);
//	if ( *theComponent == 0 )
//		return 0;
    ParameterPositioner *result = (ParameterPositioner *) theComponent;
    return result;
  }


XC::ModulatingFunction *XC::ReliabilityDomain::getModulatingFunction(int tag)
  {
    TaggedObject *theComponent = theModulatingFunctionsPtr->getComponentPtr(tag);
//	if ( *theComponent == 0 )
//		return 0;
    ModulatingFunction *result = (ModulatingFunction *) theComponent;
    return result;
  }


XC::Spectrum *XC::ReliabilityDomain::getSpectrum(int tag)
  {
    TaggedObject *theComponent = theSpectraPtr->getComponentPtr(tag);
//	if ( *theComponent == 0 )
//		return 0;
    Spectrum *result = (Spectrum *) theComponent;
    return result;
  }


XC::Filter *XC::ReliabilityDomain::getFilter(int tag)
  {
    TaggedObject *theComponent = theFiltersPtr->getComponentPtr(tag);
//	if ( *theComponent == 0 )
//		return 0;
    Filter *result = (Filter *) theComponent;
    return result;
  }


int XC::ReliabilityDomain::removeRandomVariablePositioner(int tag)
  {
    theRandomVariablePositionersPtr->removeComponent(tag);

    return 0;
  }

int XC::ReliabilityDomain::removeRandomVariable(int tag)
  {
    theRandomVariablesPtr->removeComponent(tag);

    return 0;
  }


int XC::ReliabilityDomain::removeCorrelationCoefficient(int tag)
  {
    theCorrelationCoefficientsPtr->removeComponent(tag);

    return 0;
  }

int XC::ReliabilityDomain::removePerformanceFunction(int tag)
  {
    theLimitStateFunctionsPtr->removeComponent(tag);

    return 0;
  }


int XC::ReliabilityDomain::getNumberOfRandomVariables()
  {
    return theRandomVariablesPtr->getNumComponents();
  }

int XC::ReliabilityDomain::getNumberOfCorrelationCoefficients()
  {
    return theCorrelationCoefficientsPtr->getNumComponents();
  }

int XC::ReliabilityDomain::getNumberOfLimitStateFunctions()
  {
    return theLimitStateFunctionsPtr->getNumComponents();
  }

int XC::ReliabilityDomain::getNumberOfRandomVariablePositioners()
  {
    return theRandomVariablePositionersPtr->getNumComponents();
  }

int XC::ReliabilityDomain::getNumberOfParameterPositioners()
  {
    return theParameterPositionersPtr->getNumComponents();
  }

int XC::ReliabilityDomain::getNumberOfModulatingFunctions()
  {
    return theModulatingFunctionsPtr->getNumComponents();
  }

int XC::ReliabilityDomain::getNumberOfFilters()
  {
    return theFiltersPtr->getNumComponents();
  }

int XC::ReliabilityDomain::getNumberOfSpectra()
  {
    return theSpectraPtr->getNumComponents();
  }

int XC::ReliabilityDomain::getTagOfActiveLimitStateFunction()
  {
    return tagOfActiveLimitStateFunction;
  }

void XC::ReliabilityDomain::setTagOfActiveLimitStateFunction(int passedTag)
  {
    tagOfActiveLimitStateFunction = passedTag;
  }

