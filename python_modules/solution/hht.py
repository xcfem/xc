# -*- coding: utf-8 -*-
''' Convenience classes and functions to define solution procedures for
dynamic analysis based on Hilber-Hughes-Taylor method.'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.ortega.ort@gmail.com"

import sys
import xc
from solution import transient
from misc_utils import log_messages as lmsg

class HHT1(transient.DampingFactorsIntegrator):
    ''' Base class for HHT1 solvers.

    :ivar alpha_hht: alpha factor for HHT1 integrator.
    :ivar beta_hht: beta factor for HHT1 integrator.
    :ivar gamma_hht: gamma factor for HHT1 integrator.
    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, alpha, solutionAlgorithmType, analysisType= 'direct_integration_analysis', beta= None, gamma= None):
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
        :param alpha: alpha factor for the HHT integrator (should be between
                      0.67 and 1.0).
        :param solutionAlgorithmType: type of the solution algorithm.
        :param analysisType: type of the analysis.
        :param beta: alpha factor for the HHT integrator. If None the default
                     value is used: beta= (2-alpha)^2/4.
        :param gamma: gamma factor for the HHT integrator. If None the default
                     value is used: gamma= (3/2-alpha).
        '''
        super(HHT1,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= 'HHT1_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType)
        if((alpha>=0.67) and (alpha<=1.0)):
            self.alpha_hht= alpha
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; alpha value must be between 0.67 and 1.0.'
            errorMsg+= ' The given value was: '+str(alpha)
            lmsg.error(className+'.'+methodName+errorMsg)
            sys.exit(1)
        if(beta is None):
            self.beta_hht= (2-self.alpha_hht)*(2-self.alpha_hht)*0.25
        if(gamma is None):
            self.gamma_hht= (1.5-self.alpha_hht)

    def getAlpha(self):
        ''' Return the alpha factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getAlpha()
        return retval

    # def setAlpha(self, alpha:float):
    #     ''' Set the alpha factor of the Newmark based integrator.

    #     :param alpha: value of the alpha factor.
    #     '''
    #     retval= False
    #     integrator= self.getIntegrator()
    #     if(integrator):
    #         integrator.setAlpha(alpha)
    #         retval= True
    #     return retval

    def getBeta(self):
        ''' Return the beta factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getBeta()
        return retval

    # def setBeta(self, beta:float):
    #     ''' Set the beta factor of the Newmark based integrator.

    #     :param beta: value of the beta factor.
    #     '''
    #     retval= False
    #     integrator= self.getIntegrator()
    #     if(integrator):
    #         integrator.setBeta(beta)
    #         retval= True
    #     return retval
        
    def getGamma(self):
        ''' Return the gamma factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getGamma()
        return retval

    # def setGamma(self, gamma:float):
    #     ''' Set the gamma factor of the Newmark based integrator.

    #     :param gamma: value of the gamma factor.
    #     '''
    #     retval= False
    #     integrator= self.getIntegrator()
    #     if(integrator):
    #         integrator.setGamma(gamma)
    #         retval= True
    #     return retval

class HHTRayleighBase(transient.RayleighBase):
    ''' Base class for solvers based on Rayleigh integration.

    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, alpha, integratorType, solutionAlgorithmType= 'newton_raphson_soln_algo', analysisType= 'direct_integration_analysis', gamma= None):
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
        :param alpha: alpha factor for the HHT integrator (should be between
                      0.67 and 1.0).
        :param integratorType: type of the integrator (Newmark based).
        :param solutionAlgorithmType: type of the solution algorithm.
        :param analysisType: type of the analysis.
        :param gamma: gamma factor for the HHT integrator. If None the default
                     value is used: gamma= (3/2-alpha).
        '''
        super(HHTRayleighBase,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= integratorType, solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType)

        if((alpha>=0.67) and (alpha<=1.0)):
            self.alpha_hht= alpha
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; alpha value must be between 0.67 and 1.0.'
            errorMsg+= ' The given value was: '+str(alpha)
            lmsg.error(className+'.'+methodName+errorMsg)
            sys.exit(1)
        if(gamma is None):
            self.gamma_hht= (1.5-self.alpha_hht)
            
    def getAlpha(self):
        ''' Return the alpha factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getAlpha()
        return retval

    # def setAlpha(self, alpha:float):
    #     ''' Set the alpha factor of the Newmark based integrator.

    #     :param alpha: value of the alpha factor.
    #     '''
    #     retval= False
    #     integrator= self.getIntegrator()
    #     if(integrator):
    #         integrator.setAlpha(alpha)
    #         retval= True
    #     return retval

    def getGamma(self):
        ''' Return the gamma factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getGamma()
        return retval

        # def setGamma(self, gamma:float):
        #     ''' Set the gamma factor of the Newmark based integrator.

        #     :param gamma: value of the gamma factor.
        #     '''
        #     retval= False
        #     integrator= self.getIntegrator()
        #     if(integrator):
        #         integrator.setGamma(gamma)
        #         retval= True
        #     return retval
        
class HHTBase(HHTRayleighBase):
    ''' Base class for solvers based on Hilber-Hughes-Taylor integration method.

    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, alpha, integratorType, solutionAlgorithmType= 'newton_raphson_soln_algo', analysisType= 'direct_integration_analysis', beta= None, gamma= None):
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
        :param alpha: alpha factor for the HHT integrator (should be between
                      0.67 and 1.0).
        :param integratorType: type of the integrator (Newmark based).
        :param solutionAlgorithmType: type of the solution algorithm.
        :param analysisType: type of the analysis.
        :param beta: alpha factor for the HHT integrator. If None the default
                     value is used: beta= (2-alpha)^2/4.
        :param gamma: gamma factor for the HHT integrator. If None the default
                     value is used: gamma= (3/2-alpha).
        '''
        super(HHTBase,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, alpha= alpha, integratorType= integratorType, solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType, gamma= gamma)
        
        if(beta is None):
            self.beta_hht= (2-self.alpha_hht)*(2-self.alpha_hht)*0.25

    def getBeta(self):
        ''' Return the beta factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getBeta()
        return retval

    # def setBeta(self, beta:float):
    #     ''' Set the beta factor of the Newmark based integrator.

    #     :param beta: value of the beta factor.
    #     '''
    #     retval= False
    #     integrator= self.getIntegrator()
    #     if(integrator):
    #         integrator.setBeta(beta)
    #         retval= True
    #     return retval
        
class HHT(HHTBase):
    ''' Base class for HHT solvers.

    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, alpha, solutionAlgorithmType, analysisType= 'direct_integration_analysis', beta= None, gamma= None):
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
        :param alpha: alpha factor for the HHT integrator (should be between
                      0.67 and 1.0).
        :param solutionAlgorithmType: type of the solution algorithm.
        :param analysisType: type of the analysis.
        :param beta: alpha factor for the HHT integrator. If None the default
                     value is used: beta= (2-alpha)^2/4.
        :param gamma: gamma factor for the HHT integrator. If None the default
                     value is used: gamma= (3/2-alpha).
        '''
        super(HHT,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, alpha= alpha, integratorType= 'HHT_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType, beta= beta, gamma= gamma)
        
class HHTExplicitIntegrator(HHTRayleighBase):
    ''' Base class for solvers based on  HHT explicit integration method.

    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, alpha, solutionAlgorithmType, analysisType= 'direct_integration_analysis', gamma= None):
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
        :param alpha: alpha factor for the HHT integrator (should be between
                      0.67 and 1.0).
        :param solutionAlgorithmType: type of the solution algorithm.
        :param analysisType: type of the analysis.
        :param gamma: gamma factor for the HHT integrator. If None the default
                     value is used: gamma= (3/2-alpha).
        '''
        super(HHTExplicitIntegrator,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, alpha= alpha, integratorType= 'HHT_explicit_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType, gamma= gamma)

class HHTGeneralizedIntegrator(transient.RayleighBase):
    ''' Base class for HHTGeneralizedIntegrator solvers.

    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, solutionAlgorithmType, analysisType= 'direct_integration_analysis'):
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
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        errorMsg= '; Not implemented yet.'
        lmsg.error(className+'.'+methodName+errorMsg)
        sys.exit(1)
        # super(HHTGeneralizedIntegrator,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= 'HHT_generalized_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType)
        
    def getAlphaI(self):
        ''' Return the alphaI factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getAlphaI()
        return retval

    # def setAlphaI(self, alphaI:float):
    #     ''' Set the alphaI factor of the Newmark based integrator.

    #     :param alphaI: value of the alphaI factor.
    #     '''
    #     retval= False
    #     integrator= self.getIntegrator()
    #     if(integrator):
    #         integrator.setAlphaI(alphaI)
    #         retval= True
    #     return retval

    def getAlphaF(self):
        ''' Return the alphaF factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getAlphaF()
        return retval

    # def setAlphaF(self, alphaF:float):
    #     ''' Set the alphaF factor of the Newmark based integrator.

    #     :param alphaF: value of the alphaF factor.
    #     '''
    #     retval= False
    #     integrator= self.getIntegrator()
    #     if(integrator):
    #         integrator.setAlphaF(alphaF)
    #         retval= True
    #     return retval

    def getBeta(self):
        ''' Return the beta factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getBeta()
        return retval

    # def setBeta(self, beta:float):
    #     ''' Set the beta factor of the Newmark based integrator.

    #     :param beta: value of the beta factor.
    #     '''
    #     retval= False
    #     integrator= self.getIntegrator()
    #     if(integrator):
    #         integrator.setBeta(beta)
    #         retval= True
    #     return retval

    def getGamma(self):
        ''' Return the gamma factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getGamma()
        return retval

    # def setGamma(self, gamma:float):
    #     ''' Set the gamma factor of the Newmark based integrator.

    #     :param gamma: value of the gamma factor.
    #     '''
    #     retval= False
    #     integrator= self.getIntegrator()
    #     if(integrator):
    #         integrator.setGamma(gamma)
    #         retval= True
    #     return retval

class HHTBaseAlphaF(HHTBase):
    ''' Base class for HHTGeneralizedExplicitIntegrator and  
        HHTHybridSimulationIntegrator solvers.

    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, solutionAlgorithmType, analysisType= 'direct_integration_analysis'):
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
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        errorMsg= '; Not implemented yet.'
        lmsg.error(className+'.'+methodName+errorMsg)
        sys.exit(1)
        # super(HHTBaseAlphaF, self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= 'HHT_generalized_explicit_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType)

    def getAlphaI(self):
        ''' Return the alphaI factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getAlphaI()
        return retval

    # def setAlphaI(self, alphaI:float):
    #     ''' Set the alphaI factor of the Newmark based integrator.

    #     :param alphaI: value of the alphaI factor.
    #     '''
    #     retval= False
    #     integrator= self.getIntegrator()
    #     if(integrator):
    #         integrator.setAlphaI(alphaI)
    #         retval= True
    #     return retval

    def getAlphaF(self):
        ''' Return the alphaF factor of the Newmark based integrator.'''
        retval= None
        integrator= self.getIntegrator()
        if(integrator):
            retval= integrator.getAlphaF()
        return retval

    # def setAlphaF(self, alphaF:float):
    #     ''' Set the alphaF factor of the Newmark based integrator.

    #     :param alphaF: value of the alphaF factor.
    #     '''
    #     retval= False
    #     integrator= self.getIntegrator()
    #     if(integrator):
    #         integrator.setAlphaF(alphaF)
    #         retval= True
    #     return retval
        
class HHTGeneralizedExplicitIntegrator(HHTBaseAlphaF):
    ''' Base class for HHTGeneralizedExplicitIntegrator solvers.

    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, solutionAlgorithmType, analysisType= 'direct_integration_analysis'):
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
        super(HHTGeneralizedExplicitIntegrator,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= 'HHT_generalized_explicit_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType)

class HHTHybridSimulationIntegrator(HHTBaseAlphaF):
    ''' Wrapper for xc.HHTHybridSimulation solvers.

    '''
    def __init__(self, prb, timeStep, name, constraintHandlerType, maxNumIter, convergenceTestTol, printFlag, numSteps, numberingMethod, convTestType, soeType, solverType, solutionAlgorithmType, analysisType= 'direct_integration_analysis'):
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
        super(HHTHybridSimulationIntegrator,self).__init__(prb= prb, timeStep= timeStep, name= name, constraintHandlerType= constraintHandlerType, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType, soeType= soeType, solverType= solverType, integratorType= 'HHT_hybrid_simulation_integrator', solutionAlgorithmType= solutionAlgorithmType, analysisType= analysisType)
