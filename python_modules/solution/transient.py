# -*- coding: utf-8 -*-
''' Convenience classes and functions to define solution procedures for
dynamic analysis problems.'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.ortega.ort@gmail.com"

import sys
import xc
from solution.predefined_solutions import SolutionProcedure
from misc_utils import log_messages as lmsg

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
        analysis= self.setup_if_required()
        result= analysis.analyze(self.numSteps, self.timeStep)
        if(calculateNodalReactions and (result==0)):
            nodeHandler= self.get_fe_preprocessor().getNodeHandler
            result= nodeHandler.calculateNodalReactions(includeInertia, reactionCheckTolerance)
        return result
    
class PlainLinearNewmark(NewmarkBase):
    ''' Return a linear Newmark solution algorithm
        with a plain constraint handler.
    '''
    def __init__(self, prb, timeStep, constraintHandlerType= 'plain', name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'simple', gamma= 0.5, beta= 0.25):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param timeStep: time step.
        :param constraintHandlerType: type of the constraint handler (plain, penalty, transformation or langrange).
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        :param numberingMethod: numbering method (plain or reverse Cuthill-McKee or alternative minimum degree).
        :param gamma: gamma factor (for Newmark integrator).
        :param beta: beta factor (for Newmark integrator).
        '''
        super(PlainLinearNewmark,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= None, soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver', gamma= gamma, beta= beta, solutionAlgorithmType= 'linear_soln_algo', analysisType= 'direct_integration_analysis')
        
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
        
class PenaltyNewmarkNewtonRaphsonMUMPS(NewmarkBase):
    ''' Newmark solution procedure with a Newton Raphson algorithm
        and a penalty constraint handler.'''
    def __init__(self, prb, timeStep, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test', soeType= 'mumps_soe', solverType= 'mumps_solver', gamma= 0.5, beta= 0.25):
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
        super(PenaltyNewmarkNewtonRaphsonMUMPS,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType='penalty', maxNumIter=maxNumIter, convergenceTestTol=convergenceTestTol, printFlag=printFlag, numSteps=numSteps, numberingMethod=numberingMethod, convTestType=convTestType, soeType= soeType, solverType= solverType, gamma= gamma, beta= beta, analysisType= 'direct_integration_analysis', solutionAlgorithmType= 'newton_raphson_soln_algo')
        self.setPenaltyFactors(alphaSP= 1.0e18, alphaMP= 1.0e18)
        
class PenaltyNewmarkModifiedNewtonMUMPS(NewmarkBase):
    ''' Newmark solution procedure with a Modified Newton algorithm
        and a penalty constraint handler.'''
    def __init__(self, prb, timeStep, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test', soeType= 'mumps_soe', solverType= 'mumps_solver', gamma= 0.5, beta= 0.25):
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
        super(PenaltyNewmarkModifiedNewtonMUMPS,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType='penalty', maxNumIter=maxNumIter, convergenceTestTol=convergenceTestTol, printFlag=printFlag, numSteps=numSteps, numberingMethod=numberingMethod, convTestType=convTestType, soeType= soeType, solverType= solverType, gamma= gamma, beta= beta, analysisType= 'direct_integration_analysis', solutionAlgorithmType= 'modified_newton_soln_algo')
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
        
class PlainNewmarkNewtonRaphson(NewmarkBase):
    ''' Newmark solution procedure with a Newton Raphson algorithm
        and a plain constraint handler.'''
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
        super(PlainNewmarkNewtonRaphson,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType='plain', maxNumIter=maxNumIter, convergenceTestTol=convergenceTestTol, printFlag=printFlag, numSteps=numSteps, numberingMethod=numberingMethod, convTestType=convTestType, soeType= soeType, solverType= solverType, gamma= gamma, beta= beta, solutionAlgorithmType= 'newton_raphson_soln_algo', analysisType= 'direct_integration_analysis')
        
class PlainNewmarkKrylovNewton(NewmarkBase):
    ''' Newmark solution procedure with a Newton Raphson algorithm
        and a plain constraint handler.'''
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
        super(PlainNewmarkKrylovNewton,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType='plain', maxNumIter=maxNumIter, convergenceTestTol=convergenceTestTol, printFlag=printFlag, numSteps=numSteps, numberingMethod=numberingMethod, convTestType=convTestType, soeType= soeType, solverType= solverType, gamma= gamma, beta= beta, solutionAlgorithmType= 'krylov_newton_soln_algo', analysisType= 'direct_integration_analysis')
        
class PenaltyNewmarkKrylovNewtonMUMPS(NewmarkBase):
    ''' Newmark solution procedure with a Krylow Newton algorithm, a penalty
       constraint handler and a MUMPS solver.
    '''
    def __init__(self, prb, timeStep, name= None, maxNumIter= 10, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test', soeType= 'mumps_soe', solverType= 'mumps_solver', gamma= 0.5, beta= 0.25):
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
        super(PenaltyNewmarkKrylovNewtonMUMPS,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType='penalty', maxNumIter=maxNumIter, convergenceTestTol=convergenceTestTol, printFlag=printFlag, numSteps=numSteps, numberingMethod=numberingMethod, convTestType=convTestType, soeType= soeType, solverType= solverType, gamma= gamma, beta= beta, solutionAlgorithmType= 'krylov_newton_soln_algo', analysisType= 'direct_integration_analysis')
        
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
        analysis= self.setup_if_required()
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
        analysis= self.setup_if_required()
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
        
