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
//! @brief Procedimiento de solución del problema de elementos finitos. Queda
//! definido a través de la especificación del modelo de solución (ModelWrapper),
//! el algoritmo de solución (SolutionAlgorithm), el integrador (theIntegrator)
//! el sistema de ecuaciones (SystemOfEqn) y el test de convergencia (ConvergenceTest)
class SoluMethod: public EntCmd
  {
    ModelWrapper *base; //!< Modelo de solución.
    SolutionAlgorithm *theSolnAlgo; //!< Algoritmo de solución.
    Integrator *theIntegrator; //!< Integrador.
    SystemOfEqn *theSOE; //!< Sistema de ecuaciones.
    ConvergenceTest *theTest; //!< Criterio de convergencia.

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

    void libera_sistema_ecuaciones(void);
    bool alloc_sistema_ecuaciones(const std::string &,AnalysisModel *);
    void copia_sistema_ecuaciones(SystemOfEqn *);

    void libera_conv_test(void);
    bool alloc_conv_test(const std::string &);
    void copia_conv_test(ConvergenceTest *);

    void libera(void);
    void copia(const SoluMethod &);

    bool procesa_cmd_soln_algo(const std::string &cmd,CmdStatus &status);
    bool procesa_cmd_integrator(const std::string &cmd,CmdStatus &status);
    bool procesa_cmd_sistema_ecuaciones(const std::string &cmd,CmdStatus &status);
    bool procesa_cmd_conv_test(const std::string &cmd,CmdStatus &status);
    bool procesa_comando(CmdStatus &status);
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

    //! @brief Devuelve un puntero al integrador.
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

    //! @brief Devuelve un puntero al algoritmo de solución.
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
