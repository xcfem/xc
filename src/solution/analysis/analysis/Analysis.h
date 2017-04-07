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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/Analysis.h,v $
                                                                        
                                                                        
#ifndef Analysis_h
#define Analysis_h

// File: ~/analysis/analysis/Analysis.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the interface for the Analysis class.
// Analysis is an abstract class, i.e. no objects of it's type can be created. 
//
// What: "@(#) Analysis.h, revA"

#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {

class SoluMethod;
class ProcSolu;

class Domain;
class AnalysisModel;
class ConstraintHandler;
class DOF_Numberer;

class Integrator;
class IncrementalIntegrator;
class EigenIntegrator;
class LinearBucklingIntegrator;
class TransientIntegrator;
class StaticIntegrator;

class LinearSOE;
class EigenSOE;

class SolutionAlgorithm;
class EigenAlgorithm;
class EquiSolnAlgo;
class DomainDecompAlgo;
class DomainSolver;
class Subdomain;

class ConvergenceTest;

class FEM_ObjectBroker;
class ID;

class CommParameters;

//! @ingroup Analysis
//
//! @defgroup AnalysisType Analysis type (static, dynamic, eigenvalues, modal, ...).
//
//! @ingroup AnalysisType
//
//! @brief Clase Base para los objetos que realizan the analysis.
class Analysis: public EntCmd
  {
  protected:
    int analysis_result; //!< Equal to zero if success.
    SoluMethod *metodo_solu; //!< Solution method.

    int newStepDomain(AnalysisModel *theModel,const double &dT =0.0);
    ProcSolu *getProcSolu(void);
    const ProcSolu *getProcSolu(void) const;    

    friend class ProcSolu;
    Analysis(SoluMethod *metodo);
    virtual Analysis *getCopy(void) const= 0;
  public:
    inline virtual ~Analysis(void) {}

    inline SoluMethod *getSoluMethodPtr(void)
      { return metodo_solu; }
    Domain *getDomainPtr(void);
    const Domain *getDomainPtr(void) const;
    ConstraintHandler *getConstraintHandlerPtr(void);
    DOF_Numberer *getDOF_NumbererPtr(void) const;
    AnalysisModel *getAnalysisModelPtr(void) const;

    LinearSOE *getLinearSOEPtr(void) const;
    EigenSOE *getEigenSOEPtr(void) const;

    Integrator *getIntegratorPtr(void);
    const Integrator *getIntegratorPtr(void) const;
    IncrementalIntegrator *getIncrementalIntegratorPtr(void);
    EigenIntegrator *getEigenIntegratorPtr(void);
    LinearBucklingIntegrator *getLinearBucklingIntegratorPtr(void);
    TransientIntegrator *getTransientIntegratorPtr(void);
    StaticIntegrator *getStaticIntegratorPtr(void);

    SolutionAlgorithm *getSolutionAlgorithmPtr(void);
    EigenAlgorithm *getEigenSolutionAlgorithmPtr(void);
    EquiSolnAlgo *getEquiSolutionAlgorithmPtr(void);
    DomainDecompAlgo *getDomainDecompSolutionAlgorithmPtr(void);

    virtual ConvergenceTest *getConvergenceTestPtr(void);
    virtual const ConvergenceTest *getConvergenceTestPtr(void) const;

    virtual const DomainSolver *getDomainSolver(void) const;
    virtual DomainSolver *getDomainSolver(void);
    virtual const Subdomain *getSubdomain(void) const;
    virtual Subdomain *getSubdomain(void);

    // pure virtual functions
    virtual int domainChanged(void) = 0;
    virtual void clearAll(void);
    virtual int setNumberer(DOF_Numberer &theNumberer);
    virtual int setLinearSOE(LinearSOE &theSOE); 
    virtual int setEigenSOE(EigenSOE &theSOE);
    virtual int setIntegrator(Integrator &theNewIntegrator);
    virtual int setAlgorithm(SolutionAlgorithm &theNewAlgorithm);

    void brokeConstraintHandler(const CommParameters &,const ID &);
    void brokeNumberer(const CommParameters &,const ID &);
    void brokeAnalysisModel(const CommParameters &,const ID &);

    void brokeDDLinearSOE(const CommParameters &,const ID &);
    void brokeLinearSOE(const CommParameters &,const ID &);

    void brokeIncrementalIntegrator(const CommParameters &,const ID &);
    void brokeStaticIntegrator(const CommParameters &,const ID &);
    void brokeTransientIntegrator(const CommParameters &,const ID &);

    void brokeDomainDecompAlgo(const CommParameters &,const ID &);
    void brokeEquiSolnAlgo(const CommParameters &,const ID &);

    inline int getAnalysisResult(void) const
      { return analysis_result; }

  };
} // end of XC namespace

#endif

