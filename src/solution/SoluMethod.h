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
//SoluMethod.h

#ifndef SOLUMETHOD_H
#define SOLUMETHOD_H

#include "xc_utils/src/nucleo/EntCmd.h"
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

//!  \ingroup Solu
//! 
//! @brief Solution procedure for the finite element problem. The
//! solution procedure is definde by specifiying:
//! - Wrapper for the finite element model.
//! - Solution algorithm.
//! - Integration scheme.
//! - Solver for the system of eauqtions.
//  - Convergence test.
class SoluMethod: public EntCmd
  {
    ModelWrapper *base; //!< Wrapper for the finite elemnt model.
    SolutionAlgorithm *theSolnAlgo; //!< Solution algorithm.
    Integrator *theIntegrator; //!< Integration scheme.
    SystemOfEqn *theSOE; //!< System of equations.
    ConvergenceTest *theTest; //!< Convergence test.

    Analysis *getAnalysis(void);
    const Analysis *getAnalysis(void) const;    
  protected:
    friend class ProblemaEF;
    void libera_soln_algo(void);
    bool alloc_soln_algo(const std::string &);
    void copia_soln_algo(SolutionAlgorithm *);

    void libera_integrator(void);
    bool alloc_integrator(const std::string &,const Vector &);
    void copia_integrator(Integrator *);

    void libera_system_of_equations(void);
    bool alloc_system_of_equations(const std::string &,AnalysisModel *);
    void copia_system_of_equations(SystemOfEqn *);

    void libera_conv_test(void);
    bool alloc_conv_test(const std::string &);
    void copia_conv_test(ConvergenceTest *);

    void libera(void);
    void copia(const SoluMethod &);

  public:
    SoluMethod(Analysis *a=NULL,ModelWrapper *b= NULL);
    SoluMethod(const SoluMethod &);
    SoluMethod &operator=(const SoluMethod &);
    ~SoluMethod(void);

    inline ModelWrapper *getModelWrapperPtr(void)
      { return base; }
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

    void brokeDDLinearSOE(const CommParameters &,const ID &);
    void brokeLinearSOE(const CommParameters &,const ID &);

    void brokeIncrementalIntegrator(const CommParameters &,const ID &);
    void brokeStaticIntegrator(const CommParameters &,const ID &);
    void brokeTransientIntegrator(const CommParameters &,const ID &);

    void brokeDomainDecompAlgo(const CommParameters &,const ID &);
    void brokeEquiSolnAlgo(const CommParameters &,const ID &);
    bool CheckPointers(void);
    void revertToStart(void);

    void clearAll(void);
  };

} // end of XC namespace

#endif
