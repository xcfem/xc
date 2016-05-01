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
//SoluMethod.cc

#include "SoluMethod.h"
#include "analysis/ModelWrapper.h"


//Gestor coacciones.
#include <solution/analysis/handler/ConstraintHandler.h>

//Integradores.
#include "solution/analysis/integrator/integrators.h"

//Algoritmo de solución.
#include "solution/analysis/algorithm/solution_algorithms.h"

//Sistemas de ecuaciones
#include "solution/system_of_eqn/systems_of_equations.h"

//Criterios de convergencia.
#include "solution/analysis/convergenceTest/convergence_tests.h"

//Analysis
#include <solution/analysis/analysis/Analysis.h>
#include <solution/analysis/analysis/EigenAnalysis.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/analysis/DirectIntegrationAnalysis.h>
#include <solution/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.h>


#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "utility/matrix/ID.h"

#include "boost/any.hpp"

void XC::SoluMethod::libera_soln_algo(void)
  {
    if(theSolnAlgo)
      {
        delete theSolnAlgo;
        theSolnAlgo= nullptr;
      }
  }

bool XC::SoluMethod::alloc_soln_algo(const std::string &nmb)
  {
    libera_soln_algo();

    if(nmb=="bfgs_soln_algo")
      theSolnAlgo=new BFGS(this);
    else if(nmb=="broyden_soln_algo")
      theSolnAlgo=new Broyden(this);
    else if(nmb=="krylov_newton_soln_algo")
      theSolnAlgo=new KrylovNewton(this);
    else if(nmb=="linear_soln_algo")
      theSolnAlgo=new Linear(this);
    else if(nmb=="modified_newton_soln_algo")
      theSolnAlgo=new ModifiedNewton(this);
    else if(nmb=="newton_raphson_soln_algo")
      theSolnAlgo=new NewtonRaphson(this);
    else if(nmb=="newton_line_search_soln_algo")
      theSolnAlgo=new NewtonLineSearch(this);
    else if(nmb=="periodic_newton_soln_algo")
      theSolnAlgo=new PeriodicNewton(this);
    else if(nmb=="frequency_soln_algo")
      theSolnAlgo=new FrequencyAlgo(this);
    else if(nmb=="standard_eigen_soln_algo")
      theSolnAlgo=new StandardEigenAlgo(this);
    else if(nmb=="linear_buckling_soln_algo")
      theSolnAlgo=new LinearBucklingAlgo(this);
    else
      std::cerr << "Solution algorithm: '"
                << nmb << "' unknown." << std::endl;
    if(theSolnAlgo)
      theSolnAlgo->set_owner(this);

    return (theSolnAlgo!=nullptr);
  }

void XC::SoluMethod::copia_soln_algo(SolutionAlgorithm *ptr)
  {
    if(ptr)
      {
        libera_soln_algo();
        theSolnAlgo= ptr->getCopy();
        theSolnAlgo->set_owner(this);
      }
    else
     std::cerr << "SoluMethod::copia_soln_algo; se pasó un puntero nulo." << std::endl;
  }

//! @brief Define un nuevo algortitmo de solución.
XC::SolutionAlgorithm &XC::SoluMethod::newSolutionAlgorithm(const std::string &tipo)
  {
    alloc_soln_algo(tipo);
    assert(theSolnAlgo);
    return *theSolnAlgo;
  }

void XC::SoluMethod::libera_integrator(void)
  {
    if(theIntegrator)
      {
        delete theIntegrator;
        theIntegrator= nullptr;
      }
  }

bool XC::SoluMethod::alloc_integrator(const std::string &nmb,const Vector &params)
  {
    libera_integrator();

    if(nmb=="arc_length_integrator")
      {
        double arc_length= 1.0;
        if(params.Size()>0)
          arc_length= params[0];
        double alpha= 1.0;
        if(params.Size()>1)
          alpha= params[1];
        theIntegrator=new ArcLength(this,arc_length,alpha);
      }
    else if(nmb=="arc_length1_integrator")
      {
        double arc_length= 1.0;
        if(params.Size()>0)
          arc_length= params[0];
        double alpha= 1.0;
        if(params.Size()>1)
          alpha= params[1];
        theIntegrator=new ArcLength1(this,arc_length,alpha);
      }
    else if(nmb=="displacement_control_integrator")
      {
        theIntegrator=new DisplacementControl(this);
      }
    else if(nmb=="distributed_displacement_control_integrator")
      theIntegrator=new DistributedDisplacementControl(this);
    else if(nmb=="HS_constraint_integrator")
      {
        double arc_length= 1.0;
        if(params.Size()>0)
          arc_length= params[0];
        double psi_u= 1.0;
        if(params.Size()>1)
          psi_u= params[1];
        double psi_f= 1.0;
        if(params.Size()>2)
          psi_f= params[2];
        double u_ref= 1.0;
        if(params.Size()>3)
          u_ref= params[3];
        theIntegrator=new HSConstraint(this,arc_length,psi_u,psi_f,u_ref);
      }
    else if(nmb=="load_control_integrator")
      { theIntegrator=new LoadControl(this); }
    else if(nmb=="load_path_integrator")
      theIntegrator=new LoadPath(this);
    else if(nmb=="min_unbal_disp_norm_integrator")
      {
        double lambda1= 1.0;
        if(params.Size()>0)
          lambda1= params[0];
        int specnum_iter_step= 1;
        if(params.Size()>1)
          specnum_iter_step= params[1];
        double dlambda1min= 1.0;
        if(params.Size()>2)
          dlambda1min= params[2];
        double dlambda1max= 1.0;
        if(params.Size()>3)
          dlambda1max= params[3];
        theIntegrator=new MinUnbalDispNorm(this,lambda1,specnum_iter_step,dlambda1min,dlambda1max);
      }
    else if(nmb=="eigen_integrator")
      theIntegrator=new EigenIntegrator(this);
    else if(nmb=="linear_buckling_integrator")
      theIntegrator=new LinearBucklingIntegrator(this);
    else if(nmb=="alpha_os_integrator")
      theIntegrator=new AlphaOS(this);
    else if(nmb=="alpha_os_generalized_integrator")
      theIntegrator=new AlphaOSGeneralized(this);
    else if(nmb=="central_difference_integrator")
      theIntegrator=new CentralDifference(this);
    else if(nmb=="central_difference_alternative_integrator")
      theIntegrator=new CentralDifferenceAlternative(this);
    else if(nmb=="central_difference_no_damping_integrator")
      theIntegrator=new CentralDifferenceNoDamping(this);
    else if(nmb=="collocation_integrator")
      theIntegrator=new Collocation(this);
    else if(nmb=="collocation_hybrid_simulation_integrator")
      theIntegrator=new CollocationHybridSimulation(this);
    else if(nmb=="HHT_integrator")
      theIntegrator=new HHT(this);
    else if(nmb=="HHT1_integrator")
      theIntegrator=new HHT1(this);
    else if(nmb=="HHT_explicit_integrator")
      theIntegrator=new HHTExplicit(this);
    else if(nmb=="HHT_generalized_integrator")
      theIntegrator=new HHTGeneralized(this);
    else if(nmb=="HHT_generalized_explicit_integrator")
      theIntegrator=new HHTGeneralizedExplicit(this);
    else if(nmb=="HHT_hybrid_simulation_integrator")
      theIntegrator=new HHTHybridSimulation(this);
    else if(nmb=="newmark_integrator")
      theIntegrator=new Newmark(this,.5,.25);
    else if(nmb=="newmark1_integrator")
      theIntegrator=new Newmark1(this,.5,.25);
    else if(nmb=="newmark_explicit_integrator")
      theIntegrator=new NewmarkExplicit(this,.5);
    else if(nmb=="newmark_hybrid_simulation_integrator")
      theIntegrator=new NewmarkHybridSimulation(this);
    else if(nmb=="wilson_theta_integrator")
      theIntegrator=new WilsonTheta(this);
    else
      std::cerr << "Integrator: '"
                << nmb << "' unknown." << std::endl;

    if(theIntegrator)
      theIntegrator->set_owner(this);

    return (theIntegrator!=nullptr);
  }

void XC::SoluMethod::copia_integrator(Integrator *ptr)
  {
    if(ptr)
      {
        libera_integrator();
        theIntegrator= ptr->getCopy();
        theIntegrator->set_owner(this);
      }
    else
     std::cerr << "SoluMethod::copia_integrator; se pasó un puntero nulo." << std::endl;
  }

//! @brief Define un nuevo integrador.
XC::Integrator &XC::SoluMethod::newIntegrator(const std::string &tipo, const Vector &params)
  {
    if(alloc_integrator(tipo,params))
      {
        DisplacementControl *dtmp= dynamic_cast<DisplacementControl *>(theIntegrator);
        if(dtmp)
          dtmp->set_owner(this);
      }
    assert(theIntegrator);
    return *theIntegrator;
  }

void XC::SoluMethod::libera_sistema_ecuaciones(void)
  {
    if(theSOE)
      {
        delete theSOE;
        theSOE= nullptr;
      }
  }
bool XC::SoluMethod::alloc_sistema_ecuaciones(const std::string &nmb,AnalysisModel *theModel)
  {
    libera_sistema_ecuaciones();
    if(nmb=="band_arpack_soe")
      theSOE=new BandArpackSOE(this);
    else if(nmb=="band_arpackpp_soe")
      theSOE=new BandArpackppSOE(this);
    else if(nmb=="sym_arpack_soe")
      theSOE=new SymArpackSOE(this);
    else if(nmb=="sym_band_eigen_soe")
      theSOE=new SymBandEigenSOE(this);
    else if(nmb=="full_gen_eigen_soe")
      theSOE=new FullGenEigenSOE(this);
    else if(nmb=="band_gen_lin_soe")
      theSOE=new BandGenLinSOE(this);
    else if(nmb=="distributed_band_gen_lin_soe")
      theSOE=new DistributedBandGenLinSOE(this);
    else if(nmb=="band_spd_lin_soe")
      theSOE=new BandSPDLinSOE(this);
    else if(nmb=="distributed_band_spd_lin_soe")
      theSOE=new DistributedBandSPDLinSOE(this);
    else if(nmb=="diagonal_soe")
      theSOE=new DiagonalSOE(this);
    else if(nmb=="distributed_diagonal_soe")
      theSOE=new DistributedDiagonalSOE(this);
    else if(nmb=="full_gen_lin_soe")
      theSOE=new FullGenLinSOE(this);
//     else if(nmb=="itpack_lin_soe")
//       theSOE=new ItpackLinSOE(this);
    else if(nmb=="profile_spd_lin_soe")
      theSOE=new ProfileSPDLinSOE(this);
    else if(nmb=="distributed_profile_spd_lin_soe")
      theSOE=new DistributedProfileSPDLinSOE(this);
    else if(nmb=="sparse_gen_col_lin_soe")
      theSOE=new SparseGenColLinSOE(this);
    else if(nmb=="distributed_sparse_gen_col_lin_soe")
      theSOE=new DistributedSparseGenColLinSOE(this);
    else if(nmb=="sparse_gen_row_lin_soe")
      theSOE=new SparseGenRowLinSOE(this);
    else if(nmb=="distributed_sparse_gen_row_lin_soe")
      theSOE=new DistributedSparseGenRowLinSOE(this);
    else if(nmb=="sym_sparse_lin_soe")
      theSOE =new SymSparseLinSOE(this);
//     else if(nmb=="umfpack_gen_lin_soe")
//       theSOE =new UmfpackGenLinSOE();
    else
      std::cerr << "Syztem of equations: '"
                << nmb << "' unknown." << std::endl;

    if(theSOE)
      theSOE->set_owner(this);
    return (theSOE!=nullptr);
  }

void XC::SoluMethod::copia_sistema_ecuaciones(SystemOfEqn *ptr)
  {
    if(ptr)
      {
        libera_sistema_ecuaciones();
        theSOE= ptr->getCopy();
        theSOE->set_owner(this);
      }
    else
     std::cerr << "SoluMethod::copia_sistema_ecuaciones; se pasó un puntero nulo." << std::endl;
  }

//! @brief Define el tipo de sistema de ecuaciones a emplear en la solución.
XC::SystemOfEqn &XC::SoluMethod::newSystemOfEqn(const std::string &tipo)
  {
    AnalysisModel *theModel= nullptr;
    if(base)
      {
        theModel= base->getAnalysisModelPtr();
        alloc_sistema_ecuaciones(tipo,theModel);
      }
    else
      std::cerr << "No está definido el modelo de cálculo." << std::endl;
    assert(theSOE);
    return *theSOE;
  }

void XC::SoluMethod::libera_conv_test(void)
  {
    if(theTest)
      {
        delete theTest;
        theTest= nullptr;
      }
  }

bool XC::SoluMethod::alloc_conv_test(const std::string &nmb)
  {
    libera_conv_test();
    if(nmb== "energy_inc_conv_test")
      theTest= new CTestEnergyIncr(this);
    else if(nmb== "fixed_num_iter_conv_test")
      theTest= new CTestFixedNumIter(this);
    else if(nmb== "norm_disp_incr_conv_test")
      theTest= new CTestNormDispIncr(this);
    else if(nmb== "norm_unbalance_conv_test")
      theTest= new CTestNormUnbalance(this);
    else if(nmb== "relative_energy_incr_conv_test")
      theTest= new CTestRelativeEnergyIncr(this);
    else if(nmb== "relative_norm_disp_incr_conv_test")
      theTest= new CTestRelativeNormDispIncr(this);
    else if(nmb== "relative_norm_unbalance_conv_test")
      theTest= new CTestRelativeNormUnbalance(this);
    else if(nmb== "relative_total_norm_disp_incr_conv_test")
      theTest= new CTestRelativeTotalNormDispIncr(this);
    else
      std::cerr << "Convergence test type: '"
                << nmb << "' unknown." << std::endl;
    if(theTest)
      theTest->set_owner(this);
    return (theTest!=nullptr);
  }

void XC::SoluMethod::copia_conv_test(ConvergenceTest *ptr)
  {
    if(ptr)
      {
        libera_conv_test();
        theTest= ptr->getCopy();
        theTest->set_owner(this);
      }
    else
     std::cerr << "SoluMethod::copia_conv_test; se pasó un puntero nulo." << std::endl;
  }

//! @brief Define el test de convergencia a emplear en la solución.
XC::ConvergenceTest &XC::SoluMethod::newConvergenceTest(const std::string &cmd)
  {
    alloc_conv_test(cmd);
    assert(theTest);
    return *theTest;
  }

void XC::SoluMethod::libera(void)
  {
    libera_soln_algo();
    libera_integrator();
    libera_sistema_ecuaciones();
    libera_conv_test();
  }

void XC::SoluMethod::copia(const SoluMethod &otro)
  {
    if(otro.theSolnAlgo) copia_soln_algo(otro.theSolnAlgo);
    if(otro.theIntegrator) copia_integrator(otro.theIntegrator);
    if(otro.theSOE) copia_sistema_ecuaciones(otro.theSOE);
    if(otro.theTest) copia_conv_test(otro.theTest);
  }

//! @brief Constructor por defecto.
XC::SoluMethod::SoluMethod(Analysis *owr,ModelWrapper *b)
  : EntCmd(owr), base(b), theSolnAlgo(nullptr),theIntegrator(nullptr),
    theSOE(nullptr), theTest(nullptr)
  {
    if(base)
      base->set_owner(this);
  }


//! @brief Constructor de copia.
XC::SoluMethod::SoluMethod(const SoluMethod &otro)
  : EntCmd(otro), base(otro.base), theSolnAlgo(nullptr),theIntegrator(nullptr),
    theSOE(nullptr), theTest(nullptr)
  {
    if(base)
      base->set_owner(this);
    copia(otro);
  }


//! @brief Operador asignación.
XC::SoluMethod &XC::SoluMethod::operator=(const SoluMethod &otro)
  {
    EntCmd::operator=(otro);
    base= otro.base;
    if(base)
      base->set_owner(this);
    copia(otro);
    return *this;
  }


//! @brief Destructor.
XC::SoluMethod::~SoluMethod(void)
  { libera(); }

void XC::SoluMethod::clearAll(void)
  { libera(); }

XC::Analysis *XC::SoluMethod::getAnalysis(void)
  { return dynamic_cast<Analysis *>(Owner()); }

const XC::Analysis *XC::SoluMethod::getAnalysis(void) const
  { return dynamic_cast<const Analysis *>(Owner()); }

//! @brief Devuelve un puntero al dominio.
XC::Domain *XC::SoluMethod::getDomainPtr(void)
  {
    Domain *retval= nullptr;
    Analysis *an= getAnalysis();
    if(an)
      retval= an->getDomainPtr();
    return retval;
  }

//! @brief Devuelve un puntero al dominio.
const XC::Domain *XC::SoluMethod::getDomainPtr(void) const
  {
    const Domain *retval= nullptr;
    const Analysis *an= getAnalysis();
    assert(an);
    if(an)
      retval= an->getDomainPtr();
    return retval;
  }

//! @brief Devuelve un puntero al DomainSolver.
const XC::DomainSolver *XC::SoluMethod::getDomainSolverPtr(void) const
  {
    const Analysis *an= getAnalysis();
    assert(an);
    return an->getDomainSolver();
  }

//! @brief Devuelve un puntero al DomainSolver.
XC::DomainSolver *XC::SoluMethod::getDomainSolverPtr(void)
  {
    Analysis *an= getAnalysis();
    assert(an);
    return an->getDomainSolver();
  }

//! @brief Devuelve un puntero al subdominio.
const XC::Subdomain *XC::SoluMethod::getSubdomainPtr(void) const
  {
    const Analysis *an= getAnalysis();
    assert(an);
    return an->getSubdomain();
  }

//! @brief Devuelve un puntero al subdominio.
XC::Subdomain *XC::SoluMethod::getSubdomainPtr(void)
  {
    Analysis *an= getAnalysis();
    assert(an);
    return an->getSubdomain();
  }

//! @brief Devuelve un puntero al manejador de coacciones.
XC::ConstraintHandler *XC::SoluMethod::getConstraintHandlerPtr(void)
  {
    if(base)
      return base->getConstraintHandlerPtr();
    else
      return nullptr;
  }

//! @brief Devuelve un puntero al renumerador.
XC::DOF_Numberer *XC::SoluMethod::getDOF_NumbererPtr(void) const
  {
    if(base)
      return base->getDOF_NumbererPtr();
    else
      return nullptr;
  }
//! @brief Devuelve un puntero al modelo de análisis.
XC::AnalysisModel *XC::SoluMethod::getAnalysisModelPtr(void) const
  {
    if(base)
      return base->getAnalysisModelPtr();
    else
      return nullptr;
  }


//! @brief Devuelve un puntero al sistema de ecuaciones lineal.
XC::LinearSOE *XC::SoluMethod::getLinearSOEPtr(void)
  {
    LinearSOE *ptr= dynamic_cast<LinearSOE *>(theSOE);
    if(!ptr)
      std::cerr << "SoluMethod; el sistema de ecuaciones no es de tipo LinearSOE." << std::endl;
    return ptr;
  }

//! @brief Devuelve un puntero al sistema de ecuaciones lineal.
const XC::LinearSOE *XC::SoluMethod::getLinearSOEPtr(void) const
  {
    const LinearSOE *ptr= dynamic_cast<const LinearSOE *>(theSOE);
    if(!ptr)
      std::cerr << "SoluMethod; el sistema de ecuaciones no es de tipo LinearSOE." << std::endl;
    return ptr;
  }

//! @brief Devuelve un puntero al sistema de ecuaciones de autovalores.
XC::EigenSOE *XC::SoluMethod::getEigenSOEPtr(void)
  {
    EigenSOE *ptr= dynamic_cast<EigenSOE *>(theSOE);
    if(!ptr)
      std::cerr << "SoluMethod; el sistema de ecuaciones no es de tipo EigenSOE." << std::endl;
    return ptr;
  }

//! @brief Devuelve un puntero al sistema de ecuaciones de autovalores.
const XC::EigenSOE *XC::SoluMethod::getEigenSOEPtr(void) const
  {
    const EigenSOE *ptr= dynamic_cast<const EigenSOE *>(theSOE);
    if(!ptr)
      std::cerr << "SoluMethod; el sistema de ecuaciones no es de tipo EigenSOE." << std::endl;
    return ptr;
  }

//! @brief Devuelve, si es posible, un puntero al integrador incremental en otro caso devuelve nullptr.
XC::IncrementalIntegrator *XC::SoluMethod::getIncrementalIntegratorPtr(void)
  {
    IncrementalIntegrator *ptr= dynamic_cast<IncrementalIntegrator *>(theIntegrator);
    if(!ptr)
      std::cerr << "SoluMethod; el integrador no es de tipo incremental." << std::endl;
    return ptr;
  }

//! @brief Devuelve, si es posible, un puntero al EigenIntegrator en otro caso devuelve nullptr.
XC::EigenIntegrator *XC::SoluMethod::getEigenIntegratorPtr(void)
  {
    EigenIntegrator *ptr= dynamic_cast<EigenIntegrator *>(theIntegrator);
    if(!ptr)
      std::cerr << "SoluMethod; el integrador no es de tipo autovalores." << std::endl;
    return ptr;
  }

//! @brief Devuelve, si es posible, un puntero al LinearBucklingIntegrator en otro caso devuelve nullptr.
XC::LinearBucklingIntegrator *XC::SoluMethod::getLinearBucklingIntegratorPtr(void)
  {
    LinearBucklingIntegrator *ptr= dynamic_cast<LinearBucklingIntegrator *>(theIntegrator);
    if(!ptr)
      std::cerr << "SoluMethod; el integrador no es de tipo linear buckling." << std::endl;
    return ptr;
  }

//! @brief Devuelve, si es posible, un puntero al EigenIntegrator en otro caso devuelve nullptr.
XC::TransientIntegrator *XC::SoluMethod::getTransientIntegratorPtr(void)
  {
    TransientIntegrator *ptr= dynamic_cast<TransientIntegrator *>(theIntegrator);
    if(!ptr)
      std::cerr << "SoluMethod; el integrador no es de tipo transitorio." << std::endl;
    return ptr;
  }

//! @brief Devuelve, si es posible, un puntero al StaticIntegrator en otro caso devuelve nullptr.
XC::StaticIntegrator *XC::SoluMethod::getStaticIntegratorPtr(void)
  {
    StaticIntegrator *ptr= dynamic_cast<StaticIntegrator *>(theIntegrator);
    if(!ptr)
      std::cerr << "SoluMethod; el integrador no es de tipo estático." << std::endl;
    return ptr;
  }

//! @brief Devuelve, si es posible, un puntero al algoritmo de solución del sistema de 
//! autovalores, en otro caso devuelve nullptr.
XC::EigenAlgorithm *XC::SoluMethod::getEigenSolutionAlgorithmPtr(void)
  {
    EigenAlgorithm *ptr= dynamic_cast<EigenAlgorithm *>(theSolnAlgo);
    if(!ptr)
      std::clog << "SoluMethod; el algoritmo no sirve para obtener autovalores." << std::endl;
    return ptr;
  }

//! @brief Devuelve, si es posible, un puntero al algoritmo de solución del sistema de 
//! ecuaciones, en otro caso devuelve nullptr.
XC::EquiSolnAlgo *XC::SoluMethod::getEquiSolutionAlgorithmPtr(void)
  {
    EquiSolnAlgo *ptr= dynamic_cast<EquiSolnAlgo *>(theSolnAlgo);
    if(!ptr)
      std::clog << "SoluMethod; el algortimo no sirve para obtener soluciones de equilibrio." << std::endl;
    return ptr;
  }

//! @brief Devuelve, si es posible, un puntero al algoritmo de solución
//! DomainDecomp , en otro caso devuelve nullptr.
XC::DomainDecompAlgo *XC::SoluMethod::getDomainDecompSolutionAlgorithmPtr(void)
  {
    DomainDecompAlgo *ptr= dynamic_cast<DomainDecompAlgo *>(theSolnAlgo);
    if(!ptr)
      std::cerr << "SoluMethod; el algortimo no sirve para descomposición de dominio." << std::endl;
    return ptr;
  }

//! @brief Devuelve un puntero al test de convergencia.
XC::ConvergenceTest *XC::SoluMethod::getConvergenceTestPtr(void)
  { return theTest; }

//! @brief Devuelve un puntero al test de convergencia.
const XC::ConvergenceTest *XC::SoluMethod::getConvergenceTestPtr(void) const
  { return theTest; }

//! @brief Establece el sistema de ecuaciones lineal a emplear en el análisis.
int XC::SoluMethod::setLinearSOE(LinearSOE &theNewSOE)
  {
    copia_sistema_ecuaciones(&theNewSOE);
    return 0;
  }

//! @brief Establece el sistema de autovalores a emplear en el análisis.
int XC::SoluMethod::setEigenSOE(EigenSOE &theSOE)
  {
    std::cerr << "SoluMethod::setEigenSOE() - does nothing yet\n";    
    return 0;
  }

//! @brief Establece el integrador a emplear en el análisis.
int XC::SoluMethod::setIntegrator(Integrator &theNewIntegrator)
  {
    copia_integrator(&theNewIntegrator);
    return 0;
  }

//! @brief Establece el algoritmo de solución a emplear en el análisis.
int XC::SoluMethod::setAlgorithm(SolutionAlgorithm &theNewAlgorithm) 
  {
    copia_soln_algo(&theNewAlgorithm);
    return 0;
  }

//! @brief Establece el test de convergencia a emplear en el análisis.
int XC::SoluMethod::setConvergenceTest(ConvergenceTest &theNewTest)
  {
    copia_conv_test(&theNewTest);
    return 0;
  }

void XC::SoluMethod::brokeDDLinearSOE(const CommParameters &cp,const ID &data)
  {
    theSOE = cp.brokeDDLinearSOE(data(5),data(6));
    theSOE->set_owner(this);
  }

void XC::SoluMethod::brokeLinearSOE(const CommParameters &cp,const ID &data)
  {
    theSOE = cp.brokeLinearSOE(data(4),data(5));
    theSOE->set_owner(this);
  }

void XC::SoluMethod::brokeIncrementalIntegrator(const CommParameters &cp,const ID &data)
  {
    theIntegrator= cp.brokeIncrementalIntegrator(data(4));
    theIntegrator->set_owner(this);
  }

void XC::SoluMethod::brokeStaticIntegrator(const CommParameters &cp,const ID &data)
  {
    theIntegrator= cp.brokeStaticIntegrator(data(6));
    theIntegrator->set_owner(this);
  }

void XC::SoluMethod::brokeTransientIntegrator(const CommParameters &cp,const ID &data)
  {
    theIntegrator= cp.brokeTransientIntegrator(data(6));
    theIntegrator->set_owner(this);
  }

void XC::SoluMethod::brokeDomainDecompAlgo(const CommParameters &cp,const ID &data)
  {
    theSolnAlgo= cp.brokeDomainDecompAlgo(data(3));
    theSolnAlgo->set_owner(this);
  }

void XC::SoluMethod::brokeEquiSolnAlgo(const CommParameters &cp,const ID &data)
  {
    theSolnAlgo= cp.brokeEquiSolnAlgo(data(3));
    theSolnAlgo->set_owner(this);
  }

//! @brief Verifica que los punteros no sean nulos.
bool XC::SoluMethod::CheckPointers(void)
  {
    if(!base)
      {
        std::cerr << "SoluMethod::check_pointers; error, model not defined." << std::endl;
        return false;
      }
    if(!base->CheckPointers())
      {
        return false;
      }
    if(!theSolnAlgo)
      {
        std::cerr << "SoluMethod::check_pointers; error, solution algorithm not defined." << std::endl;
        return false;
      }
    if(!theSOE)
      {
        std::cerr << "SoluMethod::check_pointers; error, system of equations not defined." << std::endl;
        return false;
      }
    if(!theIntegrator)
      {
        std::cerr << "SoluMethod::check_pointers; error, integrator not defined." << std::endl;
        return false;
      }
    return true;
  }

//! @brief Devuelve el modelo a su estado original.
void XC::SoluMethod::revertToStart(void)
  {
// AddingSensitivity:BEGIN ////////////////////////////////////
	// (Calling domainChanged() don't work because the ::getX()
	// of the linear SOE is being called without the X having
	// been instantitated.)
    TransientIntegrator *tmp= getTransientIntegratorPtr();
    if(tmp) tmp->revertToStart();
// AddingSensitivity:END //////////////////////////////////////
  }
