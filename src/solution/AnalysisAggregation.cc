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
//AnalysisAggregation.cc

#include "AnalysisAggregation.h"
#include "analysis/ModelWrapper.h"


//Constraint handler.
#include <solution/analysis/handler/ConstraintHandler.h>

//Integrators.
#include "solution/analysis/integrator/integrators.h"

//Solution algorithm.
#include "solution/analysis/algorithm/solution_algorithms.h"

//systems of equations
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

void XC::AnalysisAggregation::free_soln_algo(void)
  {
    if(theSolnAlgo)
      {
        delete theSolnAlgo;
        theSolnAlgo= nullptr;
      }
  }

bool XC::AnalysisAggregation::alloc_soln_algo(const std::string &nmb)
  {
    free_soln_algo();

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
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; solution algorithm: '"
                << nmb << "' unknown." << std::endl;
    if(theSolnAlgo)
      theSolnAlgo->set_owner(this);

    return (theSolnAlgo!=nullptr);
  }

//! To change the algorithm between analysis. It first invokes the
//! destructor on the old SolutionAlgorithm object associated with the
//! analysis. It then sets the SolutionAlgorithm 
//! associated with the analysis to be \p newAlgorithm and sets the
//! links for this object by invoking set_owner(). 
void XC::AnalysisAggregation::copy_soln_algo(SolutionAlgorithm *ptr)
  {
    if(ptr)
      {
        free_soln_algo();
        theSolnAlgo= ptr->getCopy();
        theSolnAlgo->set_owner(this);
      }
    else
     std::cerr << getClassName() << "::" << __FUNCTION__
	       << "; null pointer to solution algorithm." << std::endl;
  }

//! @brief Creates a new solution algorithm with the name being passed as parameter.
XC::SolutionAlgorithm &XC::AnalysisAggregation::newSolutionAlgorithm(const std::string &type)
  {
    alloc_soln_algo(type);
    assert(theSolnAlgo);
    return *theSolnAlgo;
  }

void XC::AnalysisAggregation::free_integrator(void)
  {
    if(theIntegrator)
      {
        delete theIntegrator;
        theIntegrator= nullptr;
      }
  }

bool XC::AnalysisAggregation::alloc_integrator(const std::string &nmb,const Vector &params)
  {
    free_integrator();

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
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; integrator: '"
                << nmb << "' unknown." << std::endl;

    if(theIntegrator)
      theIntegrator->set_owner(this);

    return (theIntegrator!=nullptr);
  }

//! To change the integration scheme between analysis. It first invokes the
//! destructor on the old Integrator object associated with the
//! analysis. It then sets the SolutionAlgorithm 
//! associated with the analysis to be \p newAlgorithm and sets the
//! links for this object. It also invokes set_owner().
void XC::AnalysisAggregation::copy_integrator(Integrator *ptr)
  {
    if(ptr)
      {
        free_integrator();
        theIntegrator= ptr->getCopy();
        theIntegrator->set_owner(this);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
 	        << "; null pointer to integrator." << std::endl;
  }

//! @brief Defines a new integrator.
XC::Integrator &XC::AnalysisAggregation::newIntegrator(const std::string &type, const Vector &params)
  {
    if(alloc_integrator(type,params))
      {
        DisplacementControl *dtmp= dynamic_cast<DisplacementControl *>(theIntegrator);
        if(dtmp)
          dtmp->set_owner(this);
      }
    assert(theIntegrator);
    return *theIntegrator;
  }

void XC::AnalysisAggregation::free_system_of_equations(void)
  {
    if(theSOE)
      {
        delete theSOE;
        theSOE= nullptr;
      }
  }
bool XC::AnalysisAggregation::alloc_system_of_equations(const std::string &nmb,AnalysisModel *theModel)
  {
    free_system_of_equations();
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
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; system of equations: '"
                << nmb << "' unknown." << std::endl;

    if(theSOE)
      theSOE->set_owner(this);
    return (theSOE!=nullptr);
  }

//! To change the linear system of equation object between analysis. It
//! first invokes the destructor on the old LinearSOE object associated
//! with the analysis. It then sets the SolutionAlgorithm 
//! associated with the analysis to be \p newSOE.
//! links for this object. It then invokes set_owner(). 
void XC::AnalysisAggregation::copy_system_of_equations(SystemOfEqn *ptr)
  {
    if(ptr)
      {
        free_system_of_equations();
        theSOE= ptr->getCopy();
        theSOE->set_owner(this);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; null pointer to SOE." << std::endl;
  }

//! @brief Sets the system of equations type to use.
XC::SystemOfEqn &XC::AnalysisAggregation::newSystemOfEqn(const std::string &type)
  {
    AnalysisModel *theModel= nullptr;
    if(base)
      {
        theModel= base->getAnalysisModelPtr();
        alloc_system_of_equations(type,theModel);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; analysis model is not defined." << std::endl;
    assert(theSOE);
    return *theSOE;
  }

void XC::AnalysisAggregation::free_conv_test(void)
  {
    if(theTest)
      {
        delete theTest;
        theTest= nullptr;
      }
  }

bool XC::AnalysisAggregation::alloc_conv_test(const std::string &nmb)
  {
    free_conv_test();
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
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; convergence test type: '"
                << nmb << "' unknown." << std::endl;
    if(theTest)
      theTest->set_owner(this);
    return (theTest!=nullptr);
  }

void XC::AnalysisAggregation::copy_conv_test(ConvergenceTest *ptr)
  {
    if(ptr)
      {
        free_conv_test();
        theTest= ptr->getCopy();
        theTest->set_owner(this);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; null pointer to convergence test." << std::endl;
  }

//! @brief Set convergence test to be used.
XC::ConvergenceTest &XC::AnalysisAggregation::newConvergenceTest(const std::string &cmd)
  {
    alloc_conv_test(cmd);
    assert(theTest);
    return *theTest;
  }

void XC::AnalysisAggregation::free_mem(void)
  {
    free_soln_algo();
    free_integrator();
    free_system_of_equations();
    free_conv_test();
  }

void XC::AnalysisAggregation::copy(const AnalysisAggregation &otro)
  {
    if(otro.theSolnAlgo) copy_soln_algo(otro.theSolnAlgo);
    if(otro.theIntegrator) copy_integrator(otro.theIntegrator);
    if(otro.theSOE) copy_system_of_equations(otro.theSOE);
    if(otro.theTest) copy_conv_test(otro.theTest);
  }

//! @brief Default constructor.
XC::AnalysisAggregation::AnalysisAggregation(Analysis *owr,ModelWrapper *b)
  : EntCmd(owr), base(b), theSolnAlgo(nullptr),theIntegrator(nullptr),
    theSOE(nullptr), theTest(nullptr)
  {
    if(base)
      base->set_owner(this);
  }


//! @brief Copy constructor.
XC::AnalysisAggregation::AnalysisAggregation(const AnalysisAggregation &otro)
  : EntCmd(otro), base(otro.base), theSolnAlgo(nullptr),theIntegrator(nullptr),
    theSOE(nullptr), theTest(nullptr)
  {
    if(base)
      base->set_owner(this);
    copy(otro);
  }


//! @brief Assignment operator.
XC::AnalysisAggregation &XC::AnalysisAggregation::operator=(const AnalysisAggregation &otro)
  {
    EntCmd::operator=(otro);
    base= otro.base;
    if(base)
      base->set_owner(this);
    copy(otro);
    return *this;
  }


//! @brief Destructor.
XC::AnalysisAggregation::~AnalysisAggregation(void)
  { free_mem(); }

void XC::AnalysisAggregation::clearAll(void)
  { free_mem(); }

XC::Analysis *XC::AnalysisAggregation::getAnalysis(void)
  { return dynamic_cast<Analysis *>(Owner()); }

const XC::Analysis *XC::AnalysisAggregation::getAnalysis(void) const
  { return dynamic_cast<const Analysis *>(Owner()); }

//! @brief Returns a pointer to the domain.
XC::Domain *XC::AnalysisAggregation::getDomainPtr(void)
  {
    Domain *retval= nullptr;
    Analysis *an= getAnalysis();
    if(an)
      retval= an->getDomainPtr();
    return retval;
  }

//! @brief Returns a pointer to the domain.
const XC::Domain *XC::AnalysisAggregation::getDomainPtr(void) const
  {
    const Domain *retval= nullptr;
    const Analysis *an= getAnalysis();
    assert(an);
    if(an)
      retval= an->getDomainPtr();
    return retval;
  }

//! @brief Returns a pointer to the DomainSolver.
const XC::DomainSolver *XC::AnalysisAggregation::getDomainSolverPtr(void) const
  {
    const Analysis *an= getAnalysis();
    assert(an);
    return an->getDomainSolver();
  }

//! @brief Returns a pointer to the DomainSolver.
XC::DomainSolver *XC::AnalysisAggregation::getDomainSolverPtr(void)
  {
    Analysis *an= getAnalysis();
    assert(an);
    return an->getDomainSolver();
  }

//! @brief Returns a pointer to the subdomain.
const XC::Subdomain *XC::AnalysisAggregation::getSubdomainPtr(void) const
  {
    const Analysis *an= getAnalysis();
    assert(an);
    return an->getSubdomain();
  }

//! @brief Returns a pointer to the subdomain.
XC::Subdomain *XC::AnalysisAggregation::getSubdomainPtr(void)
  {
    Analysis *an= getAnalysis();
    assert(an);
    return an->getSubdomain();
  }

//! @brief Returns a pointer to the constraint handler.
XC::ConstraintHandler *XC::AnalysisAggregation::getConstraintHandlerPtr(void)
  {
    if(base)
      return base->getConstraintHandlerPtr();
    else
      return nullptr;
  }

//! @brief Returns a pointer to the numberer.
XC::DOF_Numberer *XC::AnalysisAggregation::getDOF_NumbererPtr(void) const
  {
    if(base)
      return base->getDOF_NumbererPtr();
    else
      return nullptr;
  }
//! @brief Returns a pointer to the analysis model.
XC::AnalysisModel *XC::AnalysisAggregation::getAnalysisModelPtr(void) const
  {
    if(base)
      return base->getAnalysisModelPtr();
    else
      return nullptr;
  }


//! @brief Returns a pointer to the linear system of equations.
XC::LinearSOE *XC::AnalysisAggregation::getLinearSOEPtr(void)
  {
    LinearSOE *ptr= dynamic_cast<LinearSOE *>(theSOE);
    if(!ptr)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; not a suitable system of equations (not a LinearSOE)."
		<< std::endl;
    return ptr;
  }

//! @brief Returns a pointer to the linear system of equations.
const XC::LinearSOE *XC::AnalysisAggregation::getLinearSOEPtr(void) const
  {
    const LinearSOE *ptr= dynamic_cast<const LinearSOE *>(theSOE);
    if(!ptr)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; not a suitable system of equations (not a LinearSOE)."
		<< std::endl;
    return ptr;
  }

//! @brief Returns a pointer to the system of equations de eigenvalues.
XC::EigenSOE *XC::AnalysisAggregation::getEigenSOEPtr(void)
  {
    EigenSOE *ptr= dynamic_cast<EigenSOE *>(theSOE);
    if(!ptr)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; not a suitable system of equations (not an EigenSOE)."
		<< std::endl;
    return ptr;
  }

//! @brief Returns a pointer to the system of equations de eigenvalues.
const XC::EigenSOE *XC::AnalysisAggregation::getEigenSOEPtr(void) const
  {
    const EigenSOE *ptr= dynamic_cast<const EigenSOE *>(theSOE);
    if(!ptr)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; not a suitable system of equations (not an EigenSOE)."
		<< std::endl;
    return ptr;
  }

//! @brief Returns, if possible, a pointer to the incremental integrator
//! otherwise it returns nullptr.
XC::IncrementalIntegrator *XC::AnalysisAggregation::getIncrementalIntegratorPtr(void)
  {
    IncrementalIntegrator *ptr= dynamic_cast<IncrementalIntegrator *>(theIntegrator);
    if(!ptr)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; not an incremental integrator." << std::endl;
    return ptr;
  }

//! @brief Returns, if possible, a pointer to the eigen integrator
//! otherwise it returns nullptr.
XC::EigenIntegrator *XC::AnalysisAggregation::getEigenIntegratorPtr(void)
  {
    EigenIntegrator *ptr= dynamic_cast<EigenIntegrator *>(theIntegrator);
    if(!ptr)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; not an eigen integrator." << std::endl;
    return ptr;
  }

//! @brief Returns, if possible, a pointer to the linear buckling integrator
//! otherwise it returns nullptr.
XC::LinearBucklingIntegrator *XC::AnalysisAggregation::getLinearBucklingIntegratorPtr(void)
  {
    LinearBucklingIntegrator *ptr= dynamic_cast<LinearBucklingIntegrator *>(theIntegrator);
    if(!ptr)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; not a linear buckling integrator." << std::endl;
    return ptr;
  }

//! @brief Returns, if possible, a pointer to the transient integrator
//! otherwise it returns nullptr.
XC::TransientIntegrator *XC::AnalysisAggregation::getTransientIntegratorPtr(void)
  {
    TransientIntegrator *ptr= dynamic_cast<TransientIntegrator *>(theIntegrator);
    if(!ptr)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; not a transient integrator." << std::endl;
    return ptr;
  }

//! @brief Returns, if possible, a pointer to the static integrator
//! otherwise it returns nullptr.
XC::StaticIntegrator *XC::AnalysisAggregation::getStaticIntegratorPtr(void)
  {
    StaticIntegrator *ptr= dynamic_cast<StaticIntegrator *>(theIntegrator);
    if(!ptr)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; not a static integrator." << std::endl;
    return ptr;
  }

//! @brief Returns, if possible, a pointer to the solution algorithm
//! of the eigen problem,, otherwise it returns nullptr.
XC::EigenAlgorithm *XC::AnalysisAggregation::getEigenSolutionAlgorithmPtr(void)
  {
    EigenAlgorithm *ptr= dynamic_cast<EigenAlgorithm *>(theSolnAlgo);
    if(!ptr)
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; the algorithm can't compute eigenvalues." << std::endl;
    return ptr;
  }

//! @brief Returns, if possible, a pointer to the system of equations
//! solution algorithm, otherwise it returns nullptr.
XC::EquiSolnAlgo *XC::AnalysisAggregation::getEquiSolutionAlgorithmPtr(void)
  {
    EquiSolnAlgo *ptr= dynamic_cast<EquiSolnAlgo *>(theSolnAlgo);
    if(!ptr)
      std::clog << getClassName() << "::" << __FUNCTION__
                << "; the algorithm can't solve for equilibrium." << std::endl;
    return ptr;
  }

//! @brief Returns, if possible, a pointer al solution algorithm
//! DomainDecomp , otherwise it returns nullptr.
XC::DomainDecompAlgo *XC::AnalysisAggregation::getDomainDecompSolutionAlgorithmPtr(void)
  {
    DomainDecompAlgo *ptr= dynamic_cast<DomainDecompAlgo *>(theSolnAlgo);
    if(!ptr)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; the algorithm can't make domain decomposition."
		<< std::endl;
    return ptr;
  }

//! @brief Returns a pointer to the convergence test.
XC::ConvergenceTest *XC::AnalysisAggregation::getConvergenceTestPtr(void)
  { return theTest; }

//! @brief Returns a pointer to the convergence test.
const XC::ConvergenceTest *XC::AnalysisAggregation::getConvergenceTestPtr(void) const
  { return theTest; }

//! @brief Sets the linear system of equations to use in the analysis.
int XC::AnalysisAggregation::setLinearSOE(LinearSOE &theNewSOE)
  {
    copy_system_of_equations(&theNewSOE);
    return 0;
  }

//! @brief Sets the eigenSOE to use in the analysis.
int XC::AnalysisAggregation::setEigenSOE(EigenSOE &theSOE)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; does nothing yet\n";    
    return 0;
  }

//! @brief Sets the integrator to use in the analysis.
int XC::AnalysisAggregation::setIntegrator(Integrator &theNewIntegrator)
  {
    copy_integrator(&theNewIntegrator);
    return 0;
  }

//! @brief Sets the solution algorithm to use in the analysis.
int XC::AnalysisAggregation::setAlgorithm(SolutionAlgorithm &theNewAlgorithm) 
  {
    copy_soln_algo(&theNewAlgorithm);
    return 0;
  }

//! @brief Sets the convergence test to use in the analysis.
int XC::AnalysisAggregation::setConvergenceTest(ConvergenceTest &theNewTest)
  {
    copy_conv_test(&theNewTest);
    return 0;
  }

void XC::AnalysisAggregation::brokeDDLinearSOE(const CommParameters &cp,const ID &data)
  {
    theSOE = cp.brokeDDLinearSOE(data(5),data(6));
    theSOE->set_owner(this);
  }

void XC::AnalysisAggregation::brokeLinearSOE(const CommParameters &cp,const ID &data)
  {
    theSOE = cp.brokeLinearSOE(data(4),data(5));
    theSOE->set_owner(this);
  }

void XC::AnalysisAggregation::brokeIncrementalIntegrator(const CommParameters &cp,const ID &data)
  {
    theIntegrator= cp.brokeIncrementalIntegrator(data(4));
    theIntegrator->set_owner(this);
  }

void XC::AnalysisAggregation::brokeStaticIntegrator(const CommParameters &cp,const ID &data)
  {
    theIntegrator= cp.brokeStaticIntegrator(data(6));
    theIntegrator->set_owner(this);
  }

void XC::AnalysisAggregation::brokeTransientIntegrator(const CommParameters &cp,const ID &data)
  {
    theIntegrator= cp.brokeTransientIntegrator(data(6));
    theIntegrator->set_owner(this);
  }

void XC::AnalysisAggregation::brokeDomainDecompAlgo(const CommParameters &cp,const ID &data)
  {
    theSolnAlgo= cp.brokeDomainDecompAlgo(data(3));
    theSolnAlgo->set_owner(this);
  }

void XC::AnalysisAggregation::brokeEquiSolnAlgo(const CommParameters &cp,const ID &data)
  {
    theSolnAlgo= cp.brokeEquiSolnAlgo(data(3));
    theSolnAlgo->set_owner(this);
  }

//! @brief Verifica que los pointers no sean nulos.
bool XC::AnalysisAggregation::CheckPointers(void)
  {
    if(!base)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; error, model not defined." << std::endl;
        return false;
      }
    if(!base->CheckPointers())
      {
        return false;
      }
    if(!theSolnAlgo)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error, solution algorithm not defined." << std::endl;
        return false;
      }
    if(!theSOE)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error, system of equations not defined." << std::endl;
        return false;
      }
    if(!theIntegrator)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error, integrator not defined." << std::endl;
        return false;
      }
    return true;
  }

//! @brief Returns the modelo a su estado original.
void XC::AnalysisAggregation::revertToStart(void)
  {
// AddingSensitivity:BEGIN ////////////////////////////////////
	// (Calling domainChanged() don't work because the ::getX()
	// of the linear SOE is being called without the X having
	// been instantitated.)
    TransientIntegrator *tmp= getTransientIntegratorPtr();
    if(tmp) tmp->revertToStart();
// AddingSensitivity:END //////////////////////////////////////
  }
