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
    
    def __init__(self, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0):
        ''' Constructor.

        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
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
        self.convergenceTestTol= convergenceTestTol
        self.maxNumIter= maxNumIter
        self.printFlag= printFlag
        
    def clear(self):
        ''' Wipe out the solution procedure.'''
        self.solu.clear()

    def getModelWrapperName(self):
        ''' Return the name for the model wrapper.'''
        return 'sm_'+self.name

    def defineModelWrapper(self, prb, numberingMethod= 'rcm'):
        ''' Defines the model wrapper.

        :param prb: XC finite element problem.
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee algorithm).
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
    
    def defineSolutionAlgorithm(self, solAlgType= 'linear_soln_algo', integratorType= 'load_control_integrator', convTestType= None):
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

    def defineSysOfEq(self, soeType, solverType):
        ''' Defines the solver to use for the resulting system of
            equations.

        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        '''
        self.soe= self.solutionStrategy.newSystemOfEqn(soeType)
        self.solver= self.soe.newSolver(solverType)

    def defineConstraintHandler(self, cHType= None, alphaSP= 1e15, alphaMP= 1e15):
        ''' Define the constraint handler and return a reference to it.
        
        :param cHType: type of the constraint handler (plain, penalty, transformation or langrange).
        :param alphaSP: penalty factor on single points constraints (defaults to 1e15).
        :param alphaMP: penalty factor on multi-poing constraints (defaults to 1e15).
        '''
        if(cHType):
            self.cHandlerType= cHType
        if(self.cHandlerType=='penalty'):
            self.cHandler= self.sm.newConstraintHandler("penalty_constraint_handler")
            self.cHandler.alphaSP= alphaSP
            self.cHandler.alphaMP= alphaMP
        elif(self.cHandlerType=='transformation'):
            self.cHandler= self.sm.newConstraintHandler("transformation_constraint_handler")
        elif(self.cHandlerType=='lagrange'):
            self.cHandler= self.sm.newConstraintHandler("lagrange_constraint_handler")
        elif(self.cHandlerType=='plain'):
            self.cHandler= self.sm.newConstraintHandler("plain_handler")
        else:
            lmsg.error('unknown constraint handler type: '+self.cHandlerType)

    def defineAnalysis(self, analysisType= 'static_analysis'):
        ''' Define the analysis object.

        :param analysisType: type of the analysis to perform.
        '''
        self.analysis= self.solu.newAnalysis(analysisType, self.getSolutionStrategyName(),"")
                                    

#Typical solution procedures.

## Linear static analysis.
class SimpleStaticLinear(SolutionProcedure):
    ''' Return a linear static solution algorithm
        with a penalty constraint handler.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        '''
        super(SimpleStaticLinear,self).__init__(name, maxNumIter, convergenceTestTol, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.defineConstraintHandler('penalty')
        self.defineSolutionAlgorithm(solAlgType= 'linear_soln_algo', integratorType= 'load_control_integrator', convTestType= None)
        self.defineSysOfEq(soeType= 'band_spd_lin_soe', solverType= 'band_spd_lin_lapack_solver')
        self.defineAnalysis('static_analysis')

### Convenience function.
def simple_static_linear(prb):
    ''' Return a simple static linear solution procedure.'''
    solProc= SimpleStaticLinear(prb)
    return solProc.analysis

class SimpleLagrangeStaticLinear(SolutionProcedure):
    ''' Linear static solution algorithm
        with a Lagrange constraint handler.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        '''
        super(SimpleLagrangeStaticLinear,self).__init__(name, maxNumIter, convergenceTestTol, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.defineConstraintHandler('lagrange')
        self.defineSolutionAlgorithm(solAlgType= 'linear_soln_algo', integratorType= 'load_control_integrator', convTestType= None)
        self.defineSysOfEq(soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver')
        self.defineAnalysis('static_analysis')
        
class SimpleTransformationStaticLinear(SolutionProcedure):
    ''' Linear static solution algorithm with a 
        transformation constraint handler.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        '''
        super(SimpleTransformationStaticLinear,self).__init__(name, maxNumIter, convergenceTestTol, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.defineConstraintHandler('transformation')
        self.defineSolutionAlgorithm(solAlgType= 'linear_soln_algo', integratorType= 'load_control_integrator', convTestType= None)
        self.defineSysOfEq(soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver')
        self.defineAnalysis('static_analysis')

## Non-linear static analysis.
class PlainNewtonRaphson(SolutionProcedure):
    ''' Newton-Raphson solution algorithm with a 
        plain constraint handler.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        '''
        super(PlainNewtonRaphson,self).__init__(name, maxNumIter, convergenceTestTol, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'simple')
        self.defineConstraintHandler('plain')
        self.defineSolutionAlgorithm(solAlgType= 'newton_raphson_soln_algo', integratorType= 'load_control_integrator', convTestType= 'norm_unbalance_conv_test')
        self.defineSysOfEq(soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver')
        self.defineAnalysis('static_analysis')

### Convenience function
def plain_newton_raphson(prb, mxNumIter= 10):
    solProc= PlainNewtonRaphson(prb, maxNumIter= mxNumIter)
    return solProc.analysis

class PlainNewtonRaphsonBandGen(SolutionProcedure):
    ''' Newton-Raphson solution algorithm with a 
        plain constraint handler and a band general
        SOE solver.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        '''
        super(PlainNewtonRaphsonBandGen,self).__init__(name, maxNumIter, convergenceTestTol, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'simple')
        self.defineConstraintHandler('plain')
        self.defineSolutionAlgorithm(solAlgType= 'newton_raphson_soln_algo', integratorType= 'load_control_integrator', convTestType= 'norm_unbalance_conv_test')
        self.defineSysOfEq(soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver')
        self.defineAnalysis('static_analysis')

### Convenience function
def plain_newton_raphson_band_gen(prb, mxNumIter= 10):
    solProc= PlainNewtonRaphsonBandGen(prb, maxNumIter= mxNumIter)
    return solProc.analysis

class PenaltyNewtonRaphson(SolutionProcedure):
    ''' Return a static solution procedure with a Newton Raphson algorithm
        and a penalty constraint handler.'''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        '''
        super(PenaltyNewtonRaphson,self).__init__(name, maxNumIter, convergenceTestTol, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.defineConstraintHandler('penalty')
        self.defineSolutionAlgorithm(solAlgType= 'newton_raphson_soln_algo', integratorType= 'load_control_integrator', convTestType= 'norm_unbalance_conv_test')
        self.defineSysOfEq(soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver')
        self.defineAnalysis('static_analysis')

### Convenience function
def penalty_newton_raphson(prb, mxNumIter= 10, convergenceTestTol= 1e-4, printFlag= 0):
    ''' Return a penalty Newton-Raphson solution procedure.

    :param maxNumIter: maximum number of iterations (defauts to 10)
    :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :param printFlag: print message on each iteration
    '''
    solProc= PenaltyNewtonRaphson(prb, maxNumIter= mxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag)
    return solProc.analysis

class PlainStaticModifiedNewton(SolutionProcedure):
    ''' Static solution procedure with a modified Newton
        solution algorithm with a plain constraint handler.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        '''
        super(PlainStaticModifiedNewton,self).__init__(name, maxNumIter, convergenceTestTol, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'simple')
        self.defineConstraintHandler('plain')
        self.maxNumIter= 150 #Make this configurable
        self.defineSolutionAlgorithm(solAlgType= 'modified_newton_soln_algo', integratorType= 'load_control_integrator', convTestType= 'relative_total_norm_disp_incr_conv_test')
        self.defineSysOfEq(soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver')
        self.defineAnalysis('static_analysis')
    
### Convenience function
def plain_static_modified_newton(prb, mxNumIter= 10, convergenceTestTol= .01):
    ''' Return a simple static modified Newton solution procedure.

    :ivar maxNumIter: maximum number of iterations (defauts to 10)
    :ivar convergenceTestTol: convergence tolerance (defaults to 1e-9)
    '''
    solProc= PlainStaticModifiedNewton(prb, maxNumIter= mxNumIter, convergenceTestTol= convergenceTestTol)
    return solProc.analysis

class PenaltyModifiedNewton(SolutionProcedure):
    ''' Static solution procedure with a modified Newton algorithm
        and a penalty constraint handler.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        '''
        super(PenaltyModifiedNewton,self).__init__(name, maxNumIter, convergenceTestTol, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.defineConstraintHandler('penalty')
        self.defineSolutionAlgorithm(solAlgType= 'modified_newton_soln_algo', integratorType= 'load_control_integrator', convTestType= 'relative_total_norm_disp_incr_conv_test')
        self.defineSysOfEq(soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver')
        self.defineAnalysis('static_analysis')
        
### Convenience function
def penalty_modified_newton(prb, mxNumIter= 10, convergenceTestTol= 1e-4, printFlag= 0):
    ''' Return a simple static modified Newton solution procedure.

    :param maxNumIter: maximum number of iterations (defauts to 10)
    :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
    '''
    solProc= PenaltyModifiedNewton(prb,maxNumIter= mxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag)
    return solProc.analysis

class PlainKrylovNewton(SolutionProcedure):
    ''' KrylovNewton algorithm object which uses a Krylov subspace 
        accelerator to accelerate the convergence of the modified 
        newton method.

    See appendix C. Analysis model script of the document
    "Finite Element Modeling of Gusset Plate Failure Using Opensees"
    Andrew J. Walker. Oregon State University
    '''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, maxDim= 6):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param maxDim: max number of iterations until the tangent is reformed and the acceleration restarts (default = 6).
        '''
        super(PlainKrylovNewton,self).__init__(name, maxNumIter, convergenceTestTol, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'simple')
        self.defineConstraintHandler('plain')
        self.defineSolutionAlgorithm(solAlgType= 'krylov_newton_soln_algo', integratorType= 'load_control_integrator', convTestType= 'energy_inc_conv_test')
        self.solAlgo.maxDimension= maxDim
        self.defineSysOfEq(soeType= 'umfpack_gen_lin_soe', solverType= 'umfpack_gen_lin_solver')
        self.defineAnalysis('static_analysis')

### Convenience function
def plain_krylov_newton(prb, mxNumIter= 300, convergenceTestTol= 1e-9, printFlag= 0, maxDim= 6):
    ''' Return a plain Krylov Newton solution procedure.

    :param maxNumIter: maximum number of iterations (defauts to 300)
    :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :param printFlag: print message on each iteration
    :param maxDim: max number of iterations until the tangent is reformed and the acceleration restarts (default = 6).
    '''
    solProc= PlainKrylovNewton(prb, maxNumIter= mxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, maxDim= 6)
    return solProc.analysis

## Dynamic analysis        
class PlainLinearNewmark(SolutionProcedure):
    ''' Return a linear Newmark solution algorithm
        with a plain constraint handler.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        '''
        super(PlainLinearNewmark,self).__init__(name, maxNumIter, convergenceTestTol, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'simple')
        self.defineConstraintHandler('plain')
        self.defineSolutionAlgorithm(solAlgType= 'linear_soln_algo', integratorType= 'newmark_integrator', convTestType= None)
        self.defineSysOfEq(soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver')
        self.defineAnalysis('direct_integration_analysis')

class PenaltyNewmarkNewtonRapshon(SolutionProcedure):
    ''' Newmark solution procedure with a Newton Raphson algorithm
        and a penalty constraint handler.'''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        '''
        super(PenaltyNewmarkNewtonRapshon,self).__init__(name, maxNumIter, convergenceTestTol, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.defineConstraintHandler('penalty', alphaSP= 1.0e18, alphaMP= 1.0e18)
        self.defineSolutionAlgorithm(solAlgType= 'newton_raphson_soln_algo', integratorType= 'newmark_integrator', convTestType= 'norm_disp_incr_conv_test')
        self.defineSysOfEq(soeType= 'profile_spd_lin_soe', solverType= 'profile_spd_lin_direct_solver')
        self.defineAnalysis('direct_integration_analysis')


## Eigen analysis
class FrequencyAnalysis(SolutionProcedure):
    ''' Return a natural frequency computation procedure.'''

    def __init__(self, prb, name= None, printFlag= 0, systemPrefix= 'sym_band'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        '''        
        super(FrequencyAnalysis,self).__init__(name, printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.defineConstraintHandler('transformation')
        self.defineSolutionAlgorithm(solAlgType= 'frequency_soln_algo', integratorType= 'eigen_integrator', convTestType= None)
        soe_string= systemPrefix+'_eigen_soe'
        solver_string= systemPrefix+'_eigen_solver'
        self.defineSysOfEq(soeType= soe_string, solverType= solver_string)
        self.defineAnalysis('modal_analysis')
        
### Convenience function
def frequency_analysis(prb):
    ''' Return a solution procedure that computes the natural
        frequencies of the model.'''
    solProc= FrequencyAnalysis(prb)
    return solProc.analysis

class IllConditioningAnalysisBase(SolutionProcedure):
    ''' Base class for ill-conditioning
        solution procedures.
    '''
    def __init__(self, prb, name= None, printFlag= 0, systemPrefix= 'sym_band_eigen', shift= None):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        '''        
        super(IllConditioningAnalysisBase,self).__init__(name, printFlag= printFlag)
        modelWrapperName= self.defineModelWrapper(prb, numberingMethod= 'rcm')
        self.defineConstraintHandler('penalty')
        self.defineSolutionAlgorithm(solAlgType= 'ill-conditioning_soln_algo', integratorType= 'ill-conditioning_integrator', convTestType= None)
        self.defineSysOfEq(soeType= systemPrefix+"_soe", solverType= systemPrefix+"_solver")
        if(shift):
            self.soe.shift= shift
        self.defineAnalysis('ill-conditioning_analysis')
    
class ZeroEnergyModes(IllConditioningAnalysisBase):
    ''' Procedure to obtain the zero energy modes
        of the finite element model.'''
    def __init__(self, prb, name= None, printFlag= 0, systemPrefix= 'sym_band_eigen', shift= None):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        '''        
        # Very slow solver but apparently Arpack cannot
        # compute zero eigenvalues. XXX
        super(ZeroEnergyModes,self).__init__(prb, name, printFlag= printFlag, systemPrefix= systemPrefix, shift= shift)
        
### Convenience function
def zero_energy_modes(prb):
    ''' Return a solution procedure that computes the zero
        energy modes of the model.'''
    solProc= ZeroEnergyModes(prb)
    return solProc.analysis

class IllConditioningAnalysis(IllConditioningAnalysisBase):
    ''' Procedure to obtain the modes
        associated with very small eigenvalues of the
        stiffness matrix.'''
    def __init__(self, prb, name= None, printFlag= 0, systemPrefix= 'band_arpack', shift= 0.0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        '''        
        # Very slow solver but apparently Arpack cannot
        # compute zero eigenvalues. XXX
        super(IllConditioningAnalysis,self).__init__(prb, name, printFlag= printFlag, systemPrefix= systemPrefix, shift= shift)
    
### Convenience function
def ill_conditioning_analysis(prb):
    ''' Return a solution procedure that computes the modes
        that correspond to ill-conditioned degrees of freedom.'''
    solProc= IllConditioningAnalysis(prb)
    return solProc.analysis

## Utility functions
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

