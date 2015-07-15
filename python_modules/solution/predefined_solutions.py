# -*- coding: utf-8 -*-

import xc_base
import geom
import xc

class SolutionProcedure(object):
  solu= None
  solCtrl= None
  sm= None
  numberer= None
  cHandler= None
  smt= None
  solAlgo= None
  integ= None
  soe= None
  solver= None
  analysis= None
  convergenceTestTol= 1e-9
  maxNumIter= 10
  def simpleStaticLinear(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("rcm")
    self.cHandler= self.sm.newConstraintHandler("penalty_constraint_handler")
    self.cHandler.alphaSP= 1.0e15
    self.cHandler.alphaMP= 1.0e15
    solMethods= self.solCtrl.getSoluMethodContainer
    self.smt= solMethods.newSoluMethod("smt","sm")
    self.solAlgo= self.smt.newSolutionAlgorithm("linear_soln_algo")
    self.integ= self.smt.newIntegrator("load_control_integrator",xc.Vector([]))
    self.soe= self.smt.newSystemOfEqn("band_spd_lin_soe")
    self.solver= self.soe.newSolver("band_spd_lin_lapack_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","smt","")
    return self.analysis;
  def simpleLagrangeStaticLinear(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("rcm")
    self.cHandler= self.sm.newConstraintHandler("lagrange_constraint_handler")
    solMethods= self.solCtrl.getSoluMethodContainer
    self.smt= solMethods.newSoluMethod("smt","sm")
    self.solAlgo= self.smt.newSolutionAlgorithm("linear_soln_algo")
    self.integ= self.smt.newIntegrator("load_control_integrator",xc.Vector([]))
    self.soe= self.smt.newSystemOfEqn("sparse_gen_col_lin_soe")
    self.solver= self.soe.newSolver("super_lu_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","smt","")
    return self.analysis;
  def simpleNewtonRaphson(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("simple")
    self.cHandler= self.sm.newConstraintHandler("plain_handler")
    solMethods= self.solCtrl.getSoluMethodContainer
    self.smt= solMethods.newSoluMethod("smt","sm")
    self.solAlgo= self.smt.newSolutionAlgorithm("newton_raphson_soln_algo")
    self.integ= self.smt.newIntegrator("load_control_integrator",xc.Vector([]))
    self.ctest= self.smt.newConvergenceTest("norm_unbalance_conv_test")
    self.ctest.tol= self.convergenceTestTol
    self.ctest.maxNumIter= self.maxNumIter
    self.soe= self.smt.newSystemOfEqn("sparse_gen_col_lin_soe")
    self.solver= self.soe.newSolver("super_lu_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","smt","")
    return self.analysis;
  def simpleNewtonRaphsonBandGen(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("simple")
    self.cHandler= self.sm.newConstraintHandler("plain_handler")
    solMethods= self.solCtrl.getSoluMethodContainer
    self.smt= solMethods.newSoluMethod("smt","sm")
    self.solAlgo= self.smt.newSolutionAlgorithm("newton_raphson_soln_algo")
    self.integ= self.smt.newIntegrator("load_control_integrator",xc.Vector([]))
    self.ctest= self.smt.newConvergenceTest("norm_unbalance_conv_test")
    self.ctest.tol= self.convergenceTestTol
    self.ctest.maxNumIter= self.maxNumIter
    self.soe= self.smt.newSystemOfEqn("band_gen_lin_soe")
    self.solver= self.soe.newSolver("band_gen_lin_lapack_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","smt","")
    return self.analysis;
  def simpleStaticModifiedNewton(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("simple")
    self.cHandler= self.sm.newConstraintHandler("plain_handler")
    solMethods= self.solCtrl.getSoluMethodContainer
    self.smt= solMethods.newSoluMethod("smt","sm")
    self.solAlgo= self.smt.newSolutionAlgorithm("modified_newton_soln_algo")
    self.integ= self.smt.newIntegrator("load_control_integrator",xc.Vector([]))
    self.ctest= self.smt.newConvergenceTest("relative_total_norm_disp_incr_conv_test")
    self.ctest.tol= 0.01 #Make this configurable
    self.ctest.maxNumIter= 150 #Make this configurable
    self.soe= self.smt.newSystemOfEqn("sparse_gen_col_lin_soe")
    self.solver= self.soe.newSolver("super_lu_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","smt","")
    return self.analysis;
  def penaltyNewtonRaphson(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("rcm")
    self.cHandler= self.sm.newConstraintHandler("penalty_constraint_handler")
    self.cHandler.alphaSP= 1.0e15
    self.cHandler.alphaMP= 1.0e15
    solMethods= self.solCtrl.getSoluMethodContainer
    self.smt= solMethods.newSoluMethod("smt","sm")
    self.solAlgo= self.smt.newSolutionAlgorithm("newton_raphson_soln_algo")
    self.ctest= self.smt.newConvergenceTest("norm_unbalance_conv_test")
    self.ctest.tol= 1.0e-4
    self.ctest.maxNumIter= 10
    self.integ= self.smt.newIntegrator("load_control_integrator",xc.Vector([]))
    self.soe= self.smt.newSystemOfEqn("band_gen_lin_soe")
    self.solver= self.soe.newSolver("band_gen_lin_lapack_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","smt","")
    return self.analysis;

  def penaltyNewmarkNewtonRapshon(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("rcm")
    self.cHandler= self.sm.newConstraintHandler("penalty_constraint_handler")
    self.cHandler.alphaSP= 1.0e18
    self.cHandler.alphaMP= 1.0e18
    solMethods= self.solCtrl.getSoluMethodContainer
    self.smt= solMethods.newSoluMethod("smt","sm")
    self.solAlgo= self.smt.newSolutionAlgorithm("newton_raphson_soln_algo")
    self.ctest= self.smt.newConvergenceTest("norm_disp_incr_conv_test")
    self.ctest.tol= 1.0e-3
    self.ctest.maxNumIter= 35
    self.ctest.printFlag= 1
    self.integ= self.smt.newIntegrator("newmark_integrator",xc.Vector([]))
    self.soe= self.smt.newSystemOfEqn("profile_spd_lin_soe")
    self.solver= self.soe.newSolver("profile_spd_lin_lapack_solver")
    self.analysis= self.solu.newAnalysis("direct_integration_analysis","smt","")
    return self.analysis;

  def frequencyAnalysis(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.cHandler= self.sm.newConstraintHandler("transformation_constraint_handler")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("rcm")
    solMethods= self.solCtrl.getSoluMethodContainer
    self.smt= solMethods.newSoluMethod("smt","sm")
    self.solAlgo= self.smt.newSolutionAlgorithm("frequency_soln_algo")
    self.integ= self.smt.newIntegrator("eigen_integrator",xc.Vector([1.0,1,1.0,1.0]))
    self.soe= self.smt.newSystemOfEqn("sym_band_eigen_soe")
    self.solver= self.soe.newSolver("sym_band_eigen_solver")
    self.analysis= self.solu.newAnalysis("modal_analysis","smt","")
    return self.analysis

#Typical solution procedures.

#Linear static analysis.
def simple_static_linear(prb):
  solution= SolutionProcedure()
  return solution.simpleStaticLinear(prb)

#Linear static analysis.
def simple_newton_raphson(prb):
  solution= SolutionProcedure()
  return solution.simpleNewtonRaphson(prb)

def simple_newton_raphson_band_gen(prb):
  solution= SolutionProcedure()
  return solution.simpleNewtonRaphsonBandGeh(prb)

def simple_static_modified_newton(prb):
  solution= SolutionProcedure()
  return solution.simpleStaticModifiedNewton(prb)

def penalty_newton_raphson(prb):
  solution= SolutionProcedure()
  return solution.penaltyNewtonRaphson(prb)

def frequency_analysis(prb):
  solution= SolutionProcedure()
  return solution.frequencyAnalysis(prb)
