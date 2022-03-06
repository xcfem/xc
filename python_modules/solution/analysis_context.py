# -*- coding: utf-8 -*-
''' Base class for analysis contexts: this classes are used to mix
    the activation/deactivation of elements, partial introduction of 
    load combinations and changing mechanical properties of certain 
    materials and saving reactions during the analysis..'''

from __future__ import division
from __future__ import print_function

from postprocess.reports import export_internal_forces as eif
from postprocess.reports import export_reactions as er
from colorama import Fore
from colorama import Style
from misc_utils import log_messages as lmsg
from postprocess import output_styles

class AnalysisContextBase(object):
    ''' Base class for analysis contexts: those are used to mix the 
        activation/deactivation of elements, partial introduction of 
        load combinations and changing mechanical properties of certain 
        materials and saving reactions during the analysis.

        :ivar modelSpace: model space of the problem.
        :ivar calcSet: element set to compute internal forces on.
        :ivar reactionCheckTolerance: tolerance when checking reaction values.
        :ivar deactivationCandidates: set of elements that could be deactivated
                                      under certain circumstances (i.e. 
                                      no compression elements...).
        :ivar internalForcesDict: dictionary containing the internal forces for each element and each combination.
        :ivar reactionsDict: dictionary containing the reactionsfor each restrained node and each combination.
        :ivar failedCombinations: list with the names of the combinations that have failed to solve.
        :ivar preloadPatterns: names of the load patterns that are introduced 
                               during the preload phase (normally self weight 
                               and dead loads).
        :ivar silent: if true print only error messages.
    '''
    def __init__(self, modelSpace, calcSet, reactionNodeSet, reactionCheckTolerance, deactivationCandidates, silent= False):
        ''' Constructor.

        :param modelSpace: model space of the problem.
        :param calcSet: element set to compute internal forces on.
        :param reactionNodeSet: node set to compute reactions on.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        :param deactivationCandidates: set of elements that could be deactivated
                                      under certain circumstances (i.e. 
                                      no compression elements...).
        :param silent: if true print only error messages.
        '''
        self.modelSpace= modelSpace
        self.calcSet= calcSet
        self.reactionNodeSet= reactionNodeSet
        self.reactionCheckTolerance= reactionCheckTolerance
        self.deactivationCandidates= deactivationCandidates
        self.preloadPatterns= None
        self.silent= silent

    def solutionStep(self, currentCombination, calculateNodalReactions= False, combinationActive= False):
        ''' Perform a solution step.

        :param currentCombination: combination being analyzed.
        :param calculateNodalReactions: if true calculate the reactions at constrained nodes.
        :paran combinationActive: if true the current combination must be fully active.
        '''
        retval= self.modelSpace.analyze(numSteps= 1, calculateNodalReactions= calculateNodalReactions, reactionCheckTolerance= self.reactionCheckTolerance)
        if(retval!=0 or (combinationActive and not currentCombination.isActive())):
            lmsg.error('Failed to solve for combination: '+currentCombination.name)
            self.failedCombinations.append(currentCombination.name)
            quit()
        return retval

    def preloadPhase(self, comb):
        ''' Introduces the pre-load patterns (usually self weight and dead
            load) and computes the corresponding solution.

        :param comb: combination to analyze.
        '''
        retval= None
        if(self.preloadPatterns):
            if(not self.silent): lmsg.log('preload phase for: '+comb.name)
            comb.addToDomain(self.preloadPatterns) # Add the first part of the combination.
            retval= self.solutionStep(currentCombination= comb)
        else:
            if(not self.silent): lmsg.warning('no pre-load patterns specified.')
        return retval
    
    def loadPhase(self, comb):
        ''' Introduces the rest of the loads (in addition to the pre-load 
            patterns) and computes the corresponding solution.

        :param comb: combination to analyze.
        '''
        retval= None
        #Solution
        if(not self.silent): lmsg.log('load phase for: '+comb.name)
        comb.addToDomain() # Add the remaining of the combination.
        retval= self.solutionStep(currentCombination= comb, combinationActive= True)
        return retval

    def deactivationPhase(self, comb, calculateNodalReactions):
        ''' Deactivates the elements of the 'deactivationCandidates' set
            according to the conditions set in the deactivate elements
            routine and computes the corresponding solution.

        :param comb: combination to analyze.
        '''
        retval= 0
        if(self.deactivationCandidates):
            if(not self.silent): lmsg.log('deactivate elements for: '+comb.name)
            self.deactivateElements('deactivatedElements')
            retval= self.solutionStep(currentCombination= comb, calculateNodalReactions= calculateNodalReactions)
        return retval

    def resetPhase(self, comb):
        ''' Revert the model to its initial state.

        :param comb: combination to analyze.
        '''
        if(not self.silent): lmsg.log('revert model to initial state for: '+comb.name)
        if(self.deactivationCandidates):
            self.resetDeactivatedElements('deactivatedElements')
        self.modelSpace.preprocessor.resetLoadCase()
        self.modelSpace.preprocessor.getDomain.revertToStart()
        
    def resetDeactivatedElements(self, setName):
        ''' Put back compressed diagonals (at the end of the
            calculation loop).'''
        if self.modelSpace.preprocessor.getSets.exists(setName):
            if(not self.silent): lmsg.log('  reset deactivated elements.')
            deactivatedElements= self.modelSpace.preprocessor.getSets.getSet(setName)
            self.modelSpace.activateElements(deactivatedElements)
            self.modelSpace.preprocessor.getSets.removeSet(setName)
        
    def deactivateElements(self, setName):
        ''' Deactivate elements returned by getElementsToDeactivate method.'''
        elements= self.getElementsToDeactivate(setName)
        if(elements):
            self.modelSpace.deactivateElements(elements)
        
    def failedCombinationsMessage(self, loadCombinations, limitState):
        ''' Writes a message informing if there are combinations that cause
            solution to fail.

        :param loadCombinations: load combinations to use.
        :param limitState: limit state to compute displacements for.
        '''
        if(len(self.failedCombinations)>0):
            lmsg.error('Analysis failed in the following combinations: '+str(failedCombinations))
        else:
            if(not self.silent): lmsg.log(Fore.GREEN+'Analysis for combinations for '+str(limitState.label)+': '+str(loadCombinations.getKeys())+' finished.\n'+Style.RESET_ALL)
        
    def updateULSResults(self, comb, limitState):
        ''' Store internal forces and displacements.

        :param comb: combination to analyze.
        :param limitState: limit state to compute displacements for.
        '''
        if(not self.silent): lmsg.log('updating results for: '+comb.name)
        # Update internal forces.
        self.internalForcesDict.update(eif.getInternalForcesDict(comb.getName,self.calcSet.elements))
        if(self.reactionNodeSet):
            self.reactionsDict.update(er.getReactionsDict(comb.getName,self.reactionNodeSet.nodes))
        # Write displacements.
        limitState.writeDisplacements(comb.getName,self.calcSet.nodes)

    def exportReactions(self, fName, outputStyle= output_styles.defaultOutputStyle):
        '''Writes a comma separated values file with the nodes reactions.

        :param fName: name of the output file.
        '''
        er.exportReactions(self.reactionsDict, fName, outputStyle)
