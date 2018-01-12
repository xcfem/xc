# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc

class SolutionProcedure(object):
  '''
  :ivar solu:
  :ivar solCtrl:
  :ivar sm:
  :ivar numberer:  DOF numberer. Determines the mapping between equation 
                   numbers and degrees of freedom (how DOF are numbered)
  :ivar cHandler:  constraint handler. Determines how the constraint equations 
                   are enforced in the analysis, how it handles the boundary
                   conditions/imposed displacements 
  :ivar analysisAggregation:
  :ivar solAlgo: solution algorithm, which determines the sequence of steps
                 taken to solve the non-linear equation:

                 - linear
                 - Newton Raphson (the tangent is updated at each iteration)
                 - Newton with Line Search
                 - Modified Newton Raphson (the tangent is not updated at 
                 each iteration)
                 - Kyrlov-Newton
                 - BFGS, for symetric systems
                 - Broyden
  :ivar integ:   integrator, used to:

                 - determine the predictive step for time t+dt
                 - specify the tangent matrix and residual vectir at any
                   iteration
                 - determine the corrective step based on the displacement 
                   increment
  :ivar soe:       sparse system of equations: band general, band SPD, 
                   profile SPD, sparse general, umfPack, sparse SPD
  :ivar solver:
  :ivar analysis:  determines what type of analysis is to be performed
  :ivar convergenceTestTol: convergence tolerance (defaults to 1e-9)
  :ivar maxNumIter: maximum number of iterations (defauts to 10)
  '''
  def __init__(self):
    self.solu= None
    self.solCtrl= None
    self.sm= None
    self.numberer= None
    self.cHandler= None
    self.analysisAggregation= None
    self.solAlgo= None
    self.integ= None
    self.soe= None
    self.solver= None
    self.analysis= None
    self.convergenceTestTol= 1e-9
    self.maxNumIter= 10
    self.printFlag= 0
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
    analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
    self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
    self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("linear_soln_algo")
    self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
    self.soe= self.analysisAggregation.newSystemOfEqn("band_spd_lin_soe")
    self.solver= self.soe.newSolver("band_spd_lin_lapack_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
    return self.analysis;
  def simpleLagrangeStaticLinear(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("rcm")
    self.cHandler= self.sm.newConstraintHandler("lagrange_constraint_handler")
    analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
    self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
    self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("linear_soln_algo")
    self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
    self.soe= self.analysisAggregation.newSystemOfEqn("sparse_gen_col_lin_soe")
    self.solver= self.soe.newSolver("super_lu_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
    return self.analysis;
  def simpleTransformationStaticLinear(self,prb):
    ''' Simple solution algorithm with a transformation constraint handler.'''
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("rcm")
    self.cHandler= self.sm.newConstraintHandler("transformation_constraint_handler")
    analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
    self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
    self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("linear_soln_algo")
    self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
    self.soe= self.analysisAggregation.newSystemOfEqn("sparse_gen_col_lin_soe")
    self.solver= self.soe.newSolver("super_lu_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
    return self.analysis;
  def simpleNewtonRaphson(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("simple")
    self.cHandler= self.sm.newConstraintHandler("plain_handler")
    analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
    self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
    self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("newton_raphson_soln_algo")
    self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
    self.ctest= self.analysisAggregation.newConvergenceTest("norm_unbalance_conv_test")
    self.ctest.tol= self.convergenceTestTol
    self.ctest.maxNumIter= self.maxNumIter
    self.soe= self.analysisAggregation.newSystemOfEqn("sparse_gen_col_lin_soe")
    self.solver= self.soe.newSolver("super_lu_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
    return self.analysis;
  def simpleNewtonRaphsonBandGen(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("simple")
    self.cHandler= self.sm.newConstraintHandler("plain_handler")
    analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
    self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
    self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("newton_raphson_soln_algo")
    self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
    self.ctest= self.analysisAggregation.newConvergenceTest("norm_unbalance_conv_test")
    self.ctest.tol= self.convergenceTestTol
    self.ctest.maxNumIter= self.maxNumIter
    self.soe= self.analysisAggregation.newSystemOfEqn("band_gen_lin_soe")
    self.solver= self.soe.newSolver("band_gen_lin_lapack_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
    return self.analysis;
  def simpleStaticModifiedNewton(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("simple")
    self.cHandler= self.sm.newConstraintHandler("plain_handler")
    analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
    self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
    self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("modified_newton_soln_algo")
    self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
    self.ctest= self.analysisAggregation.newConvergenceTest("relative_total_norm_disp_incr_conv_test")
    self.ctest.tol= 0.01 #Make this configurable
    self.ctest.maxNumIter= 150 #Make this configurable
    self.soe= self.analysisAggregation.newSystemOfEqn("sparse_gen_col_lin_soe")
    self.solver= self.soe.newSolver("super_lu_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
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
    analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
    self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
    self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("newton_raphson_soln_algo")
    self.ctest= self.analysisAggregation.newConvergenceTest("norm_unbalance_conv_test")
    self.ctest.tol= 1.0e-4
    self.ctest.maxNumIter= self.maxNumIter
    self.ctest.printFlag= self.printFlag
    self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
    self.soe= self.analysisAggregation.newSystemOfEqn("band_gen_lin_soe")
    self.solver= self.soe.newSolver("band_gen_lin_lapack_solver")
    self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
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
    analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
    self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
    self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("newton_raphson_soln_algo")
    self.ctest= self.analysisAggregation.newConvergenceTest("norm_disp_incr_conv_test")
    self.ctest.tol= 1.0e-3
    self.ctest.maxNumIter= self.maxNumIter
    self.ctest.printFlag= self.printFlag
    self.integ= self.analysisAggregation.newIntegrator("newmark_integrator",xc.Vector([]))
    self.soe= self.analysisAggregation.newSystemOfEqn("profile_spd_lin_soe")
    self.solver= self.soe.newSolver("profile_spd_lin_lapack_solver")
    self.analysis= self.solu.newAnalysis("direct_integration_analysis","analysisAggregation","")
    return self.analysis;

  def frequencyAnalysis(self,prb):
    self.solu= prb.getSoluProc
    self.solCtrl= self.solu.getSoluControl
    solModels= self.solCtrl.getModelWrapperContainer
    self.sm= solModels.newModelWrapper("sm")
    self.cHandler= self.sm.newConstraintHandler("transformation_constraint_handler")
    self.numberer= self.sm.newNumberer("default_numberer")
    self.numberer.useAlgorithm("rcm")
    analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
    self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
    self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("frequency_soln_algo")
    self.integ= self.analysisAggregation.newIntegrator("eigen_integrator",xc.Vector([1.0,1,1.0,1.0]))
    self.soe= self.analysisAggregation.newSystemOfEqn("sym_band_eigen_soe")
    self.solver= self.soe.newSolver("sym_band_eigen_solver")
    self.analysis= self.solu.newAnalysis("modal_analysis","analysisAggregation","")
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
  return solution.simpleNewtonRaphsonBandGen(prb)

def simple_static_modified_newton(prb):
  solution= SolutionProcedure()
  return solution.simpleStaticModifiedNewton(prb)

def penalty_newton_raphson(prb):
  solution= SolutionProcedure()
  return solution.penaltyNewtonRaphson(prb)

def frequency_analysis(prb):
  solution= SolutionProcedure()
  return solution.frequencyAnalysis(prb)

def resuelveComb(preprocessor,nmbComb,analysis,numSteps):
  preprocessor.resetLoadCase()
  preprocessor.getLoadLoader.addToDomain(nmbComb)
  analOk= analysis.analyze(numSteps)
  preprocessor.getLoadLoader.removeFromDomain(nmbComb)
  # print "Resuelta combinación: ",nmbComb,"\n"

def resuelveCombEstatLin(preprocessor,nmbComb,analysis,numSteps):
  print "DEPRECATED; use resuelveComb"
  resuelveComb(preprocessor,nmbComb,analysis,numSteps)


def resuelveCombEstat2ndOrderLin(preprocessor,nmbComb,analysis,numSteps):
  preprocessor.resetLoadCase()
  preprocessor.getLoadLoader.addToDomain(nmbComb)
  analOk= analysis.analyze(numSteps)
  analOk= analysis.analyze(numSteps)
  preprocessor.getLoadLoader.removeFromDomain(nmbComb)
  # print "Resuelta combinación: ",nmbComb,"\n"

def resuelveCasoEstatNoLin(nmbComb):
  print "DEPRECATED; use use resuelveComb"
  resuelveComb(preprocessor,nmbComb,analysis,numSteps)

# def resuelveCasoEstatNoLinNR(nmbComb):
#     \preprocessor \dom{\nuevo_caso
#     cargas.addToDomain(nmbComb)

#     \sol_proc
#       {
#         \control
#           {
#             \solu_method["analysisAggregation","sm"]
#               {
#                 \norm_unbalance_conv_test \tol{1.0e-2} \print_flag{0} \max_num_iter{10}
#               }
#           }
#         \static_analysis["analysisAggregation"]  \analyze{1} analOk= result 
#       }
#     cargas.removeFromDomain(nmbComb)
#     # print "Resuelto caso: ",nmbComb,"\n"
#   }
