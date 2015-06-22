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
    MP_Constraint *getNewMP(int classTag);
    SP_Constraint *getNewSP(int classTag);
    NodalLoad *getNewNodalLoad(int classTag);
    
    BeamIntegration *getNewBeamIntegration(int classTag);

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

    // methods for ouput objects
    //    DataOutputHandler *getPtrNewDataOutputHandler(int classTag);
    Recorder *getPtrNewRecorder(int classTag);
    
    
    // methods to get new analysis objects
    //Accelerator *getAccelerator(int classTag);

    DomainSolver *getNewDomainSolver(void);

    Subdomain *getSubdomainPtr(int classTag);

    Parameter *getParameter(int classTag);

    int addUniaxialMaterial(int classTag, const char *lib, const char *funcName, UniaxialMaterial *(*)(void));
  };
} // fin namespace XC

#endif


