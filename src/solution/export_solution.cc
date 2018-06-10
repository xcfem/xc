//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//utils_python_interface.cxx

#include "python_interface.h"
#include "FEProblem.h"

void export_solution(void)
  {
    using namespace boost::python;
    docstring_options doc_options;

#include "analysis/python_interface.tcc"
#include "system_of_eqn/python_interface.tcc"

class_<XC::ConvergenceTest, bases<XC::MovableObject,EntCmd>, boost::noncopyable >("ConvergenceTest", no_init);

 class_<XC::AnalysisAggregation, bases<EntCmd>, boost::noncopyable >("AnalysisAggregation", "Solution methods container",no_init)
    .def("newSolutionAlgorithm", &XC::AnalysisAggregation::newSolutionAlgorithm,return_internal_reference<>(),"\n""newSolutionAlgorithm(type) \n""Define the solution algorithm to be used.\n" "Parameters: \n""type: type of solution algorithm. Available types: 'bfgs_soln_algo', 'broyden_soln_algo','krylov_newton_soln_algo','linear_soln_algo','modified_newton_soln_algo','newton_raphson_soln_algo','newton_line_search_soln_algo','periodic_newton_soln_algo','frequency_soln_algo','standard_eigen_soln_algo','linear_buckling_soln_algo' \n")
    .def("newIntegrator", &XC::AnalysisAggregation::newIntegrator,return_internal_reference<>()," \n""newIntegrator(type,params) \n""Define the integrator to be used. \n""Parameters: \n""type: type of integrator. Available types:  'arc_length_integrator', 'arc_length1_integrator', 'displacement_control_integrator', 'distributed_displacement_control_integrator', 'HS_constraint_integrator', 'load_control_integrator', 'load_path_integrator', 'min_unbal_disp_norm_integrator', 'eigen_integrator', 'linear_buckling_integrator', 'alpha_os_integrator', 'alpha_os_generalized_integrator', 'central_difference_integrator', 'central_difference_alternative_integrator', 'central_difference_no_damping_integrator', 'collocation_integrator', 'collocation_hybrid_simulation_integrator', 'HHT_integrator', 'HHT1_integrator', 'HHT_explicit_integrator', 'HHT_generalized_integrator', 'HHT_generalized_explicit_integrator', 'HHT_hybrid_simulation_integrator', 'newmark_integrator', 'newmark1_integrator', 'newmark_explicit_integrator' 'newmark_hybrid_simulation_integrator', 'wilson_theta_integrator'. \n""params: parameters depending upon the integrator type. \n")
    .def("newSystemOfEqn", &XC::AnalysisAggregation::newSystemOfEqn,return_internal_reference<>()," \n""newSystemOfEqn(type) \n""Define the system of equations to be used. \n""Parameters: \n""type: type of system of equations. Available types: 'band_arpack_soe', 'band_arpackpp_soe', 'sym_arpack_soe', 'sym_band_eigen_soe', 'full_gen_eigen_soe', 'band_gen_lin_soe', 'distributed_band_gen_lin_soe', 'band_spd_lin_soe', 'distributed_band_spd_lin_soe', 'diagonal_soe', 'distributed_diagonal_soe', 'full_gen_lin_soe', 'profile_spd_lin_soe', 'distributed_profile_spd_lin_soe', 'sparse_gen_col_lin_soe', 'distributed_sparse_gen_col_lin_soe', 'sparse_gen_row_lin_soe', 'distributed_sparse_gen_row_lin_soe', 'sym_sparse_lin_soe'.  \n")
    .def("newConvergenceTest", &XC::AnalysisAggregation::newConvergenceTest,return_internal_reference<>()," \n""newConvergenceTest(cmd) \n""Define the convergence test to be used. \n""Parameters: \n""cmd: type of convergente test. Available types: 'energy_inc_conv_test', 'fixed_num_iter_conv_test', 'norm_disp_incr_conv_test', 'norm_unbalance_conv_test', 'relative_energy_incr_conv_test', 'relative_norm_disp_incr_conv_test', 'relative_norm_unbalance_conv_test', 'relative_total_norm_disp_incr_conv_test'. \n")
    ;

class_<XC::AnalysisAggregationMap, bases<EntCmd>, boost::noncopyable >("AnalysisAggregationMap", no_init)
  .add_property("AnalysisAggregationExists", &XC::AnalysisAggregationMap::AnalysisAggregationExists,"\n""AnalysisAggregationExists(cod) \n""Return true if the solution method exists \n" "Parameters: \n" "cod: name of the solution method \n")
    .def("newAnalysisAggregation", &XC::AnalysisAggregationMap::newAnalysisAggregation,return_internal_reference<>(),"\n""newAnalysisAggregation(cod_solu_method,cod_solu_model) \n""Create a new solution procedure.\n""Parameters: \n" "cod_solu_method: name to identify the solution method. \n""cod_solu_model: name of the model wrapper \n")
    ;

XC::ModelWrapper *(XC::ProcSoluControl::*getModelWrapperPtr)(const std::string &)= &XC::ProcSoluControl::getModelWrapper;
class_<XC::ProcSoluControl, bases<EntCmd>, boost::noncopyable >("SoluControl", "Container for the objects that control the solution procedure", no_init)
  .add_property("getModelWrapper", make_function( getModelWrapperPtr, return_internal_reference<>() )," \n""getModelWrapper(cod) \n""Return a pointer to the model wrapper \n""Parameters: \n""cod: name of the model wrapper \n")
    .add_property("getModelWrapperContainer",  make_function(&XC::ProcSoluControl::getModelWrapperContainer, return_internal_reference<>()) ," \n""Return a reference to the model wrapper container. \n")
    .add_property("getAnalysisAggregationContainer",  make_function(&XC::ProcSoluControl::getAnalysisAggregationContainer, return_internal_reference<>()) ," \n""Return a reference to the solution procedures container. \n")
    ;

XC::ProcSoluControl &(XC::ProcSolu::*getSoluControlRef)(void)= &XC::ProcSolu::getSoluControl;
 class_<XC::ProcSolu, bases<EntCmd>, boost::noncopyable >("ProcSolu","Definition of the analysis by its type and the parameters that control the solution procedure.",no_init)
   .add_property("getSoluControl", make_function( getSoluControlRef, return_internal_reference<>() )," \n"" Return a reference to the objects  that control the solution procedure.\n")
   .add_property("getAnalysis", make_function( &XC::ProcSolu::getAnalysis, return_internal_reference<>() )," \n"" Return a reference to the analysis object. \n")
    .def("newAnalysis", &XC::ProcSolu::newAnalysis,return_internal_reference<>()," \n""newAnalysis(nmb,analysis_aggregation_code,cod_solu_eigenM) \n""Definition of a new analysis.""Parameters: \n""nmb: name of the type of analysis. Available types: 'direct_integration_analysis', 'eigen_analysis', 'modal_analysis','linear_buckling_analysis', 'linear_buckling_eigen_analysis', 'static_analysis', 'variable_time_step_direct_integration_analysis' \n""analysis_aggregation_code: name of the solution method container \n""cod_solu_eigenM: name of the solution method (only when linear buckling analysis defined).\n")
   .def("clear", &XC::ProcSolu::clearAll,"clear all previously defined analysis parameters.")
    ;

  }

