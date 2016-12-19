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
class EntCmd;

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
class Combinacion;
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

class FEM_ObjectBroker
  {
  private:
    LinearSOESolver *lastLinearSolver;
    DomainSolver *lastDomainSolver;
  public:
    FEM_ObjectBroker();

    virtual Actor*getNewActor(int classTag, Channel *theChannel);

    virtual PartitionedModelBuilder *
      getPtrNewPartitionedModelBuilder(Subdomain &theSub,
				       int classTag);

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

    virtual ConvergenceTest *getNewConvergenceTest(EntCmd *owr,int classTag);
    virtual NodeLocker *getNewNodeLocker(int classTag);
    virtual LoadPattern *getNewLoadPattern(int classTag);
    virtual Combinacion *getNewCombinacion(int classTag);
    virtual GroundMotion *getNewGroundMotion(int classTag);
    virtual TimeSeries  *getNewTimeSeries(int classTag);
    virtual TimeSeriesIntegrator  *getNewTimeSeriesIntegrator(int classTag);

    // matrix vector and id objects
    virtual Matrix	  *getPtrNewMatrix(int classTag, int noRows, int noCols);
    virtual Vector	  *getPtrNewVector(int classTag, int size);
    virtual ID	          *getPtrNewID(int classTag, int size);

    // methods for ouput objects
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


