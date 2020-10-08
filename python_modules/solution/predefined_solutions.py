# -*- coding: utf-8 -*-
from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.ortega.ort@gmail.com"

import xc_base
import geom
import xc
from misc_utils import log_messages as lmsg

class SolutionProcedure(object):
    '''
    :ivar analysisAggregation: object that aggregates all the component objects
          that define the type of analysis (constraint handler, DOF_Numberer, 
          integrator, solution algorithm, system of equations, convergence test)
    :ivar cHandler:  constraint handler. Determines how the constraint equations 
          are enforced in the analysis, how it handles the boundary
          conditions/imposed displacements 
    :ivar numberer:  DOF numberer. Determines the mapping between equation 
          numbers and degrees of freedom (how DOF are numbered)
    :ivar integ: integrator, used to:

                   - determine the predictive step for time t+dt
                   - specify the tangent matrix and residual vector at any
                     iteration
                   - determine the corrective step based on the displacement 
                     increment
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
    :ivar soe:       sparse system of equations: band general, band SPD, 
                     profile SPD, sparse general, umfPack, sparse SPD
    :ivar solver:
    :ivar analysis:  determines what type of analysis is to be performed
    :ivar convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :ivar maxNumIter: maximum number of iterations (defauts to 10)
    :ivar solu:
    :ivar solCtrl:
    :ivar sm:
    '''
    def __init__(self, maxNumIter= 10, convergenceTestTol= 1e-9):
        ''' Constructor.

        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        '''
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
        
    def clear(self):
        ''' Wipe out the solution procedure.'''
        self.solu.clear()

    def getConstraintHandler(self, cHType= None, alphaSP= 1e15, alphaMP= 1e15):
        ''' Return the constraint handler.'''
        cHandler= None
        if(cHType):
            self.cHandlerType= cHType
        if(self.cHandlerType=='penalty'):
            cHandler= self.sm.newConstraintHandler("penalty_constraint_handler")
            cHandler.alphaSP= alphaSP
            cHandler.alphaMP= alphaMP
        elif(self.cHandlerType=='transformation'):
            cHandler= self.sm.newConstraintHandler("transformation_constraint_handler")
        elif(self.cHandlerType=='lagrange'):
            self.sm.newConstraintHandler("lagrange_constraint_handler")
        elif(self.cHandlerType=='plain'):
            cHandler= self.sm.newConstraintHandler("plain_handler")
        else:
            lmsg.error('unknown constraint handler type: '+self.cHandlerType)
        return cHandler
            

    def simpleStaticLinear(self,prb):
        ''' Return a linear static solution algorithm
            with a penalty constraint handler.
        '''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        self.sm= solModels.newModelWrapper("sm")
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("rcm")
        self.cHandler= self.getConstraintHandler('penalty')
        analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
        self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
        self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("linear_soln_algo")
        self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
        self.soe= self.analysisAggregation.newSystemOfEqn("band_spd_lin_soe")
        self.solver= self.soe.newSolver("band_spd_lin_lapack_solver")
        self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
        return self.analysis
      
    def plainLinearNewmark(self,prb):
        ''' Return a linear Newmark solution algorithm
            with a plain constraint handler.
        '''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        self.sm= solModels.newModelWrapper("sm")
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("simple")
        self.cHandler= self.getConstraintHandler('plain')
        analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
        self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
        self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("linear_soln_algo")
        self.integ= self.analysisAggregation.newIntegrator("newmark_integrator",xc.Vector([0.5,0.25]))
        self.soe= self.analysisAggregation.newSystemOfEqn("band_gen_lin_soe")
        self.solver= self.soe.newSolver("band_gen_lin_lapack_solver")
        self.analysis= self.solu.newAnalysis("direct_integration_analysis","analysisAggregation","")
        return self.analysis
      
    def simpleLagrangeStaticLinear(self,prb):
        ''' Return a linear static solution algorithm
            with a Lagrange constraint handler.
        '''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        self.sm= solModels.newModelWrapper("sm")
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("rcm")
        self.cHandler= self.getConstraintHandler('lagrange')
        analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
        self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
        self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("linear_soln_algo")
        self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
        self.soe= self.analysisAggregation.newSystemOfEqn("sparse_gen_col_lin_soe")
        self.solver= self.soe.newSolver("super_lu_solver")
        self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
        return self.analysis
      
    def simpleTransformationStaticLinear(self,prb):
        ''' Return a linear static solution algorithm with a 
            transformation constraint handler.
        '''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        self.sm= solModels.newModelWrapper("sm")
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("rcm")
        self.cHandler= self.getConstraintHandler('transformation')
        analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
        self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
        self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("linear_soln_algo")
        self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
        self.soe= self.analysisAggregation.newSystemOfEqn("sparse_gen_col_lin_soe")
        self.solver= self.soe.newSolver("super_lu_solver")
        self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
        return self.analysis
      
    def plainNewtonRaphson(self,prb):
        ''' Return a Newton-Raphson solution algorithm with a 
            plain constraint handler.
        '''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        self.sm= solModels.newModelWrapper("sm")
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("simple")
        self.cHandler= self.getConstraintHandler('plain')
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
        return self.analysis
      
    def plainNewtonRaphsonBandGen(self,prb):
        ''' Return a Newton-Raphson solution algorithm with a 
            plain constraint handler and a band general
            SOE solver.
        '''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        self.sm= solModels.newModelWrapper("sm")
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("simple")
        self.cHandler= self.getConstraintHandler('plain')
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
        return self.analysis
      
    def plainStaticModifiedNewton(self,prb):
        ''' Return a static solution procedure with a modified Newton
            solution algorithm with a plain constraint handler.
        '''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        self.sm= solModels.newModelWrapper("sm")
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("simple")
        self.cHandler= self.getConstraintHandler('plain')
        analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
        self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
        self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("modified_newton_soln_algo")
        self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
        self.ctest= self.analysisAggregation.newConvergenceTest("relative_total_norm_disp_incr_conv_test")
        self.ctest.tol= self.convergenceTestTol
        self.ctest.maxNumIter= 150 #Make this configurable
        self.soe= self.analysisAggregation.newSystemOfEqn("sparse_gen_col_lin_soe")
        self.solver= self.soe.newSolver("super_lu_solver")
        self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
        return self.analysis
    
    def plainKrylovNewton(self,prb, maxDim= 6):
        ''' Return a static solution procedure with a modified Newton
            solution algorithm with a plain constraint handler.

        See appendix C. Analysis model script of the document
        "Finite Element Modeling of Gusset Plate Failure Using Opensees"
        Andrew J. Walker. Oregon State University
        '''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        self.sm= solModels.newModelWrapper("sm")
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("simple")
        self.cHandler= self.getConstraintHandler('plain')
        analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
        self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
        self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("krylov_newton_soln_algo")
        self.solAlgo.maxDimension= maxDim
        self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
        self.ctest= self.analysisAggregation.newConvergenceTest("energy_inc_conv_test")
        self.ctest.tol= self.convergenceTestTol
        self.ctest.maxNumIter= 150 #Make this configurable
        self.ctest.printFlag= self.printFlag
        self.soe= self.analysisAggregation.newSystemOfEqn("umfpack_gen_lin_soe")
        self.solver= self.soe.newSolver("umfpack_gen_lin_solver")
        self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
        return self.analysis
      
    def penaltyNewtonRaphson(self, prb):
        ''' Return a static solution procedure with a Newton Raphson algorithm
            and a penalty constraint handler.'''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        self.sm= solModels.newModelWrapper("sm")
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("rcm")
        self.cHandler= self.getConstraintHandler('penalty')
        analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
        self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
        self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("newton_raphson_soln_algo")
        self.ctest= self.analysisAggregation.newConvergenceTest("norm_unbalance_conv_test")
        self.ctest.tol= self.convergenceTestTol #1.0e-4
        self.ctest.maxNumIter= self.maxNumIter
        self.ctest.printFlag= self.printFlag
        self.integ= self.analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
        self.soe= self.analysisAggregation.newSystemOfEqn("band_gen_lin_soe")
        self.solver= self.soe.newSolver("band_gen_lin_lapack_solver")
        self.analysis= self.solu.newAnalysis("static_analysis","analysisAggregation","")
        return self.analysis

    def penaltyNewmarkNewtonRapshon(self,prb):
        ''' Return a Newmark solution procedure with a Newton Raphson algorithm
            and a penalty constraint handler.'''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        self.sm= solModels.newModelWrapper("sm")
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("rcm")
        self.cHandler= self.getConstraintHandler('penalty', alphaSP= 1.0e18, alphaMP= 1.0e18)
        analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
        self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
        self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("newton_raphson_soln_algo")
        self.ctest= self.analysisAggregation.newConvergenceTest("norm_disp_incr_conv_test")
        self.ctest.tol= self.convergenceTestTol # 1.0e-3
        self.ctest.maxNumIter= self.maxNumIter
        self.ctest.printFlag= self.printFlag
        self.integ= self.analysisAggregation.newIntegrator("newmark_integrator",xc.Vector([]))
        self.soe= self.analysisAggregation.newSystemOfEqn("profile_spd_lin_soe")
        self.solver= self.soe.newSolver("profile_spd_lin_direct_solver")
        self.analysis= self.solu.newAnalysis("direct_integration_analysis","analysisAggregation","")
        return self.analysis

    def frequencyAnalysis(self,prb,systemPrefix= 'sym_band'):
        ''' Return a natural frequency computation procedure.'''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        self.sm= solModels.newModelWrapper("sm")
        self.cHandler= self.getConstraintHandler('transformation')
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("rcm")
        analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
        self.analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
        self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("frequency_soln_algo")
        self.integ= self.analysisAggregation.newIntegrator("eigen_integrator",xc.Vector([]))
        soe_string= systemPrefix+'_eigen_soe'
        solver_string= systemPrefix+'_eigen_solver'
        self.soe= self.analysisAggregation.newSystemOfEqn(soe_string)
        self.solver= self.soe.newSolver(solver_string)
        self.analysis= self.solu.newAnalysis("modal_analysis","analysisAggregation","")
        return self.analysis
    
    def illConditioningAnalysisBase(self, prb, soePrefix= 'sym_band_eigen', shift= None):
        ''' Prepares the components of an ill-conditioning
            analysis.
        '''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        self.solModels= self.solCtrl.getModelWrapperContainer
        self.sm= self.solModels.newModelWrapper("sm")
        self.cHandler= self.getConstraintHandler('penalty')
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm("rcm")
        self.analysisAggregations= self.solCtrl.getAnalysisAggregationContainer
        self.analysisAggregation= self.analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")

        self.solAlgo= self.analysisAggregation.newSolutionAlgorithm("ill-conditioning_soln_algo")
        self.integ= self.analysisAggregation.newIntegrator("ill-conditioning_integrator",xc.Vector([]))
        self.soe= self.analysisAggregation.newSystemOfEqn(soePrefix+"_soe")
        if(shift):
            self.soe.shift= shift
        self.solver= self.soe.newSolver(soePrefix+"_solver")

        self.analysis= self.solu.newAnalysis("ill-conditioning_analysis","analysisAggregation","")
        return self.analysis
    
    def zeroEnergyModes(self, prb):
        ''' Prepares the components to obtain the zero energy modes
            of the finite element model.'''
        # Very slow solver but apparently Arpack cannot
        # compute zero eigenvalues. XXX
        return self.illConditioningAnalysisBase(prb,soePrefix= 'sym_band_eigen')

    def illConditioningAnalysis(self, prb):
        ''' Prepares the components to obtain the modes
            associated with very small eigenvalues of the
            stiffness matrix.'''
        analysis= self.illConditioningAnalysisBase(prb,soePrefix= 'band_arpack', shift= 0.0)
        return analysis

#Typical solution procedures.

## Linear static analysis.
def simple_static_linear(prb):
    ''' Return a simple static linear solution procedure.'''
    solution= SolutionProcedure()
    return solution.simpleStaticLinear(prb)

## Non-linear static analysis.
def plain_newton_raphson(prb, mxNumIter= 10):
    solution= SolutionProcedure()
    solution.maxNumIter= mxNumIter
    return solution.plainNewtonRaphson(prb)

def plain_newton_raphson_band_gen(prb, mxNumIter= 10):
    solution= SolutionProcedure()
    solution.maxNumIter= mxNumIter
    return solution.plainNewtonRaphsonBandGen(prb)

def plain_static_modified_newton(prb, mxNumIter= 10, convergenceTestTol= .01):
    ''' Return a simple static modified Newton solution procedure.

    :ivar maxNumIter: maximum number of iterations (defauts to 10)
    :ivar convergenceTestTol: convergence tolerance (defaults to 1e-9)
    '''
    solution= SolutionProcedure()
    solution.maxNumIter= mxNumIter
    solution.convergenceTestTol= convergenceTestTol
    return solution.plainStaticModifiedNewton(prb)

def penalty_newton_raphson(prb, mxNumIter= 10, convergenceTestTol= 1e-4, printFlag= 0):
    ''' Return a penalty Newton-Raphson solution procedure.

    :ivar maxNumIter: maximum number of iterations (defauts to 10)
    :ivar convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :ivar printFlag: print message on each iteration
    '''
    solution= SolutionProcedure()
    solution.maxNumIter= mxNumIter
    solution.convergenceTestTol= convergenceTestTol
    solution.printFlag= printFlag
    return solution.penaltyNewtonRaphson(prb)

def plain_krylov_newton(prb, mxNumIter= 10, convergenceTestTol= 1e-4, printFlag= 0):
    ''' Return a penalty Newton-Raphson solution procedure.

    :ivar maxNumIter: maximum number of iterations (defauts to 10)
    :ivar convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :ivar printFlag: print message on each iteration
    '''
    solution= SolutionProcedure()
    solution.maxNumIter= mxNumIter
    solution.convergenceTestTol= convergenceTestTol
    solution.printFlag= printFlag
    return solution.plainKrylovNewton(prb)

def frequency_analysis(prb):
    ''' Return a solution procedure that computes the natural
        frequencies of the model.'''
    solution= SolutionProcedure()
    return solution.frequencyAnalysis(prb)

def zero_energy_modes(prb):
    ''' Return a solution procedure that computes the zero
        energy modes of the model.'''
    solution= SolutionProcedure()
    return solution.zeroEnergyModes(prb)

def ill_conditioning_analysis(prb):
    ''' Return a solution procedure that computes the modes
        that correspond to ill-conditioned degrees of freedom.'''
    solution= SolutionProcedure()
    return solution.illConditioningAnalysis(prb)

def solveComb(preprocessor,nmbComb,analysis,numSteps):
    preprocessor.resetLoadCase()
    preprocessor.getLoadHandler.addToDomain(nmbComb)
    analOk= analysis.analyze(numSteps)
    preprocessor.getLoadHandler.removeFromDomain(nmbComb)
    # print("Resuelta combinación: ",nmbComb,"\n")

def solveStaticLinearComb(preprocessor,nmbComb,analysis,numSteps):
    print("DEPRECATED; use solveComb")
    solveComb(preprocessor,nmbComb,analysis,numSteps)

def solveCombEstat2ndOrderLin(preprocessor,nmbComb,analysis,numSteps):
    preprocessor.resetLoadCase()
    preprocessor.getLoadHandler.addToDomain(nmbComb)
    analOk= analysis.analyze(numSteps)
    analOk= analysis.analyze(numSteps)
    preprocessor.getLoadHandler.removeFromDomain(nmbComb)
    # print("Resuelta combinación: ",nmbComb,"\n")

def solveStaticNoLinCase(nmbComb):
    print("DEPRECATED; use use solveComb")
    solveComb(preprocessor,nmbComb,analysis,numSteps)

# def solveStaticNoLinCaseNR(nmbComb):
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
#     # print("Resuelto caso: ",nmbComb,"\n")
#   }
