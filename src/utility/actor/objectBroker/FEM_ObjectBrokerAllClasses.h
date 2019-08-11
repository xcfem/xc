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
                                                                        
// $Revision: 1.5 $
// $Date: 2009/05/14 22:52:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/objectBroker/FEM_ObjectBrokerAllClasses.h,v $
                                                                        
// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for FEM_ObjectBroker.
// FEM_ObjectBroker is is an object broker class for the finite element
// method. All methods are virtual to allow for subclasses; which can be
// used by programmers when introducing new subclasses of the main objects.
//
// What: "@(#) FEM_ObjectBrokerAllClasses.h, revA"


#ifndef FEM_ObjectBrokerAllClasses_h
#define FEM_ObjectBrokerAllClasses_h

#include "FEM_ObjectBroker.h"

namespace XC {
//! @ingroup IPComm
//
//! @brief The same as FEM_ObjectBroker?
class FEM_ObjectBrokerAllClasses: public FEM_ObjectBroker
  {
  private:
    DomainSolver *lastDomainSolver;
  public:
    FEM_ObjectBrokerAllClasses(void);

    Actor*getNewActor(int classTag, Channel *theChannel);
    
    PartitionedModelBuilder *
      getPtrNewPartitionedModelBuilder(Subdomain &theSub,
                                       int classTag);
    
    // methods to get new modelling class objects
    Element *getNewElement(int classTag);
    Node *getNewNode(int classTag);
    MFreedom_Constraint *getNewMP(int classTag);
    SFreedom_Constraint *getNewSP(int classTag);
    NodalLoad *getNewNodalLoad(int classTag);
    
    SectionForceDeformation *getNewSection(int classTag);    
    NDMaterial *getNewNDMaterial(int classTag);

    ConvergenceTest *getNewConvergenceTest(int classTag);
    GroundMotion *getNewGroundMotion(int classTag);
    TimeSeries *getNewTimeSeries(int classTag);    
    TimeSeriesIntegrator *getNewTimeSeriesIntegrator(int classTag);    
    
    // matrix vector and id objects
    Matrix *getPtrNewMatrix(int classTag, int noRows, int noCols);
    Vector *getPtrNewVector(int classTag, int size);
    ID *getPtrNewID(int classTag, int size);

    // methods for output objects
    //    DataOutputHandler *getPtrNewDataOutputHandler(int classTag);
    Recorder *getPtrNewRecorder(int classTag);
    
    
    // methods to get new analysis objects
    //Accelerator *getAccelerator(int classTag);

    DomainSolver *getNewDomainSolver(void);

    Subdomain *getSubdomainPtr(int classTag);

    Parameter *getParameter(int classTag);

    int addUniaxialMaterial(int classTag, const char *lib, const char *funcName, UniaxialMaterial *(*)(void));
  };
} // end of XC namespace

#endif


