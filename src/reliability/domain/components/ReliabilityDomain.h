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
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/components/ReliabilityDomain.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef ReliabilityDomain_h
#define ReliabilityDomain_h

#include <reliability/domain/components/RandomVariable.h>
#include <reliability/domain/components/CorrelationCoefficient.h>
#include <reliability/domain/components/LimitStateFunction.h>
#include <reliability/domain/components/RandomVariablePositioner.h>
#include <reliability/domain/components/ParameterPositioner.h>
#include <reliability/domain/modulatingFunction/ModulatingFunction.h>
#include <reliability/domain/filter/Filter.h>
#include <reliability/domain/spectrum/Spectrum.h>

namespace XC {

class TaggedObjectStorage;

//! @ingroup ReliabilityAnalysis
//!
//! @brief Domain that keeps inside  the sensitivity, reliability and
//! optimization components are kept.
//!
//! This reliability domain is parallel to the finite element (FE) domain
//! in XC. Currently, the commands for stand-alone sensitivity:
//! analysis (e.g., sensitivityIntegrator, sensitivityAlgorithm) are set
//! in the reliability domain only and, thus, the ‘reliability’
class ReliabilityDomain
  {
  private:
    TaggedObjectStorage *theRandomVariablesPtr;
    TaggedObjectStorage *theCorrelationCoefficientsPtr;
    TaggedObjectStorage *theLimitStateFunctionsPtr;
    TaggedObjectStorage *theRandomVariablePositionersPtr;
    TaggedObjectStorage *theParameterPositionersPtr;
    TaggedObjectStorage *theModulatingFunctionsPtr;
    TaggedObjectStorage *theFiltersPtr;
    TaggedObjectStorage *theSpectraPtr;
    int tagOfActiveLimitStateFunction;
  public:
    ReliabilityDomain();
    virtual ~ReliabilityDomain();

    // Member functions to add components to the domain
    virtual bool addRandomVariable(RandomVariable *theRandomVariable);
    virtual bool addCorrelationCoefficient(CorrelationCoefficient *theCorrelationCoefficient);
    virtual bool addLimitStateFunction(LimitStateFunction *theLimitStateFunction);
    virtual bool addRandomVariablePositioner(RandomVariablePositioner *theRandomVariablePositioner);
    virtual bool addParameterPositioner(ParameterPositioner *theParameterPositioner);
    virtual bool addModulatingFunction(ModulatingFunction *theModulatingFunction);
    virtual bool addFilter(Filter *theFilter);
    virtual bool addSpectrum(Spectrum *theSpectrum);

    // Member functions to get components from the domain
    RandomVariable *getRandomVariablePtr(int tag);
    CorrelationCoefficient *getCorrelationCoefficientPtr(int tag);
    LimitStateFunction *getLimitStateFunctionPtr(int tag);
    RandomVariablePositioner *getRandomVariablePositionerPtr(int tag);
    ParameterPositioner *getParameterPositionerPtr(int tag);
    ModulatingFunction *getModulatingFunction(int tag);
    Filter *getFilter(int tag);
    Spectrum *getSpectrum(int tag);

    // Member functions giving information about the domain
    int getNumberOfRandomVariables(void);
    int getNumberOfCorrelationCoefficients(void);
    int getNumberOfLimitStateFunctions(void);
    int getNumberOfRandomVariablePositioners(void);
    int getNumberOfParameterPositioners(void);
    int getNumberOfModulatingFunctions(void);
    int getNumberOfFilters(void);
    int getNumberOfSpectra(void);

    // Member functions to set/get active limit-state function
    int getTagOfActiveLimitStateFunction(void);
    void setTagOfActiveLimitStateFunction(int tag);

    // Member functions to remove single components from the domain
    void clearAll(void);
    int removeRandomVariablePositioner(int tag);
    int removeRandomVariable(int tag);
    int removeCorrelationCoefficient(int tag);
    int removePerformanceFunction(int tag);
  };
} // end of XC namespace

#endif

