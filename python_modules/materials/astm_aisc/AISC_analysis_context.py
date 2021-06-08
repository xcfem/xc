# -*- coding: utf-8 -*-
''' AISC analysis context.'''

from __future__ import division
from __future__ import print_function

from colorama import Fore
from colorama import Style
from misc_utils import log_messages as lmsg
from solution import analysis_context
from materials.astm_aisc import AISC_limit_state_checking as aisc

class AISCAnalysisContext(analysis_context.AnalysisContextBase):
    ''' Base class for analysis contexts of AISC steel structures.

        :ivar lstSteelBeams: list of members that need to update their 
                             lateral buckling reduction factors. 
        :ivar softSteelMembers: steel members to be "softened" according to 
                                clause C2.3 of AISC 360-16.
    '''
    def __init__(self, modelSpace, calcSet, reactionNodeSet, reactionCheckTolerance, deactivationCandidates, lstSteelBeams, softSteelMembers):
        ''' Constructor.

        :param modelSpace: model space of the problem.
        :param calcSet: element set to compute internal forces on.
        :param reactionNodeSet: node set to compute reactions on.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        :param deactivationCandidates: set of elements that could be deactivated
                                      under certain circumstances (i.e. 
                                      no compression elements...).
        :param lstSteelBeams: list of members that need to update their 
                              lateral buckling reduction factors.
        :param softSteelMembers: steel members to be "softened" according to 
                                 clause C2.3 of AISC 360-16.
        '''
        super(AISCAnalysisContext,self).__init__(modelSpace, calcSet, reactionNodeSet, reactionCheckTolerance, deactivationCandidates)
        self.lstSteelBeams= lstSteelBeams
        self.softSteelMembers= softSteelMembers

    def updateULSResults(self, comb, limitState):
        ''' Export reactions, update reduction factors and store 
            internal forces.

        :param comb: combination to analyze.
        :param limitState: limit state to compute displacements for.
        '''
        super(AISCAnalysisContext,self).updateULSResults(comb, limitState)
        
        lmsg.log('  updating reduction factors for: '+comb.name)
        if(self.lstSteelBeams):
            for sb in self.lstSteelBeams:
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

    def restoreStiffness(self):
        ''' Restore the initial stiffness of the softened elements.'''
        if(self.softSteelMembers):
            aisc.restoreStiffness(self.softSteelMembers)
 
    def softenElements(self):
        ''' Reduce column stiffness according to AISC-360.'''
        if(self.softSteelMembers):
            aisc.softenElements(self.softSteelMembers)

    def resetPhase(self, comb):
        ''' Revert the model to its initial state.

        :param comb: combination to analyze.
        '''
        super(AISCAnalysisContext,self).resetPhase(comb)
        lmsg.log('  restore stiffness for: '+comb.name)
        self.restoreStiffness()
        
    def softeningPhase(self, comb):
        ''' Reduces the stiffness of the columns

        :param comb: combination to analyze.
        '''
        retval= None
        #Solution
        lmsg.log('element softening for: '+comb.name)
        self.softenElements() # Reduce column stiffness.
        retval= self.solutionStep(comb, calculateNodalReactions= True)
        return retval
        
    def eluSolutionSteps(self, comb, limitState):
        ''' Compute internal forces (solve).

        :param comb: combination to analyze.
        :param limitState: limit state to compute internal forces for.
        '''
        # Elements to deal with
        self.resetPhase(comb)
        # Pre-load
        result= self.preloadPhase(comb)
        # Load
        result= self.loadPhase(comb)
        # Deactivate elements.
        result= self.deactivationPhase(comb, calculateNodalReactions= False)
        # Softening columns.
        result= self.softeningPhase(comb)
        # Export reactions, update reduction factors and store internal forces.
        self.updateULSResults(comb, limitState)
        comb.removeFromDomain() #Remove combination from the model.
        
    def calcInternalForces(self, loadCombinations, limitState):
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
        limitState.writeInternalForces(self.internalForcesDict)
        limitState.writeReactions(self.reactionsDict)
        self.failedCombinationsMessage(loadCombinations, limitState)
