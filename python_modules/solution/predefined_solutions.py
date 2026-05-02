# -*- coding: utf-8 -*-
''' Convenience classes and functions to define solution procedures.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.ortega.ort@gmail.com"

import sys
import xc
from solution import solution_procedure as sp
from misc_utils import log_messages as lmsg

class SolutionProcedure(sp.SolutionProcedure):
    '''
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
    :ivar solutionAlgorithmType: type of the solution algorithm.
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
    :ivar solver: solver for the system of equations.
    :ivar analysis:  determines what type of analysis is to be performed
    :ivar convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :ivar printFlag: if not zero print convergence results on each step.
    :ivar numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
    :ivar numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
    :ivar convTestType: convergence test type for non linear analysis (norm unbalance,...).
    :ivar integratorType: integrator type (see integratorSetup).
    :ivar arcLength: arc length.
    :ivar arcLengthAlpha: alpha value for the arc-length integrator.
    :ivar soeType: type of the system of equations object.
    :ivar solverType: type of the solver.
    :ivar maxNumIter: maximum number of iterations (defauts to 10)
    :ivar solu:
    :ivar solCtrl:
    :ivar modelWrapper: model representation for the analysis.
    :ivar shift: shift-and-invert mode (used with ARPACK).
    '''
    def __init__(self, name= None, constraintHandlerType= 'plain', maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', dofNumbererType= 'default_numberer', convTestType= None, soeType:str= None, solverType:str = None, shift:float= None, integratorType:str= 'load_control_integrator', arcLength= None, arcLengthAlpha= 1.0, solutionAlgorithmType= 'linear_soln_algo', analysisType= 'static_analysis'):
        ''' Constructor.

        :param name: identifier for the solution procedure.
        :param constraintHandlerType: type of the constraint handler (plain, penalty, transformation or langrange).
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param dofNumbererType: type of the DOF numberer.
        :param convTestType: convergence test type for non linear analysis
                             (norm unbalance,...).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param shift: shift-and-invert mode (used with ARPACK).
        :param integratorType: integrator type (see integratorSetup).
        :param arcLength: arc length.
        :param arcLengthAlpha: alpha value for the arc-length integrator.
        :param solutionAlgorithmType: type of the solution algorithm.
        :param analysisType: type of the analysis.
        '''
        super().__init__(name= name)

        self.cHandlerType= constraintHandlerType
        self.maxNumIter= maxNumIter
        self.convergenceTestTol= convergenceTestTol
        self.printFlag= printFlag
        self.numSteps= numSteps
        self.numberingMethod= numberingMethod
        self.dofNumbererType= dofNumbererType
        self.convTestType= convTestType
        self.integratorType= integratorType
        self.arcLength= arcLength
        self.arcLengthAlpha= arcLengthAlpha
        self.soeType= soeType
        self.solverType= solverType
        self.shift= shift
        self.solutionAlgorithmType= solutionAlgorithmType
        self.analysisType= analysisType
        
    def setup(self):
        ''' Defines the solution procedure in the finite element 
            problem object.
        '''
        self.clear()
        modelWrapperName= self.modelWrapperSetup()
        if(__debug__):
            if(not modelWrapperName):
                AssertionError('Can\'t set up the model wrapper.')
        self.constraintHandlerSetup()
        solutionStrategyName= self.getSolutionStrategyName()
        self.solutionStrategySetup(solutionStrategyName)
        self.convergenceTestSetup()
        self.solutionAlgorithmSetup()
        self.sysOfEqnSetup()
        self.analysisSetup()
        self.integratorSetup()

    def soluControlSetup(self):
        ''' Defines the solution control object.'''
        self.solu= self.feProblem.getSoluProc
        self.solCtrl= self.solu.getSoluControl        

    def modelWrapperSetup(self):
        ''' Defines the model wrapper. '''
        modelWrapperName= self.getModelWrapperName()
        numberingMethod= self.numberingMethod
        modelWrapper= super().modelWrapperSetup(modelWrapperName= modelWrapperName, numberingMethod= numberingMethod , dofNumbererType= self.dofNumbererType)
        return modelWrapperName

    def integratorSetup(self):
        ''' Define the type of integrator to use in the analysis. '''
        retval= None
        if(self.integratorType=='newmark_integrator'):
            retval= super().integratorSetup(integratorType= self.integratorType,gamma= self.gamma, beta= self.beta)
        elif(self.integratorType=='displacement_control_integrator'):
            retval= super().integratorSetup(integratorType= self.integratorType, node= self.dispControlNode, dof= self.dispControlDof, increment= self.dispControlIncrement)
        elif(self.integratorType in  ["arc-length_integrator", "arc-length1_integrator", "HS_constraint_integrator"]): # Arc-Length control.
            retval= super().integratorSetup(integratorType= self.integratorType, arcLength= self.ArcLength, alpha= self.arcLengthAlpha)
        else:
            retval= super().integratorSetup(integratorType= self.integratorType)
        return retval

    def convergenceTestSetup(self):
        ''' Define the convergence test.'''
        retval= None
        if(self.convTestType):
            retval= super().convergenceTestSetup(convergenceTestType= self.convTestType, convergenceTestTol= self.convergenceTestTol, maxNumIter= self.maxNumIter, printFlag= self.printFlag)
        return retval
        
    def solutionAlgorithmSetup(self):
        ''' Define the solution strategy.'''
        return super().solutionAlgorithmSetup(self.solutionAlgorithmType)

    def sysOfEqnSetup(self):
        ''' Defines the solver to use for the resulting system of
            equations.
        '''
        retvalSOE, retvalSolver= super().sysOfEqnSetup(soeType= self.soeType, solverType= self.solverType, shift= self.shift)
        return retvalSOE, retvalSolver
    
    def setPenaltyFactors(self, alphaSP= 1e15, alphaMP= 1e15):
        ''' Define the penalty factors to use with the penalty constraint handler.
        
        :param alphaSP: penalty factor on single points constraints (defaults to 1e15).
        :param alphaMP: penalty factor on multi-point constraints (defaults to 1e15).
        '''
        self.alphaSP= alphaSP
        self.alphaMP= alphaMP
        
    def constraintHandlerSetup(self):
        ''' Define the constraint handler and return a reference to it.'''
        if(self.cHandlerType=='penalty'):
            if(not hasattr(self, "alphaSP")):
                self.setPenaltyFactors()
            retval= super().penaltyConstraintHandlerSetup(alphaSP= self.alphaSP, alphaMP= self.alphaMP)
        elif(self.cHandlerType=='transformation'):
            retval= super().transformationConstraintHandlerSetup()
        elif(self.cHandlerType=='lagrange'):
            retval= super().lagrangeConstraintHandlerSetup()
        elif(self.cHandlerType=='plain'):
            retval= super().plainConstraintHandlerSetup()
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; unknown constraint handler type: '+self.cHandlerType)
        return retval

    def analysisSetup(self):
        ''' Create the analysis object. '''
        return super().analysisSetup(analysisType= self.analysisType)

    def solve(self, calculateNodalReactions= False, includeInertia= False, reactionCheckTolerance= 1e-12):
        ''' Compute the solution (run the analysis).

        :param calculateNodalReactions: if true calculate reactions at
                                        nodes.
        :param includeInertia: if true calculate reactions including inertia
                               effects.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        '''
        analysis= self.getAnalysis()
        if(not analysis):
            self.setup()
        return super().solve(numSteps= self.numSteps, calculateNodalReactions= calculateNodalReactions, includeInertia= includeInertia, reactionCheckTolerance= reactionCheckTolerance)
        
    def solveComb(self, combName, calculateNodalReactions= False, includeInertia= False, reactionCheckTolerance= 1e-12):
        ''' Obtains the solution for the combination argument.

        :param combName: name of the combination to obtain the response for.
        :param calculateNodalReactions: if true calculate reactions at
                                        nodes.
        :param includeInertia: if true calculate reactions including inertia
                               effects.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        '''
        self.resetLoadCase() # Remove previous loads.
        preprocessor= self.get_fe_preprocessor()
        preprocessor.getLoadHandler.addToDomain(combName) # Add comb. loads.
        analOk= self.solve(calculateNodalReactions= calculateNodalReactions, includeInertia= includeInertia, reactionCheckTolerance= reactionCheckTolerance)
        preprocessor.getLoadHandler.removeFromDomain(combName) # Remove comb.
        return analOk

#Typical solution procedures.

## Linear static analysis.
class PenaltyStaticLinearBase(SolutionProcedure):
    ''' Base class for linear static solution algorithm
        with a penalty constraint handler.
    '''
    def __init__(self, prb, name= None, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', soeType= 'band_spd_lin_soe', solverType= 'band_spd_lin_lapack_solver', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyStaticLinearBase,self).__init__(name, constraintHandlerType='penalty', maxNumIter= 1, convergenceTestTol= 1e-9, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, soeType= soeType, solverType= solverType, integratorType= integratorType, solutionAlgorithmType= 'linear_soln_algo')
        self.setFEProblem(prb)
        self.setPenaltyFactors()

class SimpleStaticLinear(PenaltyStaticLinearBase):
    ''' Return a linear static solution algorithm
        with a penalty constraint handler.
    '''
    def __init__(self, prb, name= None, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', soeType= 'band_spd_lin_soe', solverType= 'band_spd_lin_lapack_solver', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(SimpleStaticLinear,self).__init__(name, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, soeType= soeType, solverType= solverType, integratorType= integratorType)
        self.setFEProblem(prb)
        self.setPenaltyFactors()
        
### Convenience function.
def simple_static_linear(prb):
    ''' Return a simple static linear solution procedure.'''
    solProc= SimpleStaticLinear(prb)
    solProc.setup()
    return solProc.getAnalysis()
        
class SimpleStaticLinearUMF(PenaltyStaticLinearBase):
    ''' Return a linear static solution algorithm
        with a penalty constraint handler.
    '''
    def __init__(self, prb, name= None, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', soeType= 'umfpack_gen_lin_soe', solverType= 'umfpack_gen_lin_solver', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(SimpleStaticLinearUMF,self).__init__(name, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, soeType= soeType, solverType= solverType, integratorType= integratorType)
        self.setFEProblem(prb)
        self.setPenaltyFactors()
        
### Convenience function.
def simple_static_linear_umf(prb):
    ''' Return a simple static linear solution procedure.'''
    solProc= SimpleStaticLinearUMF(prb)
    solProc.setup()
    return solProc.getAnalysis()
        
class SimpleStaticLinearMUMPS(PenaltyStaticLinearBase):
    ''' Return a linear static solution algorithm
        with a penalty constraint handler.
    '''
    def __init__(self, prb, name= None, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', soeType= 'mumps_soe', solverType= 'mumps_solver', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(SimpleStaticLinearMUMPS,self).__init__(name, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, soeType= soeType, solverType= solverType, integratorType= integratorType)
        self.setFEProblem(prb)
        self.setPenaltyFactors()
        
### Convenience function.
def simple_static_linear_mumps(prb):
    ''' Return a simple static linear solution procedure.'''
    solProc= SimpleStaticLinearMUMPS(prb)
    solProc.setup()
    return solProc.getAnalysis()

class SimpleLagrangeStaticLinear(SolutionProcedure):
    ''' Linear static solution algorithm
        with a Lagrange constraint handler.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(SimpleLagrangeStaticLinear,self).__init__(name, 'lagrange', maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver', integratorType= integratorType, solutionAlgorithmType= 'linear_soln_algo')
        self.setFEProblem(prb)

class SimpleTransformationStaticLinear(SolutionProcedure):
    ''' Linear static solution algorithm with a 
        transformation constraint handler.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(SimpleTransformationStaticLinear,self).__init__(name= name, constraintHandlerType= 'transformation', maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver', integratorType= integratorType, solutionAlgorithmType= 'linear_soln_algo')
        self.setFEProblem(prb)
        
## Non-linear static analysis.
class PlainNewtonRaphson(SolutionProcedure):
    ''' Newton-Raphson solution algorithm with a 
        plain constraint handler.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_unbalance_conv_test', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PlainNewtonRaphson,self).__init__(name= name, constraintHandlerType= 'plain', maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver', integratorType= integratorType, solutionAlgorithmType= 'newton_raphson_soln_algo')
        self.setFEProblem(prb)
        
### Convenience function
def plain_newton_raphson(prb, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0):
    solProc= PlainNewtonRaphson(prb, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag)
    solProc.setup()
    return solProc.getAnalysis()

class PlainNewtonRaphsonBandGen(SolutionProcedure):
    ''' Newton-Raphson solution algorithm with a
        plain constraint handler and a band general
        SOE solver.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_unbalance_conv_test', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PlainNewtonRaphsonBandGen,self).__init__(name, constraintHandlerType= 'plain', maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver', integratorType= integratorType, solutionAlgorithmType= 'newton_raphson_soln_algo')
        self.setFEProblem(prb)
        
### Convenience function
def plain_newton_raphson_band_gen(prb, maxNumIter= 10):
    solProc= PlainNewtonRaphsonBandGen(prb, maxNumIter= maxNumIter)
    solProc.setup()
    return solProc.getAnalysis()

class PlainNewtonRaphsonMUMPS(SolutionProcedure):
    ''' Newton-Raphson solution algorithm with a
        plain constraint handler and a band general
        SOE solver.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_unbalance_conv_test', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PlainNewtonRaphsonMUMPS,self).__init__(name, constraintHandlerType= 'plain', maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= 'mumps_soe', solverType= 'mumps_solver', integratorType= integratorType, solutionAlgorithmType= 'newton_raphson_soln_algo')
        self.setFEProblem(prb)
        
### Convenience function
def plain_newton_raphson_mumps(prb, maxNumIter= 10):
    solProc= PlainNewtonRaphsonMUMPS(prb, maxNumIter= maxNumIter)
    solProc.setup()
    return solProc.getAnalysis()
        
class TransformationNewtonRaphsonBandGen(SolutionProcedure):
    ''' Newton-Raphson solution algorithm with a
        plain constraint handler and a band general
        SOE solver.
    '''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_unbalance_conv_test', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(TransformationNewtonRaphsonBandGen,self).__init__(name= name, constraintHandlerType= 'transformation', maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver', integratorType= integratorType, solutionAlgorithmType= 'newton_raphson_soln_algo')
        self.setFEProblem(prb)
        
    def setup(self):
        ''' Defines the solution procedure in the finite element 
            problem object.
        '''
        super().setup()
        if(self.numSteps!=1):
            integrator= self.getIntegrator()
            integrator.dLambda1= 1.0/self.numSteps
        
### Convenience function
def transformation_newton_raphson_band_gen(prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_unbalance_conv_test'):
    solProc= TransformationNewtonRaphsonBandGen(prb, name= name, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType)
    solProc.setup()
    return solProc.getAnalysis()
        
class PenaltyNewtonRaphsonBase(SolutionProcedure):
    ''' Base class for penalty Newton-Raphson solution aggregation.'''
    def __init__(self, prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyNewtonRaphsonBase,self).__init__(name, constraintHandlerType='penalty', maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= integratorType, solutionAlgorithmType= 'newton_raphson_soln_algo')
        self.setFEProblem(prb)
        self.setPenaltyFactors()

class PenaltyNewtonRaphson(PenaltyNewtonRaphsonBase):
    ''' Return a static solution procedure with a Newton Raphson algorithm
        and a penalty constraint handler.'''
    def __init__(self, prb, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_unbalance_conv_test', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyNewtonRaphson,self).__init__(prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver', integratorType= integratorType)

### Convenience function
def penalty_newton_raphson(prb, maxNumIter= 10, convergenceTestTol= 1e-4, printFlag= 0):
    ''' Return a penalty Newton-Raphson solution procedure.

    :param maxNumIter: maximum number of iterations (defauts to 10)
    :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :param printFlag: print message on each iteration
    '''
    solProc= PenaltyNewtonRaphson(prb, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag)
    solProc.setup()
    return solProc.getAnalysis()

class PenaltyNewtonRaphsonUMF(PenaltyNewtonRaphsonBase):
    ''' Static solution procedure with a Newton algorithm,
        a penalty constraint handler and a UMF
        (Unsimmetric multi-frontal method) solver.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_unbalance_conv_test', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyNewtonRaphsonUMF,self).__init__(prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType= 'umfpack_gen_lin_soe', solverType= 'umfpack_gen_lin_solver', integratorType= integratorType)

class PenaltyNewtonRaphsonMUMPS(PenaltyNewtonRaphsonBase):
    ''' Static solution procedure with a Newton algorithm,
        a penalty constraint handler and a MUMPS
        (parallel sparse direct solver) solver.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_unbalance_conv_test', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyNewtonRaphsonMUMPS,self).__init__(prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType= 'mumps_soe', solverType= 'mumps_solver', integratorType= integratorType)

class PlainStaticModifiedNewton(SolutionProcedure):
    ''' Static solution procedure with a modified Newton
        solution algorithm with a plain constraint handler.
    '''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse 
                                Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm 
                             unbalance,...).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PlainStaticModifiedNewton,self).__init__(name= name,  constraintHandlerType= 'plain', maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= integratorType, solutionAlgorithmType= 'modified_newton_soln_algo')
        self.setFEProblem(prb)
        
### Convenience function
def plain_static_modified_newton(prb, maxNumIter= 150,  convergenceTestTol= .01, convTestType= 'relative_total_norm_disp_incr_conv_test', soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver', printFlag= 0):
    ''' Return a simple static modified Newton solution procedure.

    :param maxNumIter: maximum number of iterations (defauts to 10)
    :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :param convTestType: convergence test for non linear analysis (norm 
                         unbalance,...).
    :param soeType: type of the system of equations object.
    :param solverType: type of the solver.
    :param printFlag: print message on each iteration
    '''
    solProc= PlainStaticModifiedNewton(prb, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, convTestType= convTestType, printFlag= printFlag)
    solProc.setup()
    return solProc.getAnalysis()

class PenaltyModifiedNewtonBase(SolutionProcedure):
    ''' Base class for penalty modified Newton solution aggregation.'''
    def __init__(self, prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType= 'relative_total_norm_disp_incr_conv_test', soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyModifiedNewtonBase,self).__init__(name, constraintHandlerType='penalty', maxNumIter= maxNumIter, convergenceTestTol=convergenceTestTol, printFlag=printFlag, numSteps=numSteps, numberingMethod=numberingMethod, convTestType=convTestType, soeType= soeType, solverType= solverType, integratorType= integratorType, solutionAlgorithmType= 'modified_newton_soln_algo')
        self.setFEProblem(prb)
        self.setPenaltyFactors()

class PenaltyModifiedNewton(PenaltyModifiedNewtonBase):
    ''' Static solution procedure with a modified Newton algorithm
        and a penalty constraint handler.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyModifiedNewton,self).__init__(prb= prb, name= name, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver', integratorType= integratorType)

### Convenience function
def penalty_modified_newton(prb, maxNumIter= 10, convergenceTestTol= 1e-4, printFlag= 0, convTestType= 'relative_total_norm_disp_incr_conv_test'):
    ''' Return a simple static modified Newton solution procedure.

    :param maxNumIter: maximum number of iterations (defauts to 10)
    :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :param printFlag: print message on each iteration
    '''
    solProc= PenaltyModifiedNewton(prb,maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, convTestType= convTestType)
    solProc.setup()
    return solProc.getAnalysis()
    
class PenaltyModifiedNewtonUMF(PenaltyModifiedNewtonBase):
    ''' Static solution procedure with a modified Newton algorithm,
        a penalty constraint handler and a UMF
        (Unsimmetric multi-frontal method) solver.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyModifiedNewtonUMF,self).__init__(prb= prb, name= name, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= 'umfpack_gen_lin_soe', solverType= 'umfpack_gen_lin_solver', integratorType= integratorType)
        
class PenaltyModifiedNewtonMUMPS(PenaltyModifiedNewtonBase):
    ''' Static solution procedure with a modified Newton algorithm,
        a penalty constraint handler and a MUMPS
        (parallel sparse direct solver) solver.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', integratorType:str= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyModifiedNewtonMUMPS,self).__init__(prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType= 'mumps_soe', solverType= 'mumps_solver', integratorType= integratorType)

### Convenience function
def penalty_modified_newton_raphson_mumps(prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', integratorType:str= 'load_control_integrator'):
    solProc= PenaltyModifiedNewtonMUMPS(prb= prb, name= name, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, integratorType= integratorType)
    solProc.setup()
    return solProc.getAnalysis()

class LineSearchBase(SolutionProcedure):
    ''' Base class for line search solution aggregations.


    :ivar lineSearchMethod: line search method to use (bisection_line_search, initial_interpolated_line_search, regula_falsi_line_search, secant_line_search).
    '''
    def __init__(self, prb, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, lineSearchMethod, soeType, solverType, integratorType, solutionAlgorithmType):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param lineSearchMethod: line search method to use (bisection_line_search, initial_interpolated_line_search, regula_falsi_line_search, secant_line_search).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param integratorType: integrator type (see integratorSetup).
        :param solutionAlgorithmType: type of the solution algorithm.
        '''
        super(LineSearchBase,self).__init__(name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType= soeType, solverType= solverType, integratorType= integratorType, solutionAlgorithmType= solutionAlgorithmType)
        self.setFEProblem(prb)
        self.lineSearchMethod= lineSearchMethod
        
    def setup(self):
        ''' Defines the solution procedure in the finite element 
            problem object.
        '''
        super(LineSearchBase,self).setup()
        self.solAlgo.setLineSearchMethod(self.lineSearchMethod)

class PenaltyNewtonLineSearchBase(LineSearchBase):
    ''' Base class for penalty Newton line search solution aggregation.'''
    def __init__(self, prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, lineSearchMethod, soeType, solverType, integratorType= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param lineSearchMethod: line search method to use (bisection_line_search, initial_interpolated_line_search, regula_falsi_line_search, secant_line_search).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyNewtonLineSearchBase,self).__init__(prb, name, constraintHandlerType='penalty', maxNumIter=maxNumIter, convergenceTestTol=convergenceTestTol, printFlag=printFlag, numSteps=numSteps, numberingMethod=numberingMethod, convTestType=convTestType, lineSearchMethod= lineSearchMethod, soeType=soeType, solverType= solverType, integratorType= integratorType, solutionAlgorithmType=  'newton_line_search_soln_algo')
        self.setPenaltyFactors()
        
class PenaltyNewtonLineSearch(PenaltyNewtonLineSearchBase):
    ''' Static solution procedure with a Newton line search algorithm
        and a penalty constraint handler.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', lineSearchMethod= 'regula_falsi_line_search', integratorType= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param lineSearchMethod: line search method to use (bisection_line_search, initial_interpolated_line_search, regula_falsi_line_search, secant_line_search).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyNewtonLineSearch,self).__init__(prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, lineSearchMethod, soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver', integratorType= integratorType)
        
class PenaltyNewtonLineSearchUMF(PenaltyNewtonLineSearchBase):
    ''' Static solution procedure with a Newton line search algorithm,
        a penalty constraint handler and a UMF
        (Unsimmetric multi-frontal method) solver.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', lineSearchMethod= 'regula_falsi_line_search', integratorType= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param lineSearchMethod: line search method to use (bisection_line_search, initial_interpolated_line_search, regula_falsi_line_search, secant_line_search).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyNewtonLineSearchUMF,self).__init__(prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, lineSearchMethod, soeType= 'umfpack_gen_lin_soe', solverType= 'umfpack_gen_lin_solver', integratorType= integratorType)
        
class PenaltyNewtonLineSearchMUMPS(PenaltyNewtonLineSearchBase):
    ''' Static solution procedure with a Newton line search algorithm,
        a penalty constraint handler and a MUMPS
        (parallel sparse direct solver) solver.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', lineSearchMethod= 'regula_falsi_line_search', integratorType= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param lineSearchMethod: line search method to use (bisection_line_search, initial_interpolated_line_search, regula_falsi_line_search, secant_line_search).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(PenaltyNewtonLineSearchMUMPS,self).__init__(prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, lineSearchMethod, soeType= 'mumps_soe', solverType= 'mumps_solver', integratorType= integratorType)
        
### Convenience function
def penalty_newton_line_search_mumps(prb, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, convTestType= 'relative_total_norm_disp_incr_conv_test', lineSearchMethod= 'regula_falsi_line_search', integratorType= 'load_control_integrator'):
    ''' Return a simple static modified Newton solution procedure.

    :param maxNumIter: maximum number of iterations (defauts to 10)
    :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :param printFlag: print message on each iteration
    :param integratorType: integrator type (see integratorSetup).
    '''
    solProc= PenaltyNewtonLineSearchMUMPS(prb,maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, convTestType= convTestType, lineSearchMethod= lineSearchMethod, integratorType= integratorType)
    solProc.setup()
    return solProc.getAnalysis()
 
class TransformationNewtonLineSearchBase(LineSearchBase):
    ''' Base class for transformation Newton line search solution aggregation.'''
    def __init__(self, prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, lineSearchMethod, soeType, solverType, integratorType= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param lineSearchMethod: line search method to use (bisection_line_search, initial_interpolated_line_search, regula_falsi_line_search, secant_line_search).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(TransformationNewtonLineSearchBase,self).__init__(prb, name, 'transformation', maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, lineSearchMethod, soeType, solverType, integratorType= integratorType, solutionAlgorithmType=  'newton_line_search_soln_algo')
        #self.setTransformationFactors()
        
class TransformationNewtonLineSearch(TransformationNewtonLineSearchBase):
    ''' Static solution procedure with a Newton line search algorithm
        and a transformation constraint handler.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', lineSearchMethod= 'regula_falsi_line_search', integratorType= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param lineSearchMethod: line search method to use (bisection_line_search, initial_interpolated_line_search, regula_falsi_line_search, secant_line_search).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(TransformationNewtonLineSearch,self).__init__(prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, lineSearchMethod, soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver', integratorType= integratorType)
        
class TransformationNewtonLineSearchUMF(TransformationNewtonLineSearchBase):
    ''' Static solution procedure with a Newton line search algorithm,
        a transformation constraint handler and a UMF
        (Unsimmetric multi-frontal method) solver.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', lineSearchMethod= 'regula_falsi_line_search', integratorType= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param lineSearchMethod: line search method to use (bisection_line_search, initial_interpolated_line_search, regula_falsi_line_search, secant_line_search).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(TransformationNewtonLineSearchUMF,self).__init__(prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, lineSearchMethod, soeType= 'umfpack_gen_lin_soe', solverType= 'umfpack_gen_lin_solver', integratorType= integratorType)
        
class TransformationNewtonLineSearchMUMPS(TransformationNewtonLineSearchBase):
    ''' Static solution procedure with a Newton line search algorithm,
        a transformation constraint handler and a MUMPS
        (parallel sparse direct solver) solver.'''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', lineSearchMethod= 'regula_falsi_line_search', integratorType= 'load_control_integrator'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param lineSearchMethod: line search method to use (bisection_line_search, initial_interpolated_line_search, regula_falsi_line_search, secant_line_search).
        :param integratorType: integrator type (see integratorSetup).
        '''
        super(TransformationNewtonLineSearchMUMPS,self).__init__(prb, name, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, lineSearchMethod, soeType= 'mumps_soe', solverType= 'mumps_solver', integratorType= integratorType)
        
### Convenience function
def transformation_newton_line_search_mumps(prb, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, convTestType= 'relative_total_norm_disp_incr_conv_test', integratorType= 'load_control_integrator'):
    ''' Return a simple static Newton-Raphson solution procedure.

    :param maxNumIter: maximum number of iterations (defauts to 10)
    :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :param printFlag: print message on each iteration
    :param convTestType: convergence test for non linear analysis (norm unbalance,...).
    :param integratorType: integrator type (see integratorSetup).
    '''
    solProc= TransformationNewtonLineSearchMUMPS(prb,maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, convTestType= convTestType, integratorType= integratorType)
    solProc.setup()
    return solProc.getAnalysis()

class PlainKrylovNewton(SolutionProcedure):
    ''' KrylovNewton algorithm object which uses a Krylov subspace 
        accelerator to accelerate the convergence of the modified 
        newton method.

    See appendix C. Analysis model script of the document
    "Finite Element Modeling of Gusset Plate Failure Using Opensees"
    Andrew J. Walker. Oregon State University
    '''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'simple', convTestType= 'energy_incr_conv_test', soeType= 'umfpack_gen_lin_soe', solverType= 'umfpack_gen_lin_solver', integratorType:str= 'load_control_integrator', maxDim= 6):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param maxDim: max number of iterations until the tangent is reformed and the acceleration restarts (default = 6).
        '''
        super(PlainKrylovNewton,self).__init__(name,  constraintHandlerType= 'plain', maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= integratorType, solutionAlgorithmType= 'krylov_newton_soln_algo')
        self.setFEProblem(prb)
        self.maxDim= maxDim
        
    def setup(self):
        ''' Defines the solution procedure in the finite element 
            problem object.
        '''
        super(PlainKrylovNewton,self).setup()
        self.solAlgo.maxDimension= self.maxDim

### Convenience function
def plain_krylov_newton(prb, maxNumIter= 300, convergenceTestTol= 1e-9, printFlag= 0, maxDim= 6):
    ''' Return a plain Krylov Newton solution procedure.

    :param maxNumIter: maximum number of iterations (defauts to 300)
    :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
    :param printFlag: print message on each iteration
    :param maxDim: max number of iterations until the tangent is reformed and the acceleration restarts (default = 6).
    '''
    solProc= PlainKrylovNewton(prb, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, maxDim= 6)
    solProc.setup()
    return solProc.getAnalysis()

class PenaltyKrylovNewton(SolutionProcedure):
    ''' KrylovNewton algorithm object which uses a Krylov subspace 
        accelerator to accelerate the convergence of the modified 
        newton method.

    See appendix C. Analysis model script of the document
    "Finite Element Modeling of Gusset Plate Failure Using Opensees"
    Andrew J. Walker. Oregon State University
    '''
    def __init__(self, prb, name= None, maxNumIter= 150, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test', integratorType:str= 'load_control_integrator', maxDim= 6):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param maxDim: max number of iterations until the tangent is reformed and the acceleration restarts (default = 6).
        '''
        super(PenaltyKrylovNewton,self).__init__(name, constraintHandlerType='penalty', maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= 'mumps_soe', solverType= 'mumps_solver', integratorType= integratorType, solutionAlgorithmType= 'krylov_newton_soln_algo')
        self.setFEProblem(prb)
        self.maxDim= maxDim
        self.setPenaltyFactors()
        
    def setup(self):
        ''' Defines the solution procedure in the finite element 
            problem object.
        '''
        super(PenaltyKrylovNewton,self).setup()
        self.integrator.dLambda1= 1.0/self.numSteps
        self.solAlgo.maxDimension= self.maxDim

## Dynamic analysis        
class NewmarkBase(SolutionProcedure):
    ''' Base class for Newmark solvers.

    :ivar gamma: gamma factor (used only for Newmark integrator).
    :ivar beta: beta factor (used only for Newmark integrator).
    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, gamma= 0.5, beta= 0.25, solutionAlgorithmType= 'newton_raphson_soln_algo', analysisType= 'direct_integration_analysis'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param timeStep: time step.
        :param name: identifier for the solution procedure.
        :param constraintHandlerType: type of the constraint handler (plain, penalty, transformation or langrange).
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test type for non linear analysis (norm unbalance,...).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param gamma: gamma factor (for Newmark integrator).
        :param beta: beta factor (for Newmark integrator).
        :param solutionAlgorithmType: type of the solution algorithm.
        :param analysisType: type of the analysis.
        '''
        super(NewmarkBase,self).__init__(name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= 'newmark_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType)
        self.setFEProblem(prb)
        self.timeStep= timeStep
        self.gamma= gamma
        self.beta= beta
        
    def solve(self, calculateNodalReactions= False, includeInertia= False, reactionCheckTolerance= 1e-12):
        ''' Compute the solution (run the analysis).

        :param calculateNodalReactions: if true calculate reactions at
                                        nodes.
        :param includeInertia: if true calculate reactions including inertia
                               effects.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        '''
        analysis= self.getAnalysis()
        if(not analysis):
            self.setup()
            analysis= self.getAnalysis()
        result= analysis.analyze(self.numSteps, self.timeStep)
        if(calculateNodalReactions and (result==0)):
            nodeHandler= self.get_fe_preprocessor().getNodeHandler
            result= nodeHandler.calculateNodalReactions(includeInertia, reactionCheckTolerance)
        return result
    
class PlainLinearNewmark(NewmarkBase):
    ''' Return a linear Newmark solution algorithm
        with a plain constraint handler.
    '''
    def __init__(self, prb, timeStep, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'simple', gamma= 0.5, beta= 0.25):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param timeStep: time step.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param gamma: gamma factor (for Newmark integrator).
        :param beta: beta factor (for Newmark integrator).
        '''
        super(PlainLinearNewmark,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType= 'plain', maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= None, soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver', gamma= gamma, beta= beta, solutionAlgorithmType= 'linear_soln_algo', analysisType= 'direct_integration_analysis')
        
class PenaltyNewmarkNewtonRaphson(NewmarkBase):
    ''' Newmark solution procedure with a Newton Raphson algorithm
        and a penalty constraint handler.'''
    def __init__(self, prb, timeStep, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test', soeType= 'profile_spd_lin_soe', solverType= 'profile_spd_lin_direct_solver', gamma= 0.5, beta= 0.25):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param timeStep: time step.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param gamma: gamma factor (for Newmark integrator).
        :param beta: beta factor (for Newmark integrator).
        '''
        super(PenaltyNewmarkNewtonRaphson,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType='penalty', maxNumIter=maxNumIter, convergenceTestTol=convergenceTestTol, printFlag=printFlag, numSteps=numSteps, numberingMethod=numberingMethod, convTestType=convTestType, soeType= soeType, solverType= solverType, gamma= gamma, beta= beta, analysisType= 'direct_integration_analysis', solutionAlgorithmType= 'newton_raphson_soln_algo')
        self.setPenaltyFactors(alphaSP= 1.0e18, alphaMP= 1.0e18)
        
class TransformationNewmarkNewtonRaphson(NewmarkBase):
    ''' Newmark solution procedure with a Newton Raphson algorithm
        and a transformation constraint handler.'''
    def __init__(self, prb, timeStep, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test', soeType= 'profile_spd_lin_soe', solverType= 'profile_spd_lin_direct_solver', gamma= 0.5, beta= 0.25):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param timeStep: time step.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param gamma: gamma factor (for Newmark integrator).
        :param beta: beta factor (for Newmark integrator).
        '''
        super(TransformationNewmarkNewtonRaphson,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType='transformation', maxNumIter=maxNumIter, convergenceTestTol=convergenceTestTol, printFlag=printFlag, numSteps=numSteps, numberingMethod=numberingMethod, convTestType=convTestType, soeType= soeType, solverType= solverType, gamma= gamma, beta= beta, solutionAlgorithmType= 'newton_raphson_soln_algo', analysisType= 'direct_integration_analysis')
        
class TRBDF2Base(SolutionProcedure):
    ''' Base class for TRBDF2 solvers.

    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, solutionAlgorithmType, analysisType):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param timeStep: time step.
        :param name: identifier for the solution procedure.
        :param constraintHandlerType: type of the constraint handler (plain, penalty, transformation or langrange).
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test type for non linear analysis (norm unbalance,...).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param solutionAlgorithmType: type of the solution algorithm.
        :param analysisType: type of the analysis.
        '''
        super(TRBDF2Base,self).__init__(name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= 'TRBDF2_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType)
        self.setFEProblem(prb)
        self.timeStep= timeStep
        
    def solve(self, calculateNodalReactions= False, includeInertia= False, reactionCheckTolerance= 1e-12):
        ''' Compute the solution (run the analysis).

        :param calculateNodalReactions: if true calculate reactions at
                                        nodes.
        :param includeInertia: if true calculate reactions including inertia
                               effects.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        '''
        analysis= self.getAnalysis()
        if(not analysis):
            self.setup()
            analysis= self.getAnalysis()
        result= analysis.analyze(self.numSteps, self.timeStep)
        if(calculateNodalReactions and (result==0)):
            nodeHandler= self.get_fe_preprocessor().getNodeHandler
            result= nodeHandler.calculateNodalReactions(includeInertia, reactionCheckTolerance)
        return result

class TransformationTRBDF2NewtonRaphson(TRBDF2Base):
    ''' TRBDF2 solution procedure with a Newton Raphson algorithm
        and a transformation constraint handler.'''
    def __init__(self, prb, timeStep, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_unbalance_conv_test', soeType= 'profile_spd_lin_soe', solverType= 'profile_spd_lin_direct_solver'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param timeStep: time step.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        '''
        super(TransformationTRBDF2NewtonRaphson,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType='transformation', maxNumIter=maxNumIter, convergenceTestTol=convergenceTestTol, printFlag=printFlag, numSteps=numSteps, numberingMethod=numberingMethod, convTestType=convTestType, soeType= soeType, solverType= solverType, solutionAlgorithmType= 'newton_raphson_soln_algo', analysisType= 'direct_integration_analysis')
        
class TRBDF3Base(SolutionProcedure):
    ''' Base class for TRBDF3 solvers.

    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, solutionAlgorithmType, analysisType):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param timeStep: time step.
        :param name: identifier for the solution procedure.
        :param constraintHandlerType: type of the constraint handler (plain, penalty, transformation or langrange).
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test type for non linear analysis (norm unbalance,...).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param solutionAlgorithmType: type of the solution algorithm.
        :param analysisType: type of the analysis.
        '''
        super(TRBDF3Base,self).__init__(name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= 'TRBDF3_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType)
        self.setFEProblem(prb)
        self.timeStep= timeStep
        
    def solve(self, calculateNodalReactions= False, includeInertia= False, reactionCheckTolerance= 1e-12):
        ''' Compute the solution (run the analysis).

        :param calculateNodalReactions: if true calculate reactions at
                                        nodes.
        :param includeInertia: if true calculate reactions including inertia
                               effects.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        '''
        analysis= self.getAnalysis()
        if(not analysis):
            self.setup()
            analysis= self.getAnalysis()
        result= analysis.analyze(self.numSteps, self.timeStep)
        if(calculateNodalReactions and (result==0)):
            nodeHandler= self.get_fe_preprocessor().getNodeHandler
            result= nodeHandler.calculateNodalReactions(includeInertia, reactionCheckTolerance)
        return result

class TransformationTRBDF3NewtonRaphson(TRBDF3Base):
    ''' TRBDF3 solution procedure with a Newton Raphson algorithm
        and a transformation constraint handler.'''
    def __init__(self, prb, timeStep, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_unbalance_conv_test', soeType= 'profile_spd_lin_soe', solverType= 'profile_spd_lin_direct_solver'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param timeStep: time step.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        '''
        super(TransformationTRBDF3NewtonRaphson,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType='transformation', maxNumIter=maxNumIter, convergenceTestTol=convergenceTestTol, printFlag=printFlag, numSteps=numSteps, numberingMethod=numberingMethod, convTestType=convTestType, soeType= soeType, solverType= solverType, solutionAlgorithmType= 'newton_raphson_soln_algo', analysisType= 'direct_integration_analysis')
        
## Eigen analysis
class OrdinaryEigenvalues(SolutionProcedure):
    ''' Procedure to obtain the ordinary eigenvalues of the stiffness matrix
        of the finite element model.'''
    
    def __init__(self, prb, name= None, printFlag= 0, systemPrefix= 'sym_band_lapack', numberingMethod= 'rcm', shift:float= None):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        '''        
        super(OrdinaryEigenvalues, self).__init__(name= name, constraintHandlerType= 'penalty', printFlag= printFlag, numberingMethod= numberingMethod, soeType= systemPrefix+"_soe", solverType= systemPrefix+"_solver", shift= shift, integratorType= 'standard_eigen_integrator', solutionAlgorithmType= 'standard_eigen_soln_algo', analysisType= 'ill-conditioning_analysis')
        self.setFEProblem(prb)
        self.setPenaltyFactors()

### Convenience function
def ordinary_eigenvalues(prb):
    ''' Return a solution procedure that computes the ordinary eigenvalues
        of the model stiffness matrix.'''
    solProc= OrdinaryEigenvalues(prb)
    solProc.setup()
    return solProc.getAnalysis()


class FrequencyAnalysis(SolutionProcedure):
    ''' Return a natural frequency computation procedure.'''

    def __init__(self, prb, name= None, printFlag= 0, systemPrefix= 'sym_band', numberingMethod= 'rcm', shift:float= None):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        '''        
        self.systemPrefix= systemPrefix
        soe_string= self.systemPrefix+'_eigen_soe'
        solver_string= self.systemPrefix+'_eigen_solver'
        super(FrequencyAnalysis,self).__init__(name, 'transformation', printFlag, numberingMethod= numberingMethod, soeType= soe_string, solverType= solver_string, shift= shift, integratorType= 'eigen_integrator', solutionAlgorithmType= 'frequency_soln_algo', analysisType= 'modal_analysis')
        self.setFEProblem(prb)
        
### Convenience function
def frequency_analysis(prb, systemPrefix= 'sym_band', shift:float= None):
    ''' Return a solution procedure that computes the natural
        frequencies of the model.'''
    solProc= FrequencyAnalysis(prb, systemPrefix= systemPrefix, shift= shift)
    solProc.setup()
    return solProc.getAnalysis()

class IllConditioningAnalysisBase(SolutionProcedure):
    ''' Base class for ill-conditioning
        solution procedures.
    '''
    def __init__(self, prb, name= None, printFlag= 0, systemPrefix= 'sym_band_eigen', shift:float= None, numberingMethod= 'rcm'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        :param systemPrefix: string that identifies the SOE and Solver types.
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        '''        
        super(IllConditioningAnalysisBase,self).__init__(name, constraintHandlerType='penalty', printFlag= printFlag, numberingMethod= numberingMethod, soeType= systemPrefix+"_soe", solverType= systemPrefix+"_solver", shift= shift, integratorType= 'ill-conditioning_integrator', solutionAlgorithmType= 'ill-conditioning_soln_algo', analysisType= 'ill-conditioning_analysis')
        self.setFEProblem(prb)
        self.setPenaltyFactors()
        
class ZeroEnergyModes(IllConditioningAnalysisBase):
    ''' Procedure to obtain the zero energy modes
        of the finite element model.'''
    def __init__(self, prb, name= None, printFlag= 0, systemPrefix= 'sym_band_eigen', shift:float= None):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        '''        
        # Very slow solver but apparently Arpack cannot
        # compute zero eigenvalues.
        # TODO: take a look to SpectraSOE/SpectraSolver as an alternative.
        super(ZeroEnergyModes,self).__init__(prb, name, printFlag= printFlag, systemPrefix= systemPrefix, shift= shift)
        
### Convenience function
def zero_energy_modes(prb):
    ''' Return a solution procedure that computes the zero
        energy modes of the model.'''
    solProc= ZeroEnergyModes(prb)
    solProc.setup()
    return solProc.getAnalysis()        

class IllConditioningAnalysis(IllConditioningAnalysisBase):
    ''' Procedure to obtain the modes
        associated with very small eigenvalues of the
        stiffness matrix.'''
    def __init__(self, prb, name= None, printFlag= 0, systemPrefix= 'band_arpack', shift:float= 0.0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        '''        
        # Very slow solver but apparently Arpack cannot
        # compute zero eigenvalues.
        # TODO: take a look to SpectraSOE/SpectraSolver as an alternative.
        super(IllConditioningAnalysis,self).__init__(prb, name, printFlag= printFlag, systemPrefix= systemPrefix, shift= shift)
    
### Convenience function
def ill_conditioning_analysis(prb):
    ''' Return a solution procedure that computes the modes
        that correspond to ill-conditioned degrees of freedom.'''
    solProc= IllConditioningAnalysis(prb)
    solProc.setup()
    return solProc.getAnalysis()

## Utility functions

def solveStaticLinearComb(combName, solutionProcedure):
    methodName= sys._getframe(0).f_code.co_name
    lmsg.warning(methodName+'; DEPRECATED; use solutionProcedure.solveComb.')
    solutionProcedure.solveComb(combName= combName)

def solveCombEstat2ndOrderLin(combName,solutionProcedure):
    solutionProcedure.resetLoadCase()
    preprocessor= solutionProcedure.feProblem.getPreprocessor
    preprocessor.getLoadHandler.addToDomain(combName)
    analOk= solutionProcedure.solve() # 1st order.
    if(analOk!=0):
        analOk= solutionProcedure.solve() # 2nd order.
        if(analOk!=0):
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(methodName+'; can\'t solve.')
    preprocessor.getLoadHandler.removeFromDomain(combName)
    # lmsg.info("Resuelta combinación: ",combName,"\n")

def solveStaticNoLinCase(combName, solutionProcedure):
    methodName= sys._getframe(0).f_code.co_name
    lmsg.warning(methodName+'; DEPRECATED; use solutionProcedure.solveComb.')
    solutionProcedure.solveComb(combName= combName)

class BucklingAnalysisEigenPart(SolutionProcedure):
    ''' Eigenvalue part of a linear buckling analysis.'''

    def __init__(self, prb, staticAnalysisPart, name= None, printFlag= 0, soeType:str= None, solverType:str= None, shift:float= 0.0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param staticAnalysisPart: static analysis part of the linear buckling analysis. 
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param shift: shift-and-invert mode (used with ARPACK).
        '''        
        super(BucklingAnalysisEigenPart,self).__init__(name, constraintHandlerType= None, printFlag= printFlag, numberingMethod= None, soeType= soeType, solverType= solverType, shift= shift, integratorType= 'linear_buckling_integrator', solutionAlgorithmType= 'linear_buckling_soln_algo', analysisType= 'linear_buckling_analysis')
        self.staticPart= staticAnalysisPart
        modelWrapper= self.staticPart.solutionStrategy.getModelWrapper
        if(modelWrapper is not None):
            self.modelWrapper= modelWrapper
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; model wrapper not set.')
        self.setFEProblem(prb)
                        
    def setup(self):
        ''' Defines the solution procedure in the finite element 
            problem object.
        '''
        self.soluControlSetup()
        solutionStrategyName= self.getSolutionStrategyName()
        self.solutionStrategySetup(solutionStrategyName)
        self.solutionAlgorithmSetup()
        self.sysOfEqnSetup()

class BucklingAnalysisStaticPart(SolutionProcedure):
    ''' Static analysis part of a linear buckling analysis.'''

    def __init__(self, prb, name= None, constraintHandlerType= 'transformation', maxNumIter= 100, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= None, soeType:str= None, solverType:str= None, solutionAlgorithmType= 'krylov_newton_soln_algo'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param printFlag: if not zero print convergence results on each step.
        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        '''        
        super(BucklingAnalysisStaticPart,self).__init__(name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= 'load_control_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= 'static_analysis')
        self.setFEProblem(prb)
                
class LinearBucklingAnalysis(object):
    ''' Linear buckling analysis.

    :ivar feProblem: XC finite element problem.
    :ivar numModes: number of buckling modes to compute.
    :ivar constraintHandlerType: type of the constraint handler to use.
    :ivar numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
    :ivar convTestType: convergence test type for non linear analysis (norm unbalance,...).
    :ivar convergenceTestTol: convergence tolerance (defaults to 1e-8).
    :ivar maxNumIter: maximum number of iterations (defauts to 1000).
    :ivar soeType: type of the system of equations object for the static part of the analysis.
    :ivar solverType: type of the solver for the static part of the analysis.
    :ivar solutionAlgorithmType: type of the solution algorithm for the static part of the analysis.
    :ivar eigenSOEType: type of the system of equations object for the eigenproblem part of the analysis.
    :ivar eigenSolverType: type of the solver for the eigenproblem part of the analysis.
    '''
    def __init__(self, prb, numModes, constraintHandlerType= 'transformation', numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test', convergenceTestTol= 1e-8, maxNumIter= 1000, soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver', solutionAlgorithmType= 'krylov_newton_soln_algo', eigenSOEType= 'band_arpackpp_soe', eigenSolverType= 'band_arpackpp_solver', printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param numModes: number of buckling modes to compute.
        :param constraintHandlerType: type of the constraint handler to use.
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test type for non linear analysis (norm unbalance,...).
        :param convergenceTestTol: convergence tolerance (defaults to 1e-8).
        :param maxNumIter: maximum number of iterations (defauts to 1000).
        :param soeType: type of the system of equations object for the static part of the analysis.
        :param solverType: type of the solver for the static part of the analysis.
        :param solutionAlgorithmType: type of the solution algorithm for the static part of the analysis.
        :param eigenSOEType: type of the system of equations object for the eigenproblem part of the analysis.
        :param eigenSolverType: type of the solver for the eigenproblem part of the analysis.
        :param printFlag: if not zero print convergence results on each step.
        '''
        self.feProblem= prb
        self.numModes= numModes
        self.constraintHandlerType= constraintHandlerType 
        self.numberingMethod= numberingMethod 
        self.convTestType= convTestType 
        self.convergenceTestTol= convergenceTestTol 
        self.maxNumIter= maxNumIter 
        self.soeType= soeType 
        self.solverType= solverType 
        self.solutionAlgorithmType= solutionAlgorithmType
        self.printFlag= printFlag
        self.eigenSOEType= eigenSOEType 
        self.eigenSolverType= eigenSolverType

    def staticPartSetup(self):
        ''' Create the static part of the linear buckling analysis.'''
        retval= BucklingAnalysisStaticPart(prb= self.feProblem, constraintHandlerType= self.constraintHandlerType, numberingMethod= self.numberingMethod , convTestType= self.convTestType, convergenceTestTol= self.convergenceTestTol, maxNumIter= self.maxNumIter, soeType= self.soeType, solverType= self.solverType , solutionAlgorithmType= self.solutionAlgorithmType, printFlag= self.printFlag)
        retval.setup()
        return retval

    def eigenPartSetup(self, staticPart):
        ''' Create the eigen part of the linear buckling analysis.

        :param staticPart: static part of the linear buckling analysis.
        '''
        retval= BucklingAnalysisEigenPart(prb= self.feProblem, staticAnalysisPart= staticPart, soeType= self.eigenSOEType, solverType= self.eigenSolverType, printFlag= self.printFlag)
        retval.setup()
        return retval
        
    def soluControlSetup(self):
        ''' Defines the solution control object.'''
        self.solu= self.feProblem.getSoluProc
        self.solCtrl= self.solu.getSoluControl

    def analysisSetup(self):
        ''' Define the analysis object.

        :param analysisType: type of the analysis to perform.
        '''
        retval= self.eigenPart.solu.newAnalysis('linear_buckling_analysis', self.staticPart.getSolutionStrategyName(), self.eigenPart.getSolutionStrategyName())
        retval.numModes= self.numModes
        return retval
        
    def setup(self):
        ''' Defines the solution procedure in the finite element 
            problem object.
        '''
        self.staticPart= self.staticPartSetup()
        self.eigenPart= self.eigenPartSetup(staticPart= self.staticPart)
        self.soluControlSetup()
        self.analysis= self.analysisSetup()
        self.eigenPart.integratorSetup()
        
    def solve(self):
        ''' Compute the linear buckling modes.'''
        return self.analysis.analyze(self.numModes)

class SpectraLinearBucklingAnalysis(LinearBucklingAnalysis):
    ''' Linear buckling analysis that uses Spectra library to solve the eigenproblem part.
    '''
    def __init__(self, prb, numModes, constraintHandlerType= 'transformation', numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test', convergenceTestTol= 1e-8, maxNumIter= 1000, soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver', solutionAlgorithmType= 'krylov_newton_soln_algo', eigenSOEType= 'spectra_soe', eigenSolverType= 'spectra_solver', printFlag= 0):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param numModes: number of buckling modes to compute.
        :param constraintHandlerType: type of the constraint handler to use.
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test type for non linear analysis (norm unbalance,...).
        :param convergenceTestTol: convergence tolerance (defaults to 1e-8).
        :param maxNumIter: maximum number of iterations (defauts to 1000).
        :param soeType: type of the system of equations object for the static part of the analysis.
        :param solverType: type of the solver for the static part of the analysis.
        :param solutionAlgorithmType: type of the solution algorithm for the static part of the analysis.
        :param eigenSOEType: type of the system of equations object for the eigenproblem part of the analysis.
        :param eigenSolverType: type of the solver for the eigenproblem part of the analysis.
        :param printFlag: if not zero print convergence results on each step.
        '''
        super().__init__(prb= prb, numModes= numModes, constraintHandlerType= constraintHandlerType, numberingMethod= numberingMethod, convTestType= convTestType, convergenceTestTol= convergenceTestTol, maxNumIter= 1000, soeType= soeType, solverType= solverType, solutionAlgorithmType= solutionAlgorithmType, eigenSOEType= eigenSOEType, eigenSolverType= eigenSolverType, printFlag= printFlag)
    
# Displacement control analysis.
class DisplacementControlBase(SolutionProcedure):
    ''' Base class for displacement control analysis.

    :ivar dispControlNode: node whose response controls solution.
    :ivar dispControlDof: degree of freedom that controls solution.
    :ivar dispControlIncrement: first displacement increment.
    '''
    def __init__(self, prb, node, dof, increment, numSteps, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numberingMethod, convTestType, soeType, solverType, solutionAlgorithmType):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param node: node whose response controls solution.
        :param dof: degree of freedom that controls solution.
        :param increment: first displacement increment.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        :param solutionAlgorithmType: type of the solution algorithm.
        '''
        super().__init__(name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= 'displacement_control_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= 'static_analysis')
        self.setFEProblem(prb)
        self.dispControlNode= node
        self.dispControlDof= dof
        self.dispControlIncrement= increment
        
class SimpleNewtonRaphsonDisplacementControl(DisplacementControlBase):
    ''' Newton-Raphson solution algorithm with a
        plain constraint handler and a band general
        SOE solver.
    '''
    def __init__(self, prb, node, dof, increment, numSteps, name= None, maxNumIter= 10, convergenceTestTol= 1e-12, printFlag= 0, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test'):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param node: node whose response controls solution.
        :param dof: degree of freedom that controls solution.
        :param increment: first displacement increment.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param convTestType: convergence test for non linear analysis (norm unbalance,...).
        '''
        super().__init__(prb= prb, node= node, dof= dof, increment= increment, name= name, constraintHandlerType= 'transformation', maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver', solutionAlgorithmType= 'newton_raphson_soln_algo')
        
