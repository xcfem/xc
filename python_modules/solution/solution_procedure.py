# -*- coding: utf-8 -*-
''' Convenience classes and functions to define solution procedures.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.ortega.ort@gmail.com"

import sys
import xc
from misc_utils import log_messages as lmsg

class SolutionProcedure(object):
    ''' Base class for solution procedure wrappers.

    :ivar name: identifier of the solution procedure.
    :ivar feProblem: finite element problem so solve.
    :ivar solutionStrategy: object that aggregates all the component objects
                            that define the type of analysis (constraint 
                            handler, DOF_Numberer, integrator, solution 
                            algorithm, system of equations, convergence 
                            test)
    :ivar modelWrapper: model representation for the analysis.
    '''
    _counter = 0 # Counts the objects of this type.
    
    def __init__(self, name= None):
        ''' Constructor.

        :param name: identifier for the solution procedure.
        '''
        SolutionProcedure._counter += 1
        self.id = SolutionProcedure._counter
        if(name):
            self.name= name
        else:
            self.name= str(self.id)
        self.feProblem= None
        self.modelWrapper= None
        self.solutionStrategy= None
        
    def clear(self):
        ''' Wipe out the solution procedure.'''
        if(self.feProblem):
            solu= self.get_fe_solu_proc()
            if(solu):
                solu.clear()
            self.modelWrapper= None
            self.solutionStrategy= None

    def getModelWrapperName(self):
        ''' Return the name for the model wrapper.'''
        retval= 'sm_'+self.name
        if(self.modelWrapper):
            mw= self.modelWrapper
            tmp= mw.name
            if(tmp!='nil'):
                retval= tmp
        return retval

    def setFEProblem(self, feProblem):
        ''' Defines the solution control object.

        :param feProblem: finite element problem to solve.
        '''
        self.clear()
        self.feProblem= feProblem

    def get_fe_solu_proc(self):
        ''' Return the current solution procedure in the FE problem.'''
        retval= None
        if(self.feProblem):
            retval= self.feProblem.getSoluProc
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; FE problem not set.')
            exit(1)
        return retval
        
    def get_fe_solu_control(self):
        ''' Return the current solution control in the FE problem.'''
        return self.get_fe_solu_proc().getSoluControl

    def get_fe_preprocessor(self):
        ''' Return the preprocessor for this problem.'''
        retval= None
        if(self.feProblem):
            retval= self.feProblem.getPreprocessor
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; FE problem not set.')
            exit(1)
        return retval        

    def modelWrapperSetup(self, modelWrapperName, numberingMethod , dofNumbererType= 'default_numberer'):
        ''' Defines the model wrapper to use with this solution procedure.
         
        :param modelWrapperName: name of the new model wrapper.
        :param numberingMethod: name of the algorithm to be used for numerating
                                the graph. Available algorithms are: 'rcm' for
                                Reverse Cuthill-Macgee, 'amd' for alternative
                                min-degree numberer or 'simple' for simple 
                                algorithm.
        :param dofNumbererType: name of the type of numberer. Available types
                                are 'default_numberer', 'plain_numberer' and 
                                'parallel_numberer'.
        '''
        solCtrl= self.get_fe_solu_control()
        solModels= solCtrl.getModelWrapperContainer
        retval= solModels.getModelWrapper(modelWrapperName)
        if(retval is None):
            retval= solModels.newModelWrapper(modelWrapperName)
            numberer= retval.newNumberer("default_numberer")
            numberer.useAlgorithm(numberingMethod)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; model wrapper already set. Command ignored.')
        self.modelWrapper= retval
        return retval

    def penaltyConstraintHandlerSetup(self, alphaSP= 1e15, alphaMP= 1e15):
        ''' Define a penalty constraint handler and return a reference to it.
        
        :param alphaSP: penalty factor on single points constraints 
                        (defaults to 1e15).
        :param alphaMP: penalty factor on multi-point constraints 
                        (defaults to 1e15).
        '''
        retval= None
        if(self.modelWrapper):
            retval= self.modelWrapper.newConstraintHandler("penalty_constraint_handler")
            if(not hasattr(self, "alphaSP")):
                self.setPenaltyFactors()
            retval.alphaSP= self.alphaSP
            retval.alphaMP= self.alphaMP
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; model wrapper not set.')
            exit(1)
        return retval

    def transformationConstraintHandlerSetup(self):
        ''' Define a transformation constraint handler.
        '''
        retval= None
        if(self.modelWrapper):
            retval= self.modelWrapper.newConstraintHandler("transformation_constraint_handler")
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; model wrapper not set.')
            exit(1)
        return retval

    def lagrangeConstraintHandlerSetup(self):
        ''' Define a Lagrange multipliers constraint handler.
        '''
        retval= None
        if(self.modelWrapper):
            retval= self.modelWrapper.newConstraintHandler("lagrange_constraint_handler")
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; model wrapper not set.')
            exit(1)
        return retval

    def plainConstraintHandlerSetup(self):
        ''' Define a plain constraint handler.
        '''
        retval= None
        if(self.modelWrapper):
            retval= self.modelWrapper.newConstraintHandler("plain_handler")
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; model wrapper not set.')
            exit(1)
        return retval
    
    def getSolutionStrategyContainer(self):
        ''' Return the container of the solution strategies.'''
        return self.get_fe_solu_control().getSolutionStrategyContainer

    def solutionStrategySetup(self, solutionStrategyName):
        ''' Define the solution strategy.

        :param solutionStrategyName: name of the new solution strategy.
        '''
        solutionStrategies= self.getSolutionStrategyContainer()
        modelWrapperName= self.getModelWrapperName()
        retval= solutionStrategies.newSolutionStrategy(solutionStrategyName, modelWrapperName)
        self.solutionStrategy= retval
        return retval
    
    def getSolutionStrategyName(self):
        ''' Return the name for the model wrapper.'''
        if(self.solutionStrategy):
            return self.solutionStrategy.name
        else:
            return 'se_'+self.name
    
    def convergenceTestSetup(self, convergenceTestType, convergenceTestTol, maxNumIter, printFlag= 0):
        ''' Define the convergence test.

        :param convergenceTestType: type of the convergence test. Available 
                                    types are: 'energy_incr_conv_test', 
                                    'fixed_num_iter_conv_test', 
                                    'norm_disp_incr_conv_test', 
                                    'norm_unbalance_conv_test', 
                                    'relative_energy_incr_conv_test', 
                                    'relative_norm_disp_incr_conv_test', 
                                    'relative_norm_unbalance_conv_test' and 
                                    'relative_total_norm_disp_incr_conv_test'
        :param convergenceTestTol: convergence tolerance.
        :param maxNumIter: maximum number of iterations.
        :param printFlag: if not zero print convergence results on each step.
        '''
        retval= None
        if(convergenceTestType):
            if(self.solutionStrategy):
                retval= self.solutionStrategy.newConvergenceTest(convergenceTestType)
                if(retval):
                    retval.tol= convergenceTestTol
                    retval.maxNumIter= maxNumIter
                    retval.printFlag= printFlag
                else:
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.error(className+'.'+methodName+'; something went wrong: '+str(sconvergenceTestType) + ' seems not to be valid convergence test type.')
                    exit(1)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; solution strategy not set.')
                exit(1)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; convergence test type cannot be None.')
            exit(1)
        return retval
        
    def getConvergenceTest(self):
        ''' Return the convergence test.'''
        retval= None
        if(self.solutionStrategy):
            retval= self.solutionStrategy.getConvergenceTest
        return retval

    def integrator_setup(self, integratorType):
        ''' Define an integrator that does not need additional parameters.

        :param integratorType: type of integrator.
        '''
        retval= None
        if(self.solutionStrategy):
            integratorParameters= xc.Vector()
            retval= self.solutionStrategy.newIntegrator(integratorType, integratorParameters)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; solution strategy not set.')
            exit(1)
        return retval
        
    def displacementControlIntegratorSetup(self, node, dof:int, increment:float, numIter:int= 1, dUmin:float= None, dUmax:float= None):
        ''' This method is used to construct a DisplacementControl integrator 
            object. In an analysis step with Displacement Control we seek to 
            determine the time step that will result in a displacement 
            increment for a particular degree-of-freedom at a node to be a 
            prescribed value.

        :param node: node whose response controls solution
        :param dof: degree of freedom at the node.
        :param increment: first displacement increment.
        :param numIter: the number of iterations the user would like to occur 
                        in the solution algorithm. Optional; default = 1.0.
        :param dUmin: the min step size the user will allow. optional; default
                      incr.
	:param dUmax: the max step size the user will allow. optional: default
                      incr. 
        '''
        retval= None
        if(self.solutionStrategy):
            integratorType= 'displacement_control_integrator'
            integratorParameters= xc.Vector()
            retval= self.solutionStrategy.newIntegrator(integratorType, integratorParameters)
            retval.nodeTag= node.tag
            retval.dof= dof
            retval.increment= increment
            retval.setNumIncr(numIter)
            if(dUmin):
                retval.minIncrement= dUmin
            if(dUmax):
                retval.maxIncrement= dUmax
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; solution strategy not set.')
            exit(1)
        return retval

    def newmarkIntegratorSetup(self, gamma, beta):
        ''' Define a Newmark integrator.

        :param gamma: gamma factor.
        :param beta: beta factor.
        '''
        retval= None
        if(self.solutionStrategy):
            integratorType= 'newmark_integrator'
            integratorParameters= xc.Vector([self.gamma, self.beta])
            retval= self.solutionStrategy.newIntegrator(integratorType, integratorParameters)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; solution strategy not set.')
            exit(1)
        return retval

    def getArcLengthIntegratorParameters(self, arcLength, alpha= 1.0):
        ''' Return the values of the Arc-Length integrator in a XC::Vector that
            can be used as argument for this type of integrator.

        :param arcLength: radius of desired intersection with the equilibrium path.
        :param alpha: scaling factor on the reference loads.
        '''
        return xc.Vector([arcLength, alpha])

    def arcLengthIntegratorSetup(self, integratorType, arcLength, alpha= 1.0):
        ''' Define a Newmark integrator.

        :param integratorType: type of arc-length integrator, available types
                               are: "arc-length_integrator", 
                               "arc-length1_integrator" and
                               "HS_constraint_integrator"
        '''
        retval= None
        if(self.solutionStrategy):
            integratorParameters= xc.Vector([arcLength, alpha])
            retval= self.solutionStrategy.newIntegrator(integratorType, integratorParameters)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; solution strategy not set.')
            exit(1)
        return retval
        
    def integratorSetup(self, integratorType, gamma= None, beta= None, node= None, dof:int= -1, increment:float= None, numIter:int= 1, dUmin:float= None, dUmax:float= None, arcLength= None, alpha= 1.0):
        ''' Define the type of integrator to use in the analysis.


        :param gamma: gamma factor (Newmark integrator only).
        :param beta: beta factor (Newmark integrator only).
        :param node: node whose response controls solution (for displacement
                     control only).
        :param dof: degree of freedom at the node (for displacement control 
                    only).
        :param increment: first displacement increment (for displacement 
                          control only).
        :param numIter: the number of iterations the user would like to occur 
                        in the solution algorithm. Optional; default = 1.0
                        (for displacement control only).
        :param dUmin: the min step size the user will allow. optional; default
                      increment. For displacement control only.
	:param dUmax: the max step size the user will allow. optional: default
                      increment. For displacement control only). 
        '''
        retval= None
        if(integratorType in ['load_control_integrator','eigen_integrator','standard_eigen_integrator', 'ill-conditioning_integrator', 'linear_buckling_integrator', 'TRBDF2_integrator', 'TRBDF3_integrator']):
            retval= self.integrator_setup(integratorType)
        elif(integratorType=='newmark_integrator'):
            retval= self.newmarkIntegratorSetup(gamma, beta)
        elif(integratorType=='displacement_control_integrator'):
            retval= self.displacementControlIntegratorSetup(node= node, dof= dof, increment= increment, numIter= numIter, dUmin= dUmin, dUmax= dUmax)
        elif(integratorType in ["arc-length_integrator", "arc-length1_integrator", "HS_constraint_integrator"]): # Arc-Length control.
            retval= self.arcLengthIntegratorSetup(integratorType, arcLength= arcLength, alpha= alpha)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; integrator type: '+str(integratorType)+' unknown.')
        return retval

    def getIntegrator(self):
        ''' Return a reference to the integrator.'''
        retval= None
        if(self.solutionStrategy):
            retval= self.solutionStrategy.getIntegrator
        return retval
            
    def solutionAlgorithmSetup(self, solutionAlgorithmType):
        ''' Define the solution strategy.

        :param solutionAlgorithType: type of the solution algorithm. Available
                                     types are: 'bfgs_soln_algo', 
                                     'broyden_soln_algo', 
                                     'krylov_newton_soln_algo', 
                                     'linear_soln_algo', 
                                     'modified_newton_soln_algo', 
                                     'newton_raphson_soln_algo', 
                                     'newton_line_search_soln_algo', 
                                     'periodic_newton_soln_algo', 
                                     'frequency_soln_algo', 
                                     'standard_eigen_soln_algo', 
                                     'linear_buckling_soln_algo', 
                                     'ill-conditioning_soln_algo'
        '''
        retval= None
        if(self.solutionStrategy):
            retval= self.solutionStrategy.newSolutionAlgorithm(solutionAlgorithmType)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; solution strategy not set.')
            exit(1)
        return retval

    def getSolutionAlgorithm(self):
        ''' Return the solution algorithm.'''
        retval= None
        if(self.solutionStrategy):
            retval= self.solutionStrategy.getSolutionAlgorithm
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; solution strategy not set.')
            exit(1)
        return retval
        
    def linearSolutionAlgorithm(self):
        ''' Return true if the solution algorithm is linear, false otherwise.'''
        retval= False
        solutionAlgorithm= self.getSolutionAlgoritm()
        if(solutionAlgorithm):
            solutionAlgoritmType= solutionAlgorithm.tipo()
            if(solutionAlgorithmType=='Linear'):
                retval= True
        return retval
        
    def sysOfEqnSetup(self, soeType, solverType, shift= None):
        ''' Defines the solver to use for the resulting system of
            equations.

        :param soeType: type of the system of equations object.
        :param solverType: type of the solver.
        :param shift: shift-and-invert mode (used with ARPACK).
        '''
        retvalSOE= None
        retvalSolver= None
        if(self.solutionStrategy):
            retvalSOE= self.solutionStrategy.newSystemOfEqn(soeType)
            if(retvalSOE):
                if(shift!=None):
                    retvalSOE.shift= shift
                retvalSolver= retvalSOE.newSolver(solverType)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+"; could not get a system of equations of type: '"+str(soeType) + "' probably is not a valid type or it is not implemented in your system.")
                exit(1)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; solution strategy not set.')
            exit(1)
        return retvalSOE, retvalSolver

    def analysisSetup(self, analysisType, numModes= None):
        ''' Create the analysis object.

        :param analysisType: type of the analysis to perform. Available types
                             are: 'direct_integration_analysis', 
                             'eigen_analysis', 
                             'modal_analysis', 
                             'linear_buckling_analysis', 
                             'ill-conditioning_analysis', 
                             'static_analysis', 
                             'variable_time_step_direct_integration_analysis'
        :param numModes: when appropriate, number of modes to compute.
        '''       
        soluProc= self.get_fe_solu_proc()
        solutionStrategyName= self.getSolutionStrategyName()
        retval= soluProc.newAnalysis(analysisType, self.getSolutionStrategyName(),"")
        # Set the number of modes.
        if(numModes is not None):
            retval.numModes= numModes
        return retval
    
    def getAnalysis(self):
        ''' Return the analysis object.'''
        retval= None
        if(self.feProblem):
            soluProc= self.get_fe_solu_proc()
            retval= soluProc.getAnalysis
        return retval
        
    def resetLoadCase(self):
        ''' Remove previous load from the domain.'''
        preprocessor= self.get_fe_preprocessor()
        preprocessor.resetLoadCase() # Remove previous loads.
        preprocessor.getDomain.revertToStart() # Revert to initial state.
        
    def solve(self, numSteps, calculateNodalReactions= False, includeInertia= False, reactionCheckTolerance= 1e-12):
        ''' Compute the solution (run the analysis).

        :param numSteps: number of steps to use in the analysis (useful only 
                         when loads are variable in time).
        :param calculateNodalReactions: if true calculate reactions at
                                        nodes.
        :param includeInertia: if true calculate reactions including inertia
                               effects.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        '''
        result= None
        analysis= self.getAnalysis()
        if(analysis):
            result= analysis.analyze(numSteps)
            if(result!=0):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; can\'t solve for current load case.')
                exit(-1)
            else:
                if(calculateNodalReactions):
                    nodeHandler= self.get_fe_preprocessor().getNodeHandler
                    result= nodeHandler.calculateNodalReactions(includeInertia, reactionCheckTolerance)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; analysis not defined yet.')
            exit(-1)
        return result

    def solveComb(self, combName, numSteps, calculateNodalReactions= False, includeInertia= False, reactionCheckTolerance= 1e-12):
        ''' Obtains the solution for the combination argument.

        :param combName: name of the combination to obtain the response for.

        :param numSteps: number of steps to use in the analysis (useful only 
                         when loads are variable in time).
        :param calculateNodalReactions: if true calculate reactions at
                                        nodes.
        :param includeInertia: if true calculate reactions including inertia
                               effects.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        '''
        self.resetLoadCase() # Remove previous loads.
        preprocessor= self.get_fe_preprocessor()
        preprocessor.getLoadHandler.addToDomain(combName) # Add comb. loads.
        analOk= self.solve(numSteps, calculateNodalReactions, includeInertia, reactionCheckTolerance)
        preprocessor.getLoadHandler.removeFromDomain(combName) # Remove comb.
        return analOk
