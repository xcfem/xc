# -*- coding: utf-8 -*-
''' AWCNDS analysis context.'''

from __future__ import division
from __future__ import print_function

from colorama import Fore
from colorama import Style
from misc_utils import log_messages as lmsg
from solution import analysis_context
from solution import predefined_solutions

class AnalysisContext(analysis_context.AnalysisContextBase):
    ''' Base class for analysis contexts of wood structures according
        to AWC NDS-2018 steel structures.

        :ivar bucklingMembers: list of members that need to update their 
                               buckling reduction factors. 
    '''
    def __init__(self, modelSpace, calcSet, reactionNodeSet, reactionCheckTolerance, deactivationCandidates, bucklingMembers, silent= False):
        ''' Constructor.

        :param modelSpace: model space of the problem.
        :param calcSet: element set to compute internal forces on.
        :param reactionNodeSet: node set to compute reactions on.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        :param deactivationCandidates: set of elements that could be deactivated
                                      under certain circumstances (i.e. 
                                      no compression elements...).
        :param bucklingMembers: list of members that need to update their 
                              buckling reduction factors.
        :param silent: if true print only error messages.
        '''
        super(AnalysisContext,self).__init__(modelSpace, calcSet, reactionNodeSet, reactionCheckTolerance, deactivationCandidates, silent= silent)
        self.bucklingMembers= bucklingMembers

    def resetPhase(self, comb):
        ''' Revert the model to its initial state and set
        the load duration factor and buckling reduction factor.

        :param comb: combination to analyze.
        '''
        # Call the parent method.
        super(AnalysisContext,self).resetPhase(comb)
        for member in self.bucklingMembers:
            member.updateLoadDurationFactor(comb)
            member.updateReductionFactors()

        
    def updateULSResults(self, comb, limitState):
        ''' Export reactions, update reduction factors and store 
            internal forces.

        :param comb: combination to analyze.
        :param limitState: limit state to compute displacements for.
        '''
        super(AnalysisContext,self).updateULSResults(comb, limitState)
        
        if(not self.silent): lmsg.log('  updating reduction factors for: '+comb.name)
        if(self.bucklingMembers):
            for sb in self.bucklingMembers:
                sb.updateReductionFactors()
        
    def slsSolutionSteps(self, comb, limitState= None):
        ''' Compute displacements (solve).

        :param comb: combination to analyze.
        :param limitState: limit state to compute displacements for.
        '''
        # Revert the model to its initial state.
        self.resetPhase(comb)
        # Pre-load
        result= self.preloadPhase(comb)
        # Load
        result= self.loadPhase(comb)
        # Deactivate elements.
        self.deactivationPhase(comb, calculateNodalReactions= True)
        # #Writing results.
        # if(limitState):
        #     nodSet= self.calcSet.nodes
        #     limitState.writeDisplacements(comb.getName,nodSet)
        comb.removeFromDomain() #Remove combination from the model.
    
    def calcDisplacements(self, loadCombinations, limitState= None):
        ''' Compute internal forces (solve).

        :param loadCombinations: load combinations to use.
        :param limitState: limit state to compute displacements forces for.
        '''
        # Non linear analysis
        limitState.createOutputFiles()
        self.internalForcesDict= dict()
        self.failedCombinations= list()
        for key in loadCombinations.getKeys():
            comb= loadCombinations[key]
            self.slsSolutionSteps(comb= comb, limitState= limitState)
        limitState.writeInternalForces(self.internalForcesDict)
        self.failedCombinationsMessage(loadCombinations, limitState)

    def eluSolutionSteps(self, comb, limitState):
        ''' Compute internal forces (solve).

        :param comb: combination to analyze.
        :param limitState: limit state to compute internal forces for.
        '''
        # Reset the model to its initial state.
        self.resetPhase(comb)
        # Pre-load
        result= self.preloadPhase(comb)
        # Load
        result= self.loadPhase(comb)
        # Deactivate elements.
        result= self.deactivationPhase(comb, calculateNodalReactions= False)
        # Export reactions, update reduction factors and store internal forces.
        self.updateULSResults(comb, limitState)
        comb.removeFromDomain() #Remove combination from the model.

    def removeInternalForces(self, internalForcesToRemove):
        ''' Remove (make them zero) the internal forces in the
            list argument.

        :param internalForcesToRemove: list of the internal forces to nullify.
        '''
        if(len(internalForcesToRemove)>0):
            # Remove axial load.
            for cKey in self.internalForcesDict:
                cDict= self.internalForcesDict[cKey]
                for eKey in cDict:
                    eDict= cDict[eKey] # element data.
                    for iKey in eDict: 
                        if(iKey=='internalForces'): # element internal forces. 
                            iDict= eDict[iKey]
                            for sKey in iDict:
                                sDict= iDict[sKey]
                                for ifCode in internalForcesToRemove:
                                    sDict[ifCode]= 0.0 # remove axial load.
          
    def calcInternalForces(self, loadCombinations, limitState, internalForcesToRemove= None):
        ''' Compute internal forces (solve).

        :param loadCombinations: load combinations to use.
        :param limitState: limit state to compute internal forces for.
        '''
        # Non linear analysis
        limitState.createOutputFiles()
        self.internalForcesDict= dict()
        self.reactionsDict= dict()
        self.failedCombinations= list()
        for key in loadCombinations.getKeys():
            comb= loadCombinations[key]
            self.eluSolutionSteps(comb, limitState)
        if(internalForcesToRemove):
            self.removeInternalForces(internalForcesToRemove)
        limitState.writeInternalForces(self.internalForcesDict)
        limitState.writeReactions(self.reactionsDict)
        self.failedCombinationsMessage(loadCombinations, limitState)

class SimpleAnalysisContext(AnalysisContext):
    ''' Simple analysis context with no elements to deactivate.

    '''
    def __init__(self, modelSpace, calcSet, bucklingMembers, reactionNodes= None, reactionCheckTolerance= None, silent= False):
        ''' Constructor.

        :param modelSpace: model space of the problem.
        :param calcSet: element set to compute internal forces on.
        :param reactionNodes: nodes attached to the foundation.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        :param bucklingMembers: list of members that need to update their 
                                buckling reduction factors.
        :param silent: if true print only error messages.
        '''
        super(SimpleAnalysisContext,self).__init__(modelSpace= modelSpace, calcSet= calcSet, reactionNodeSet= reactionNodes, reactionCheckTolerance= reactionCheckTolerance, deactivationCandidates= None, bucklingMembers= bucklingMembers, silent= silent)

    def getElementsToDeactivate(self, setName):
        ''' Return the elements that will be deactivated in the next phase of
            the analysis.

        :param setName: dummy argument used for compatibility.
        '''
        return None

    def defineSolver(self, mxNumIter, convergenceTestTol, printFlag= 0):
        ''' Define the solver to use.

        :param mxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        '''
        self.modelSpace.analysis= predefined_solutions.penalty_modified_newton(self.modelSpace.preprocessor.getProblem, mxNumIter= mxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag)
