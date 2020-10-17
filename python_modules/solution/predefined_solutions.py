# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division


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
    :ivar solutionStrategy: object that aggregates all the component objects
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
    _counter = 0 # Counts the objects of this type.
    
    def __init__(self, name= None, maxNumIter= 10, convergenceTestTol= 1e-9):
        ''' Constructor.

        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        '''
        SolutionProcedure._counter += 1
        self.id = SolutionProcedure._counter
        if(name):
            self.name= name
        else:
            self.name= str(self.id)
        self.solu= None
        self.solCtrl= None
        self.sm= None
        self.numberer= None
        self.cHandler= None
        self.solutionStrategy= None
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

    def getModelWrapperName(self):
        ''' Return the name for the model wrapper.'''
        return 'sm_'+self.name

    def defineModelWrapper(self, prb, numberingMethod= 'rcm'):
        ''' Defines the model wrapper.

        :param prb: XC finite element problem.
        :param numberginMethod: numbering method (plain or reverse Cuthill-McKee algorithm).
        '''
        self.solu= prb.getSoluProc
        self.solCtrl= self.solu.getSoluControl
        solModels= self.solCtrl.getModelWrapperContainer
        modelWrapperName= self.getModelWrapperName()
        self.sm= solModels.newModelWrapper(modelWrapperName)
        self.numberer= self.sm.newNumberer("default_numberer")
        self.numberer.useAlgorithm(numberingMethod)
        return modelWrapperName

    def defineIntegrator(self, integratorType= 'load_control_integrator'):
        ''' Define the type of integrator to use in the analysis.

        :param integratorType: type of integrator to use.
        '''
        if(integratorType in ['load_control_integrator','eigen_integrator', 'ill-conditioning_integrator']):
            self.integ= self.solutionStrategy.newIntegrator(integratorType,xc.Vector([]))
        elif(integratorType=='newmark_integrator'):
            self.integ= self.solutionStrategy.newIntegrator(integratorType,xc.Vector([0.5,0.25]))

    def getSolutionStrategyName(self):
        ''' Return the name for the model wrapper.'''
        return 'se_'+self.name
    
    def defineSolutionStrategy(self, solAlgType= 'linear_soln_algo', integratorType= 'load_control_integrator', convTestType= None):
        ''' Define the solution strategy.
        
        :param solAlgType: type of the solution algorithm (linear, Newton, modified Newton, ...)
        :param integratorType: type of integrator to use.
        :poram convTestType: convergence test for non linear analysis (norm unbalance,...).
        '''
        solutionStrategies= self.solCtrl.getSolutionStrategyContainer
        modelWrapperName= self.getModelWrapperName()
        solutionStrategyName= self.getSolutionStrategyName()
        self.solutionStrategy= solutionStrategies.newSolutionStrategy(solutionStrategyName, modelWrapperName)
        self.solAlgo= self.solutionStrategy.newSolutionAlgorithm(solAlgType)
        self.defineIntegrator(integratorType)
        if(convTestType):
            self.ctest= self.solutionStrategy.newConvergenceTest(convTestType)
            self.ctest.tol= self.convergenceTestTol
            self.ctest.maxNumIter= self.maxNumIter
            self.ctest.printFlag= self.printFlag

    def getConstraintHandler(self, cHType= None, alphaSP= 1e15, alphaMP= 1e15):
        ''' Define the constraint handler and return a reference to it.
        
        :param cHType: type of the constraint handler (plain, penalty, transformation or langrange).
        :param alphaSP: penalty factor on single points constraints (defaults to 1e15).
        :param alphaMP: penalty factor on multi-poing constraints (defaults to 1e15).
        '''
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
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.cHandler= self.getConstraintHandler('penalty')
        self.defineSolutionStrategy(solAlgType= 'linear_soln_algo', integratorType= 'load_control_integrator', convTestType= None)
        self.soe= self.solutionStrategy.newSystemOfEqn("band_spd_lin_soe")
        self.solver= self.soe.newSolver("band_spd_lin_lapack_solver")
        self.analysis= self.solu.newAnalysis("static_analysis", self.getSolutionStrategyName(),"")
        return self.analysis
      
    def plainLinearNewmark(self,prb):
        ''' Return a linear Newmark solution algorithm
            with a plain constraint handler.
        '''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'simple')
        self.cHandler= self.getConstraintHandler('plain')
        self.defineSolutionStrategy(solAlgType= 'linear_soln_algo', integratorType= 'newmark_integrator', convTestType= None)
        self.soe= self.solutionStrategy.newSystemOfEqn("band_gen_lin_soe")
        self.solver= self.soe.newSolver("band_gen_lin_lapack_solver")
        self.analysis= self.solu.newAnalysis("direct_integration_analysis", self.getSolutionStrategyName(),"")
        return self.analysis
      
    def simpleLagrangeStaticLinear(self,prb):
        ''' Return a linear static solution algorithm
            with a Lagrange constraint handler.
        '''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.cHandler= self.getConstraintHandler('lagrange')
        self.defineSolutionStrategy(solAlgType= 'linear_soln_algo', integratorType= 'load_control_integrator', convTestType= None)
        self.soe= self.solutionStrategy.newSystemOfEqn("sparse_gen_col_lin_soe")
        self.solver= self.soe.newSolver("super_lu_solver")
        self.analysis= self.solu.newAnalysis("static_analysis", self.getSolutionStrategyName(),"")
        return self.analysis
      
    def simpleTransformationStaticLinear(self,prb):
        ''' Return a linear static solution algorithm with a 
            transformation constraint handler.
        '''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.cHandler= self.getConstraintHandler('transformation')
        self.defineSolutionStrategy(solAlgType= 'linear_soln_algo', integratorType= 'load_control_integrator', convTestType= None)
        self.soe= self.solutionStrategy.newSystemOfEqn("sparse_gen_col_lin_soe")
        self.solver= self.soe.newSolver("super_lu_solver")
        self.analysis= self.solu.newAnalysis("static_analysis", self.getSolutionStrategyName(),"")
        return self.analysis
      
    def plainNewtonRaphson(self,prb):
        ''' Return a Newton-Raphson solution algorithm with a 
            plain constraint handler.
        '''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'simple')
        self.cHandler= self.getConstraintHandler('plain')
        self.defineSolutionStrategy(solAlgType= 'newton_raphson_soln_algo', integratorType= 'load_control_integrator', convTestType= 'norm_unbalance_conv_test')
        self.soe= self.solutionStrategy.newSystemOfEqn("sparse_gen_col_lin_soe")
        self.solver= self.soe.newSolver("super_lu_solver")
        self.analysis= self.solu.newAnalysis("static_analysis", self.getSolutionStrategyName(),"")
        return self.analysis
      
    def plainNewtonRaphsonBandGen(self,prb):
        ''' Return a Newton-Raphson solution algorithm with a 
            plain constraint handler and a band general
            SOE solver.
        '''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'simple')
        self.cHandler= self.getConstraintHandler('plain')
        self.defineSolutionStrategy(solAlgType= 'newton_raphson_soln_algo', integratorType= 'load_control_integrator', convTestType= 'norm_unbalance_conv_test')
        self.soe= self.solutionStrategy.newSystemOfEqn("band_gen_lin_soe")
        self.solver= self.soe.newSolver("band_gen_lin_lapack_solver")
        self.analysis= self.solu.newAnalysis("static_analysis", self.getSolutionStrategyName(),"")
        return self.analysis
      
    def plainStaticModifiedNewton(self,prb):
        ''' Return a static solution procedure with a modified Newton
            solution algorithm with a plain constraint handler.
        '''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'simple')
        self.cHandler= self.getConstraintHandler('plain')
        self.maxNumIter= 150 #Make this configurable
        self.defineSolutionStrategy(solAlgType= 'modified_newton_soln_algo', integratorType= 'load_control_integrator', convTestType= 'relative_total_norm_disp_incr_conv_test')
        self.soe= self.solutionStrategy.newSystemOfEqn("sparse_gen_col_lin_soe")
        self.solver= self.soe.newSolver("super_lu_solver")
        self.analysis= self.solu.newAnalysis("static_analysis", self.getSolutionStrategyName(),"")
        return self.analysis
    
    def plainKrylovNewton(self,prb, maxDim= 6):
        ''' Return a static solution procedure with a modified Newton
            solution algorithm with a plain constraint handler.

        See appendix C. Analysis model script of the document
        "Finite Element Modeling of Gusset Plate Failure Using Opensees"
        Andrew J. Walker. Oregon State University
        '''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'simple')
        self.cHandler= self.getConstraintHandler('plain')
        self.maxNumIter= 150 #Make this configurable
        self.defineSolutionStrategy(solAlgType= 'krylov_newton_soln_algo', integratorType= 'load_control_integrator', convTestType= 'energy_inc_conv_test')
        self.soe= self.solutionStrategy.newSystemOfEqn("umfpack_gen_lin_soe")
        self.solver= self.soe.newSolver("umfpack_gen_lin_solver")
        self.analysis= self.solu.newAnalysis("static_analysis", self.getSolutionStrategyName(),"")
        return self.analysis
      
    def penaltyNewtonRaphson(self, prb):
        ''' Return a static solution procedure with a Newton Raphson algorithm
            and a penalty constraint handler.'''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.cHandler= self.getConstraintHandler('penalty')
        self.defineSolutionStrategy(solAlgType= 'newton_raphson_soln_algo', integratorType= 'load_control_integrator', convTestType= 'norm_unbalance_conv_test')
        self.soe= self.solutionStrategy.newSystemOfEqn("band_gen_lin_soe")
        self.solver= self.soe.newSolver("band_gen_lin_lapack_solver")
        self.analysis= self.solu.newAnalysis("static_analysis", self.getSolutionStrategyName(),"")
        return self.analysis

    def penaltyModifiedNewton(self, prb):
        ''' Return a static solution procedure with a Newton Raphson algorithm
            and a penalty constraint handler.'''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.cHandler= self.getConstraintHandler('penalty')
        self.maxNumIter= 150 #Make this configurable
        self.defineSolutionStrategy(solAlgType= 'modified_newton_soln_algo', integratorType= 'load_control_integrator', convTestType= 'relative_total_norm_disp_incr_conv_test')
        self.soe= self.solutionStrategy.newSystemOfEqn("sparse_gen_col_lin_soe")
        self.solver= self.soe.newSolver("super_lu_solver")
        self.analysis= self.solu.newAnalysis("static_analysis", self.getSolutionStrategyName(),"")
        return self.analysis
    
    def penaltyNewmarkNewtonRapshon(self,prb):
        ''' Return a Newmark solution procedure with a Newton Raphson algorithm
            and a penalty constraint handler.'''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.cHandler= self.getConstraintHandler('penalty', alphaSP= 1.0e18, alphaMP= 1.0e18)
        self.defineSolutionStrategy(solAlgType= 'newton_raphson_soln_algo', integratorType= 'newmark_integrator', convTestType= 'norm_disp_incr_conv_test')
        self.soe= self.solutionStrategy.newSystemOfEqn("profile_spd_lin_soe")
        self.solver= self.soe.newSolver("profile_spd_lin_direct_solver")
        self.analysis= self.solu.newAnalysis("direct_integration_analysis", self.getSolutionStrategyName(),"")
        return self.analysis

    def frequencyAnalysis(self,prb,systemPrefix= 'sym_band'):
        ''' Return a natural frequency computation procedure.'''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.cHandler= self.getConstraintHandler('transformation')
        self.defineSolutionStrategy(solAlgType= 'frequency_soln_algo', integratorType= 'eigen_integrator', convTestType= None)
        soe_string= systemPrefix+'_eigen_soe'
        solver_string= systemPrefix+'_eigen_solver'
        self.soe= self.solutionStrategy.newSystemOfEqn(soe_string)
        self.solver= self.soe.newSolver(solver_string)
        self.analysis= self.solu.newAnalysis("modal_analysis", self.getSolutionStrategyName(),"")
        return self.analysis
    
    def illConditioningAnalysisBase(self, prb, soePrefix= 'sym_band_eigen', shift= None):
        ''' Prepares the components of an ill-conditioning
            analysis.
        '''
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.cHandler= self.getConstraintHandler('penalty')
        self.defineSolutionStrategy(solAlgType= 'ill-conditioning_soln_algo', integratorType= 'ill-conditioning_integrator', convTestType= None)
        self.soe= self.solutionStrategy.newSystemOfEqn(soePrefix+"_soe")
        if(shift):
            self.soe.shift= shift
        self.solver= self.soe.newSolver(soePrefix+"_solver")

        self.analysis= self.solu.newAnalysis("ill-conditioning_analysis", self.getSolutionStrategyName(),"")
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

def plain_krylov_newton(prb, mxNumIter= 300, convergenceTestTol= 1e-9, printFlag= 0):
    ''' Return a plain Krylov Newton solution procedure.

    :ivar maxNumIter: maximum number of iterations (defauts to 300)
    :ivar convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :ivar printFlag: print message on each iteration
    '''
    solution= SolutionProcedure()
    solution.maxNumIter= mxNumIter
    solution.convergenceTestTol= convergenceTestTol
    solution.printFlag= printFlag
    return solution.plainKrylovNewton(prb)

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

def penalty_modified_newton(prb, mxNumIter= 10, convergenceTestTol= 1e-4, printFlag= 0):
    ''' Return a simple static modified Newton solution procedure.

    :ivar maxNumIter: maximum number of iterations (defauts to 10)
    :ivar convergenceTestTol: convergence tolerance (defaults to 1e-9)
    '''
    solution= SolutionProcedure()
    solution.maxNumIter= mxNumIter
    solution.convergenceTestTol= convergenceTestTol
    solution.printFlag= printFlag
    return solution.penaltyModifiedNewton(prb)

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

