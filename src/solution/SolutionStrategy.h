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
//SolutionStrategy.h

#ifndef ANALYSISAGGREGATION_H
#define ANALYSISAGGREGATION_H

#include "utility/kernel/CommandEntity.h"
#include "utility/handler/DataOutputHandler.h"

namespace XC {

class Domain;

class AnalysisModel;
class ConstraintHandler;
class DOF_Numberer;

class SolutionAlgorithm;
class EigenAlgorithm;
class EquiSolnAlgo;
class DomainDecompAlgo;

class Integrator;
class IncrementalIntegrator;
class EigenIntegrator;
class LinearBucklingIntegrator;
class TransientIntegrator;
class StaticIntegrator;

class SystemOfEqn;
class LinearSOE;
class EigenSOE;

class ConvergenceTest;
class ModelWrapper;

class Analysis;

class FEM_ObjectBroker;
class ID;

class SolutionStrategyMap;

//! @brief Solution strategy for the finite element problem.
//! 
//! Solution strategy for the finite element problem. The
//! solution strategy is defined by specifying:
//! - Wrapper for the finite element model. Container for the finite
//!   element model "as seen" from the solver.
//! - Solution algorithm.  An algorithmic class specifying the
//!   sequence of operations to be performed in setting up and solving the
//!   finite element equation which can be represented by the equation K(U)
//!   U = P(U).
//! - Integration scheme. An algorithmic object which provides
//!   methods which are invoked by the FE\_Element to determine their
//!   current tangent and residual matrices; that is this is the object that
//!   sets up the system of equations.  It also provides the {\em
//!   update()} method which is invoked to set up the appropriate dof
//!   response values once the solution algorithm has formed and solved the
//!   system of equations.
//! - System of equations. A numeric object responsible for the creation
//!   and subsequent solution of large systems of linear equations of the
//!   form \f$Ax = b\f$, where \f$A\f$ is a matrix and \f$x\f$
//!   and \f$b\f$ are vectors.
//! - Convergence test.
//! @ingroup Solu
class SolutionStrategy: public CommandEntity
  {
    ModelWrapper *theModelWrapper; //!< Wrapper for the finite element model.
    SolutionAlgorithm *theSolnAlgo; //!< Solution algorithm.
    Integrator *theIntegrator; //!< Integration scheme.
    SystemOfEqn *theSOE; //!< System of equations.
    ConvergenceTest *theTest; //!< Convergence test.

    Analysis *getAnalysis(void);
    const Analysis *getAnalysis(void) const;    
  protected:
    friend class FEProblem;
    void free_soln_algo(void);
    bool alloc_soln_algo(const std::string &);
    void copy_soln_algo(SolutionAlgorithm *);

    void free_integrator(void);
    bool alloc_integrator(const std::string &,const Vector &);
    void copy_integrator(Integrator *);

    void free_system_of_equations(void);
    bool alloc_system_of_equations(const std::string &,AnalysisModel *);
    void copy_system_of_equations(SystemOfEqn *);

    void free_conv_test(void);
    bool alloc_conv_test(const std::string &);
    void copy_conv_test(ConvergenceTest *);

    void free_mem(void);
    void copy(const SolutionStrategy &);

  public:
    SolutionStrategy(Analysis *a= nullptr,ModelWrapper *b= nullptr);
    SolutionStrategy(const SolutionStrategy &);
    SolutionStrategy &operator=(const SolutionStrategy &);
    ~SolutionStrategy(void);

    const SolutionStrategyMap *getSolutionStrategyMap(void) const;
    SolutionStrategyMap *getSolutionStrategyMap(void);
    std::string getName(void) const;
    const std::string &getModelWrapperName(void) const;

    inline ModelWrapper *getModelWrapperPtr(void)
      { return theModelWrapper; }
    Domain *getDomainPtr(void);
    const Domain *getDomainPtr(void) const;
    ConstraintHandler *getConstraintHandlerPtr(void);
    DOF_Numberer *getDOF_NumbererPtr(void) const;
    AnalysisModel *getAnalysisModelPtr(void) const;

    LinearSOE *getLinearSOEPtr(void);
    const LinearSOE *getLinearSOEPtr(void) const;
    EigenSOE *getEigenSOEPtr(void);
    const EigenSOE *getEigenSOEPtr(void) const;
    Integrator &newIntegrator(const std::string &, const Vector &);

    //! @brief Returns a pointer to the integrator.
    inline Integrator *getIntegratorPtr(void)
      { return theIntegrator; }
    inline const Integrator *getIntegratorPtr(void) const
      { return theIntegrator; }
    IncrementalIntegrator *getIncrementalIntegratorPtr(void);
    EigenIntegrator *getEigenIntegratorPtr(void);
    LinearBucklingIntegrator *getLinearBucklingIntegratorPtr(void);
    TransientIntegrator *getTransientIntegratorPtr(void);
    StaticIntegrator *getStaticIntegratorPtr(void);
    SystemOfEqn &newSystemOfEqn(const std::string &);

    //! @brief Returns a pointer to the solution algorithm.
    inline SolutionAlgorithm *getSolutionAlgorithmPtr(void)
      { return theSolnAlgo; }
    inline const SolutionAlgorithm *getSolutionAlgorithmPtr(void) const
      { return theSolnAlgo; }
    EigenAlgorithm *getEigenSolutionAlgorithmPtr(void);
    EquiSolnAlgo *getEquiSolutionAlgorithmPtr(void);
    DomainDecompAlgo *getDomainDecompSolutionAlgorithmPtr(void);
    SolutionAlgorithm &newSolutionAlgorithm(const std::string &);

    ConvergenceTest *getConvergenceTestPtr(void);
    const ConvergenceTest *getConvergenceTestPtr(void) const;
    ConvergenceTest &newConvergenceTest(const std::string &);

    virtual const DomainSolver *getDomainSolverPtr(void) const;
    virtual DomainSolver *getDomainSolverPtr(void);
    virtual const Subdomain *getSubdomainPtr(void) const;
    virtual Subdomain *getSubdomainPtr(void);

    int setLinearSOE(LinearSOE &theSOE); 
    int setEigenSOE(EigenSOE &theSOE);
    int setIntegrator(Integrator &theNewIntegrator);
    int setAlgorithm(SolutionAlgorithm &theNewAlgorithm);
    int setConvergenceTest(ConvergenceTest &theNewTest);

    void brokeDDLinearSOE(const Communicator &,const ID &);
    void brokeLinearSOE(const Communicator &,const ID &);

    void brokeIncrementalIntegrator(const Communicator &,const ID &);
    void brokeStaticIntegrator(const Communicator &,const ID &);
    void brokeTransientIntegrator(const Communicator &,const ID &);

    void brokeDomainDecompAlgo(const Communicator &,const ID &);
    void brokeEquiSolnAlgo(const Communicator &,const ID &);
    bool CheckPointers(void);
    void revertToStart(void);

    void clearAll(void);
  };

} // end of XC namespace

#endif
