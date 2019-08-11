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
//  software es libre: usted puede redistribuirlo y/o modificarlo
//  under the terms of the GNU General Public License published by 
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO.
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// junto a este programa.
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
// $Date: 2005/11/22 19:40:05 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/objectBroker/FEM_ObjectBroker.h,v $


// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for FEM_ObjectBroker.
// FEM_ObjectBroker is is an object broker class for the finite element
// method. All methods are virtual to allow for subclasses; which can be
// used by programmers when introducing new subclasses of the main objects.
//
// What: "@(#) FEM_ObjectBroker.h, revA"

#ifndef FEM_ObjectBroker_h
#define FEM_ObjectBroker_h

#include "utility/actor/objectBroker/ObjectBroker.h"
class CommandEntity;

namespace XC {
class Channel;

class GraphNumberer;
class Vertex;

class Element;
class Node;
class MFreedom_Constraint;
class MRMFreedom_Constraint;
class SFreedom_Constraint;
class NodeLocker;
class NodalLoad;
class ElementalLoad;
class ElementEdgeLoad;
class LoadPattern;
class LoadCombination;
class TimeSeries;
class TimeSeriesIntegrator;

class Matrix;
class Vector;
class ID;
class Subdomain;
class ConstraintHandler;
class DOF_Numberer;
class AnalysisModel;
class EquiSolnAlgo;
class DomainDecompAlgo;
class StaticIntegrator;
class TransientIntegrator;
class IncrementalIntegrator;

class LinearSOE;
class EigenSOE;
class DomainSolver;
class DomainDecompositionAnalysis;
class LinearSOESolver;
class PartitionedModelBuilder;
class LineSearch;

class CrdTransf;
class CrdTransf2d;
class CrdTransf3d;
class ShellCrdTransf3dBase;

class BeamIntegration;

class UniaxialMaterial;
class SectionForceDeformation;
class NDMaterial;
class Material;
class Fiber;
class FrictionModel;

class ConvergenceTest;
class SectionForceDeformation;
class GroundMotion;

class DataOutputHandler;
class Recorder;

class Actor;

class Parameter;

//! @ingroup IPComm
//
//! @brief FEM_ObjectBroker is is an object broker class for the finite element
//! method. All methods are virtual to allow for subclasses; which can be
//! used by programmers when introducing new subclasses of the main objects.
//!
//! object used to create a new blank 
//! of a certain type in a process. The explicit type of object
//! created depends on the method invoked and the integer classTag
//! passed as an argument to the method. Once the object has been created, 
//! recvSelf() can be invoked on the object to instantiate the object
//! with it's data.
class FEM_ObjectBroker
  {
  private:
    LinearSOESolver *lastLinearSolver;
    DomainSolver *lastDomainSolver;
  public:
    FEM_ObjectBroker();

    virtual Actor* getNewActor(int classTag, Channel *theChannel);

    virtual PartitionedModelBuilder *getPtrNewPartitionedModelBuilder(Subdomain &theSub, int classTag);

    virtual GraphNumberer *getPtrNewGraphNumberer(int classTag);
    virtual Vertex *getNewVertex(int classTag);

    // methods to get new modelling class objects
    virtual Element *getNewElement(int classTag);
    virtual Node *getNewNode(int classTag);
    virtual MFreedom_Constraint *getNewMP(int classTag);
    virtual MRMFreedom_Constraint *getNewMRMP(int classTag);
    virtual SFreedom_Constraint *getNewSP(int classTag);
    virtual NodalLoad     *getNewNodalLoad(int classTag);
    virtual ElementalLoad *getNewElementalLoad(int classTag);

    virtual CrdTransf2d *getNewCrdTransf2d(int classTag);
    virtual CrdTransf3d *getNewCrdTransf3d(int classTag);
    CrdTransf *getNewCrdTransf(int classTag);
    ShellCrdTransf3dBase *getNewShellCrdTransf3d(int classTag);

    virtual BeamIntegration *getNewBeamIntegration(int classTag);

    virtual UniaxialMaterial  *getNewUniaxialMaterial(int classTag);
    virtual SectionForceDeformation *getNewSection(int classTag);
    virtual NDMaterial *getNewNDMaterial(int classTag);
    virtual Fiber *getNewFiber(int classTag);
    virtual FrictionModel *getNewFrictionModel(int classTag);
    virtual Material *getNewMaterial(int classTag);

    virtual ConvergenceTest *getNewConvergenceTest(CommandEntity *owr,int classTag);
    virtual NodeLocker *getNewNodeLocker(int classTag);
    virtual LoadPattern *getNewLoadPattern(int classTag);
    virtual LoadCombination *getNewLoadCombination(int classTag);
    virtual GroundMotion *getNewGroundMotion(int classTag);
    virtual TimeSeries  *getNewTimeSeries(int classTag);
    virtual TimeSeriesIntegrator  *getNewTimeSeriesIntegrator(int classTag);

    // matrix vector and id objects
    virtual Matrix	  *getPtrNewMatrix(int classTag, int noRows, int noCols);
    virtual Vector	  *getPtrNewVector(int classTag, int size);
    virtual ID	          *getPtrNewID(int classTag, int size);

    // methods for output objects
    virtual DataOutputHandler *getPtrNewDataOutputHandler(int classTag);
    virtual Recorder *getPtrNewRecorder(int classTag);


    // methods to get new analysis objects
    virtual ConstraintHandler *getNewConstraintHandler(int classTag);
    virtual DOF_Numberer *getNewNumberer(int classTag);
    virtual AnalysisModel *getNewAnalysisModel(int classTag);
    virtual LineSearch *getLineSearch(int classTag);
    virtual EquiSolnAlgo *getNewEquiSolnAlgo(int classTag);
    virtual DomainDecompAlgo *getNewDomainDecompAlgo(int classTag);
    virtual StaticIntegrator *getNewStaticIntegrator(int classTag);
    virtual TransientIntegrator *getNewTransientIntegrator(int classTag);
    virtual IncrementalIntegrator *getNewIncrementalIntegrator(int classTag);

    virtual LinearSOE *getNewLinearSOE(int classTagSOE, int classTagSolver);
    virtual EigenSOE *getNewEigenSOE(int classTagSOE);
    virtual LinearSOESolver *getNewLinearSolver(void);

    virtual LinearSOE *getPtrNewDDLinearSOE(int classTagSOE,int classTagDDSolver);
    virtual DomainSolver *getNewDomainSolver(void);

    virtual DomainDecompositionAnalysis *getNewDomainDecompAnalysis(int classTag, Subdomain &theDomain);

    virtual Subdomain *getSubdomainPtr(int classTag);


    virtual int addUniaxialMaterial(int classTag, const char *lib, const char *funcName, UniaxialMaterial *(*)(void));
  };
} // end of XC namespace

#endif


