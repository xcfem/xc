# -*- coding: utf-8 -*-
''' Utilities for limit state checking.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2016,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

import pickle
import os
import sys
import re
import json
from solution import predefined_solutions
from postprocess.reports import export_internal_forces as eif
from postprocess.reports import export_reactions as er
from postprocess.reports import export_displacements as ed
from postprocess.reports import export_modes as em
from misc_utils import log_messages as lmsg
from materials.sections import internal_forces
from materials import stresses
from collections import defaultdict
import csv
from postprocess import control_vars as cv
from postprocess.config import file_names as fn

defaultSolutionProcedureType=  predefined_solutions.SimpleStaticLinear

class VerifOutVars(object):
    '''Variables that control the output of limit state verifications.

    :ivar setCalc: set of elements to be checked (defaults to 'None' which 
           means that all the elements in the file of internal forces
           results are analyzed) 
    :ivar appendToResFile:  'Yes','Y','y',.., if results are appended to 
           existing file of results (defaults to 'N')
    :ivar listFile: 'Yes','Y','y',.., if latex listing file of results 
           is desired to be generated (defaults to 'N')
    :ivar calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
           meant to be calculated (defaults to 'N')
    :ivar controller: object that controls the limit state checking.
    :ivar outputDataBaseFileName: file name for the file of files to write the output on.
    '''
    def __init__(self, setCalc=None, appendToResFile='N', listFile='N', calcMeanCF='N', controller= None, outputDataBaseFileName= None):
        ''' Constructor.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
               existing file of results (defaults to 'N')
        :param listFile: 'Yes','Y','y',.., if latex listing file of results 
               is desired to be generated (defaults to 'N')
        :param calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
               meant to be calculated (defaults to 'N')
        :param controller: object that controls the limit state checking.
        :param outputDataBaseFileName: file name for the file of files to write the output on.
        '''
        self.setCalc= setCalc
        self.appendToResFile= appendToResFile
        self.listFile= listFile
        self.calcMeanCF= calcMeanCF
        self.controller= controller
        self.outputDataBaseFileName= outputDataBaseFileName

    def getCalcSetElements(self, preprocessor):
        ''' Return the set of elements to be analyzed.

        :param preprocessor: pre-processor for the XC finite element problem.
        '''
        retval= None
        if(self.setCalc):
            retval= self.setCalc.elements
        else:
            retval= preprocessor.getSets['total'].elements
        return retval

    def runChecking(self, intForcCombFileName, sections):
        '''Launch checking.

        :param intForcCombFileName: Name of the file containing the internal 
                                    forces on the element sections.
        :param sections: names of the sections to write the output for.
        '''
        retval= None
        if(self.setCalc):
            if(len(self.setCalc.elements)>0):
                # Initialize control variables.
                self.controller.initControlVars(self.setCalc)
                # Update efficiency values.
                self.controller.updateEfficiencyForSet(intForcCombFileName,self.setCalc)
                preprocessor= self.setCalc.getPreprocessor
                elements= self.setCalc.elements
                controlVarsDict= cv.get_control_vars_dict(elements= elements, controlVarName= self.controller.limitStateLabel, sections= sections)
                retval= cv.write_control_vars_from_elements(preprocessor= preprocessor, controlVarsDict= controlVarsDict, outputCfg= self, sections= sections)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+"; element set is empty. Nothing done.")                
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; result file hasn't been created, you must specify a valid set of elements.")
        return retval
        

class LimitStateData(object):
    ''' Data used when checking limit states.

    :ivar label: limit state check label; Something like "Fatigue" 
                 or "CrackControl".
    :ivar outputDataBaseFileName: name (whithout extension) of the 
                                       file that contains the results to
                                       display.
    :ivar designSituations: design situations that will be checked; 
                            i. e. uls_permanent, sls_quasi-permanent,
                            sls_frequent, sls_rare, uls_earthquake, etc. 
    :ivar woodArmerAlsoForAxialForces: if true, use Wood-Armer method for
                                       both axial and bending internal
                                       forces otherwise, use it only for 
                                       bending moments.
    '''
    envConfig= None # configuration of XC environment variables.
    def __init__(self, limitStateLabel, outputDataBaseFileName, designSituations, woodArmerAlsoForAxialForces= False, cfg= None):
        '''Limit state data constructor.


        :param limitStateLabel: limit state check label; Something like "Fatigue"                               or "CrackControl".
        :param outputDataBaseFileName: name (whithout extension) of the 
                                       file that contains the results to
                                       display.
        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, sls_quasi-permanent,
                                 sls_frequent, sls_rare, uls_earthquake, etc. 
        :param woodArmerAlsoForAxialForces: if true, use Wood-Armer method for
                                            both axial and bending internal
                                            forces otherwise, use it only for 
                                            bending moments.
        :param cfg: (STATIC) configuration of XC environment variables.
        '''
        self.label= limitStateLabel
        self.outputDataBaseFileName= outputDataBaseFileName
        self.designSituations= designSituations
        self.woodArmerAlsoForAxialForces= woodArmerAlsoForAxialForces
        LimitStateData.envConfig= cfg

    @staticmethod
    def setEnvConfig(cfg):
        ''' Set configuration of XC environment variables.

        :param cfg: configuration of XC environment variables.
        '''
        LimitStateData.envConfig= cfg
        
    def getInternalForcesFileName(self):
        '''Return the name of the file where internal forces are stored.'''
        return self.envConfig.projectDirTree.getInternalForcesResultsPath()+'intForce_'+ self.label +'.json'
    
    def getReactionsFileName(self):
        '''Return the name of the file where reactions are stored.'''
        return self.envConfig.projectDirTree.getReactionsResultsPath()+'reactions_'+ self.label +'.json'
        
    def getBucklingAnalysisResultsFileName(self):
        '''Return the name of the file where results of the buckling analysis
           are stored.'''
        return self.envConfig.projectDirTree.getInternalForcesResultsPath()+'buckling_analysis_results_'+ self.label +'.json'
    
    def readInternalForces(self, setCalc):
        ''' Read the internal forces for the elements in the set argument.

        :param setCalc: elements to read internal forces for.
        '''
        return read_internal_forces_file(self.getInternalForcesFileName(), setCalc)
    
    # def readReactions(self, nodeSet):
    #     ''' Read the reactions for the nodes in the set argument.

    #     :param nodeSet: nodes to read reactions for.
    #     '''
    #     return readReactionsFile(self.getReactionsFileName(), nodeSet)

    def getInternalForcesDict(self, nmbComb, elems):
        '''Creates a dictionary with the element's internal forces.

         :param nmbComb: combination name.
         :param elems: element set.
        '''
        return eif.get_internal_forces_dict(nmbComb= nmbComb, elems= elems,  woodArmerAlsoForAxialForces= self.woodArmerAlsoForAxialForces)

    def getInternalForcesTuple(self, setCalc):
        ''' Read the element tags, load combinations identifiers and internal
            forces for the elements in the given set and return them in a 
            tuple: (eTags, loadCombinations, internalForces).

        :param setCalc: elements to read internal forces for.
        '''
    
        intForcCombFileName= self.getInternalForcesFileName()
        # intForcItems: tuple containing the element tags, the identifiers
        # of the load combinations and the values of the
        # internal forces.        
        intForcItems= read_internal_forces_file(intForcCombFileName, setCalc= setCalc)
        return intForcItems 
            
    def getDisplacementsDict(self, nmbComb, nodes):
        '''Creates a dictionary with the displacements of the given nodes.

         :param nmbComb: combination name.
         :param nodes: node set.
        '''
        return ed.getDisplacementsDict(nmbComb= nmbComb, nodes= nodes)
    
    def getModesDict(self, nmbComb, xcSet, eigenvectorNormThreshold= 1e-3):
        '''Creates a dictionary with the modes of the given nodes.

         :param nmbComb: combination name.
         :param xcSet: set containing the nodes to export the modes on.
         :param eigenvectorNormThreshold: if the node eigenvector has a norm
                                          smaller than this threshold it is 
                                          considered null.
        '''
        return em.get_modes_dict(nmbComb= nmbComb, xcSet= xcSet)

    def getInternalForcesSubset(self, elementsOfInterestTags):
        ''' Return a dictionary containing the internal forces for the given
            elements.

        :param elementsOfInterestTags: identifiers of the elements of interest.
        '''
        fName= self.getInternalForcesFileName()
        with open(fName) as json_data:
            dct= json.load(json_data)
        retval= dict()
        for loadComb in dct: # iterate through load combinations.
            retval[loadComb]= dict()
            loadCombResults= dct[loadComb]
            for key in loadCombResults:
                if(int(key) in elementsOfInterestTags):
                    retval[loadComb][key]= loadCombResults[key]
        return retval
    
    def getReactionsDict(self, nmbComb, constrainedNodes):
        '''Creates a dictionary with the element's internal forces.

         :param nmbComb: combination name.
         :param constrainedNodes: constrainedNodes.
        '''
        return er.getReactionsDict(nmbComb, constrainedNodes)
    
    def getDisplacementsFileName(self):
        '''Return the file name to read: combination name, node number and 
        displacements (ux,uy,uz,rotX,rotY,rotZ).'''
        return self.envConfig.projectDirTree.getInternalForcesResultsPath()+'displ_'+ self.label +'.csv'
    
    def getOutputDataBaseFileName(self):
        '''Return the output file name without extension.'''
        return self.envConfig.projectDirTree.getFullVerifPath()+self.outputDataBaseFileName
    
    def getOutputDataFileName(self):
        '''Return the Python executable file name.'''
        return self.getOutputDataBaseFileName() + '.py'
    
    @staticmethod
    def loadPickleObject(objName):
        '''Read a Python object from a pickle file.'''
        with open(objName + '.pkl', 'r') as f:
            return pickle.load(f)

    def getCorrespondingLoadCombinations(self, combContainer):
        '''Return the load combinations needed for this limit state.

        :param combContainer: container with the definition of the different
                              combination families (ULS, fatigue, SLS,...)
                              see actions/combinations module.
        '''
        return combContainer.getCorrespondingLoadCombinations(self.designSituations)
    
    def dumpCombinations(self, combContainer, loadCombinations):
        '''Load into the solver the combinations needed for this limit state.

        :param combContainer: container with the definition of the different
                              combination families (ULS, fatigue, SLS,...)
                              see actions/combinations module.
        :param loadCombinations: load combination handler inside the XC solver.
        '''
        loadCombinations.clear()
        # Get the combinations corresponding to the design situations.
        situationsCombs= self.getCorrespondingLoadCombinations(combContainer)
        if(situationsCombs): # for each situation.
            for combinations in situationsCombs:
                if(combinations): # if not empty.
                    combinations.dumpCombinations(loadCombinations)
        else:
           className= type(self).__name__
           methodName= sys._getframe(0).f_code.co_name
           lmsg.warning(className+'.'+methodName+"; something went wrong. No combinations to analyze.")    
        return loadCombinations
                         
    def getLastCalculationTime(self):
        ''' Return the time of last modification of the internal
           forces file.'''
        fNameIntForc= self.getInternalForcesFileName()
        retval= None
        if(os.path.exists(fNameIntForc)):
            retval= os.path.getmtime(fNameIntForc)
        return retval

    def createOutputFiles(self):
        ''' Create the internal forces and displacement output files.'''
        self.envConfig.projectDirTree.createTree()
        self.fNameIntForc= self.getInternalForcesFileName()
        self.fNameReactions= self.getReactionsFileName()
        self.fNameDispl= self.getDisplacementsFileName()
        self.fNameBucklingAnalysisResults= self.getBucklingAnalysisResultsFileName()
        os.system("rm -f " + self.fNameIntForc) #Clear obsolete files.
        os.system("rm -f " + self.fNameReactions)
        os.system("rm -f " + self.fNameDispl)
        os.system("rm -f " + self.fNameBucklingAnalysisResults)
        fDisp= open(self.fNameDispl,"w")
        fDisp.write('Comb., Node, uX, uY, uZ, rotX, rotY , rotZ\n')
        fDisp.close()

    def writeDisplacementsLegacy(self, combNm, nodSet):
        '''Writes the resuls of displacements in a load combination 
           and set of nodes given as parameters 

        :param combNM: name of the load combination
        :param nodSet: set of nodes
        '''
        fDisp= open(self.fNameDispl,"a")
        for n in nodSet:
            strDisp= str(n.getDisp).rstrip().replace(' ',', ') #displacement vector [ux,uy,uz,rotx,roty,rotz]
            fDisp.write(combNm+", "+str(n.tag)+", " + strDisp+'\n')
        fDisp.close()

    def writeInternalForces(self):
        '''Write the internal forces results.
        '''
        with open(self.fNameIntForc, 'w') as outfile:
            json.dump(self.internalForcesDict, outfile)
        outfile.close()
        
    def writeReactions(self):
        '''Write the reactions.
        '''
        with open(self.fNameReactions, 'w') as outfile:
            json.dump(self.reactionsDict, outfile)
        outfile.close()
        
    def writeDisplacements(self):
        '''Write the displacements.
        '''
        with open(self.fNameReactions, 'w') as outfile:
            json.dump(self.displacementsDict, outfile)
        outfile.close()
                
    def prepareResultsDictionaries(self):
        ''' Prepare the dictionaries to store the results of the analysis.'''
        self.createOutputFiles()
        self.internalForcesDict= dict()
        self.reactionsDict= dict()
        self.displacementsDict= dict()

    def updateResults(self, combName, calcSet, constrainedNodes= None):
        ''' Append the results of the current analysis to the results 
            dictionaries.

        :param combName: load combination corresponding to the current analysis.
        :param setCalc: set of entities for which the verification is 
                          going to be performed.
        :param constrainedNodes: constrained nodes (defaults to None)
        '''
        self.internalForcesDict.update(self.getInternalForcesDict(combName,calcSet.elements))
        self.reactionsDict.update(self.getReactionsDict(combName, constrainedNodes= constrainedNodes))
        self.displacementsDict.update(self.getDisplacementsDict(combName, calcSet.nodes))
        
    def analyzeLoadCombinations(self, combContainer, setCalc, solutionProcedureType= defaultSolutionProcedureType, constrainedNodeSet= None, bucklingMembers= None):
        '''Analize the given load combinations and write internal forces, 
           displacements, etc. in temporary files for later use.

        :param combContainer: load combination container.
        :param setCalc: set of entities for which the verification is 
                          going to be performed
        :param solutionProcedureType: type of the solution strategy to solve
                                      the finite element problem.
        :param constrainedNodeSet: constrained nodes (defaults to None)
        :param bucklingMembers: list of members whose buckling reduction
                                factors need to be updated after each
                                commit (defaults to None)
        '''
        preprocessor= setCalc.getPreprocessor
        feProblem= preprocessor.getProblem
        solutionProcedure= solutionProcedureType(feProblem)
        preprocessor= feProblem.getPreprocessor
        loadCombinations= preprocessor.getLoadHandler.getLoadCombinations
        #Putting combinations inside XC.
        loadCombinations= self.dumpCombinations(combContainer,loadCombinations)
        
        self.prepareResultsDictionaries()
        for key in loadCombinations.getKeys():
            comb= loadCombinations[key]
            preprocessor.resetLoadCase()
            preprocessor.getDomain.revertToStart()
            comb.addToDomain() #Combination to analyze.
            #Solution
            result= solutionProcedure.solve()
            if(result!=0):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+"; can't solve.")
            if(bucklingMembers): # Update reduction factors for buckling members
                for bm in bucklingMembers:
                    bm.updateReductionFactors()
            # Store results.
            self.updateResults(combName= comb.getName, calcSet= setCalc, constrainedNodes= constrainedNodeSet)
            self.writeDisplacementsLegacy(comb.getName,setCalc.nodes)
            comb.removeFromDomain() #Remove combination from the model.
        self.writeAnalysisResults()

    def writeAnalysisResults(self):
        ''' Write the given analysis results in the corresponding results files.
        '''
        self.writeInternalForces()
        self.writeReactions()
        self.writeDisplacements()
        
    def saveAll(self, combContainer, setCalc, solutionProcedureType= defaultSolutionProcedureType, constrainedNodeSet= None, bucklingMembers= None):
        '''Write internal forces, displacements, .., for each combination


        :param combContainer: load combination container.
        :param setCalc: set of entities for which the verification is 
                          going to be performed
        :param solutionProcedureType: type of the solution strategy to solve
                                      the finite element problem.
        :param constrainedNodeSet: constrained nodes (defaults to None)
        :param bucklingMembers: list of members whose buckling reduction
                                factors need to be updated after each
                                commit (defaults to None)
        '''
        # TO DEPRECATE LP 20230220
        self.analyzeLoadCombinations(combContainer= combContainer, setCalc= setCalc, solutionProcedureType= solutionProcedureType, constrainedNodeSet= constrainedNodeSet, bucklingMembers= bucklingMembers)
        
    def runChecking(self, outputCfg, sections= ['Sect1', 'Sect2']):
        '''This method reads, for the elements in setCalc,  the internal 
        forces previously calculated and saved in the corresponding file.
        Using the 'initControlVars' and 'updateEfficiencyForSet' methods of 
        the controller, the appropriate attributes are assigned to the 
        elements and the associated limit state verification is run.
        The results are written to a file in order to be displayed or listed.

        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to the 
               result file [defatults to 'N'], generation or not
               of list file [defatults to 'N', ...)
        :param sections: names of the sections to write the output for.
        '''
        intForcCombFileName= self.getInternalForcesFileName()
        return outputCfg.runChecking(intForcCombFileName, sections)
    
    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+"; not implemented yet.")

    def getCriticalLCombs(self,threshold):
        '''Pick the load combinations for which the capacity factor exceeds the given threshold.
        
        Returns a dictionary with the following keys:
        - limitStateName: label of the limit state in question
        - threshold: capacity factor's  threshold above which the load combinations are selected
        - nElems: number of elements in the limit state verification file.
        - sect1_critical_comb: dictionary of key:value pairs, where keys are the name of the 
                               load combination and its value is the percentage of elements for
                               which the threshold is exceeded (for section 1 of the elements)
        - sect2_critical_comb: same as sect1_critical_comb in the case of element's section 2.
        - critical_combNms: list with all critical combinations
        '''
        verifFile=self.getOutputDataBaseFileName()+'.json'
        # read json file
        if os.path.isfile(verifFile):
            f=open(verifFile,'r') # Open JSON file
            inputDct= json.load(f)
            f.close()
        else:
            lmsg.error('File '+verifFile+'does not exist')
            return
        nElems=len(inputDct['elementData'])
        sect1CritCmb=dict()
        sect2CritCmb=dict()
        allCritCmb=list()
        # Section 1
        for e in inputDct['elementData'].keys():
            #section 1
            s=inputDct['elementData'][e][self.label+"Sect1"]
            posCF=re.search('CF= \d+\.*\d+',s)
            CF=eval(s[posCF.start()+4:posCF.end()])
            if CF>threshold:
                posCombNm=re.search('combName= \"\w+\"',s)
                combNm=str(s[posCombNm.start()+11:posCombNm.end()-1])
                if combNm in sect1CritCmb.keys():
                    sect1CritCmb[combNm]+=1
                else:
                    sect1CritCmb[combNm]=1
                    allCritCmb.append(combNm)
        # Section 2
        for e in inputDct['elementData'].keys():
            s=inputDct['elementData'][e][self.label+"Sect2"]
            posCF=re.search('CF= \d+\.*\d+',s)
            CF=eval(s[posCF.start()+4:posCF.end()])
            if CF>threshold:
                posCombNm=re.search('combName= \"\w+\"',s)
                combNm=str(s[posCombNm.start()+11:posCombNm.end()-1])
                if combNm in sect2CritCmb.keys():
                    sect2CritCmb[combNm]+=1
                else:
                    sect2CritCmb[combNm]=1
                    if combNm not in sect1CritCmb.keys():
                        allCritCmb.append(combNm)
        # express in percentage
        for elu in sect1CritCmb.keys(): sect1CritCmb[elu]=round(sect1CritCmb[elu]/nElems*100,2)
        for elu in sect2CritCmb.keys(): sect2CritCmb[elu]=round(sect2CritCmb[elu]/nElems*100,2)
        retval={'limitStateName':self.label,
                'threshold':threshold,
                'nElems':nElems,
                'sect1_critical_comb':sect1CritCmb,
                'sect2_critical_comb':sect2CritCmb,
                'critical_combNms':allCritCmb,
                }
        return retval
    
#20181117 end

class ULS_LimitStateData(LimitStateData):
    ''' Ultimate limit state data for permanent or transient combinations.'''
    
    def __init__(self, limitStateLabel, outputDataBaseFileName, designSituations):
        '''Constructor.

        :param limitStateLabel: limit state check label; Something like "Fatigue" 
                              or "CrackControl".
        :param outputDataBaseFileName: name (whithout extension) of the 
                                       file that contains the results to
                                       display.
        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, uls_accidental,
                                 uls_earthquake, etc. 
        '''
        super(ULS_LimitStateData,self).__init__(limitStateLabel= limitStateLabel, outputDataBaseFileName= outputDataBaseFileName, designSituations= designSituations)

    def check(self, crossSections, outputCfg= VerifOutVars(), threeDim= True):
        '''Checking of normal stresses in ultimate limit states
        (see self.dumpCombinations).

        :param crossSections: cross sections on each element.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to a file,
               generation or not of lists, ...)
        :param threeDim: true if it's 3D (Fx,Fy,Fz,Mx,My,Mz) 
               false if it's 2D (Fx,Fy,Mz).
        '''
        controlVarsDict= crossSections.check(limitStateData= self, matDiagType= "d", outputCfg= outputCfg, threeDim= threeDim)
        retval= cv.write_control_vars_from_phantom_elements(controlVarsDict= controlVarsDict, outputCfg= outputCfg)
        return retval
        
defaultLinearBucklingProcedureType= predefined_solutions.SpectraLinearBucklingAnalysis

default_uls_design_situations= ['uls_permanent', 'uls_accidental', 'uls_earthquake']

class BucklingParametersLimitStateData(ULS_LimitStateData):
    ''' Buckling parameters data for limit state checking.


    :ivar numModes: number of buckling modes to compute.
    '''
    def __init__(self, numModes= 4, limitStateLabel= 'ULS_bucklingParametersComputation', outputDataBaseFileName= fn.bucklingVerificationResultsFile, designSituations= default_uls_design_situations):
        '''Constructor

        :param numModes: number of buckling modes to compute.
        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, sls_quasi-permanent,
                                 sls_frequent, sls_rare, uls_earthquake, etc. 
        '''
        super(BucklingParametersLimitStateData, self).__init__(limitStateLabel= limitStateLabel, outputDataBaseFileName= outputDataBaseFileName, designSituations= designSituations)
        self.numModes= numModes
        
    def prepareResultsDictionaries(self):
        ''' Prepare the dictionaries to store the results of the analysis.'''
        super().prepareResultsDictionaries()
        self.modesDict= dict()

    def updateResults(self, combName, calcSet, constrainedNodes= None):
        ''' Append the results of the current analysis to the results 
            dictionaries.

        :param combName: load combination corresponding to the current analysis.
        :param setCalc: set of entities for which the verification is 
                          going to be performed.
        :param constrainedNodes: constrained nodes (defaults to None)
        '''
        super().updateResults(combName= combName, calcSet= calcSet, constrainedNodes= constrainedNodes)
        self.modesDict.update(self.getModesDict(combName, xcSet= calcSet))
        
    def analyzeLoadCombinations(self, combContainer, setCalc, solutionProcedureType= defaultLinearBucklingProcedureType, constrainedNodeSet= None, bucklingMembers= None):
        '''Analize the given load combinations and write internal forces, 
           displacements, etc. in temporary files for later use.

        :param combContainer: load combination container.
        :param setCalc: set of entities for which the verification is 
                          going to be performed
        :param solutionProcedureType: type of the solution strategy to solve
                                      the finite element problem.
        :param constrainedNodeSet: constrained nodes (defaults to None)
        :param bucklingMembers: list of members whose buckling reduction
                                factors need to be updated after each
                                commit (defaults to None)
        '''
        preprocessor= setCalc.getPreprocessor
        feProblem= preprocessor.getProblem
        solutionProcedure= solutionProcedureType(prb= feProblem, numModes= self.numModes)
        solutionProcedure.setup()
        preprocessor= feProblem.getPreprocessor
        loadCombinations= preprocessor.getLoadHandler.getLoadCombinations
        # Putting combinations inside XC.
        loadCombinations= self.dumpCombinations(combContainer, loadCombinations)

        self.prepareResultsDictionaries()
        for key in loadCombinations.getKeys():
            comb= loadCombinations[key]
            preprocessor.resetLoadCase()
            preprocessor.getDomain.revertToStart()
            comb.addToDomain() #Combination to analyze.
            #Solution
            result= solutionProcedure.solve()
            if(result!=0):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+"; can't solve.")
            # Store results.
            self.updateResults(combName= comb.getName, calcSet= setCalc, constrainedNodes= constrainedNodeSet)
            comb.removeFromDomain() # Remove combination from the model.
        
        self.writeAnalysisResults()
        
    def getResultsDict(self):
        ''' Build a dictionary containing all the analysis results.'''
        retval= dict()
        retval['internal_forces']= self.internalForcesDict
        retval['reactions']= self.reactionsDict
        retval['displacements']= self.displacementsDict
        retval['modes']= self.modesDict
        return retval
        
    def writeAnalysisResults(self):
        '''Write the analysis results.

        :param bucklingAnalysisResults: buckling analysis results.
        '''
        bucklingAnalysisResultsDict= self.getResultsDict()
        with open(self.fNameBucklingAnalysisResults, 'w') as outfile:
            json.dump(bucklingAnalysisResultsDict, outfile)
        outfile.close()
        
    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifBucklingFile())
    
    def check(self, setCalc, crossSections, controller, appendToResFile='N', listFile='N', calcMeanCF='N', threeDim= True):
        ''' Perform buckling limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param crossSections: cross sections on each element.
        :param controller: object that controls the limit state checking.
        :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
               existing file of results (defaults to 'N')
        :param listFile: 'Yes','Y','y',.., if latex listing file of results 
               is desired to be generated (defaults to 'N')
        :param calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
               meant to be calculated (defaults to 'N')
        :param threeDim: true if it's 3D (Fx,Fy,Fz,Mx,My,Mz) 
               false if it's 2D (Fx,Fy,Mz).
        '''
        dct= self.eheBucklingParametersDict['element_parameters']
        outputCfg= VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        controlVarsDict= crossSections.check(limitStateData= self, matDiagType= "d", outputCfg= outputCfg, threeDim= threeDim)
        for eTag in controlVarsDict:
            elementControlVars= controlVarsDict[eTag]
            extendedData= dct[eTag]
            Leffi= extendedData['Leffi']
            mechLambdai= extendedData['mechLambdai']
            Efi= extendedData['Efi']
            strongAxisBucklingPercent= extendedData['strongAxisBucklingPercent']
            alpha_cr_i= extendedData['alpha_cr_i']
            for index in elementControlVars:
                controlVar= elementControlVars[index]
                loadCombinationName= controlVar.combName
                mode= int(loadCombinationName.split('_')[-1])
                if(mode<=len(Leffi)):
                    mode0= mode-1
                    Leff= Leffi[mode0]
                    mechLambda= mechLambdai[mode0]
                    (EfZ, EfY)= Efi[mode0]
                    alpha_cr= alpha_cr_i[mode0]
                    dotProduct= strongAxisBucklingPercent[mode0]
                    controlVar.setBucklingParameters(Leff= Leff, mechLambda= mechLambda, efZ= EfZ, efY= EfY, mode= mode, alpha_cr= alpha_cr, strongAxisBucklingPercent= dotProduct)
                else:
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.error(className+'.'+methodName+"; can't get item: "+str(mode)+' from list: '+str(Leffi))
                    exit(1)
                    
        retval= cv.write_control_vars_from_phantom_elements(controlVarsDict= controlVarsDict, outputCfg= outputCfg)
        return retval
    
class NormalStressesRCLimitStateData(ULS_LimitStateData):
    ''' Reinforced concrete normal stresses data for limit state checking.'''
    def __init__(self, designSituations= default_uls_design_situations):
        '''Constructor 

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, sls_quasi-permanent,
                                 sls_frequent, sls_rare, uls_earthquake, etc. 
        '''
        super(NormalStressesRCLimitStateData,self).__init__(limitStateLabel= 'ULS_normalStressesResistance', outputDataBaseFileName= fn.normalStressesVerificationResultsFile, designSituations= designSituations)

    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifNormStrFile())

    def check(self, setCalc, crossSections, controller, appendToResFile='N', listFile='N', calcMeanCF='N', threeDim= True):
        ''' Perform limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param crossSections: cross sections on each element.
        :param controller: object that controls the limit state checking.
        :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
               existing file of results (defaults to 'N')
        :param listFile: 'Yes','Y','y',.., if latex listing file of results 
               is desired to be generated (defaults to 'N')
        :param calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
               meant to be calculated (defaults to 'N')
        :param threeDim: true if it's 3D (Fx,Fy,Fz,Mx,My,Mz) 
               false if it's 2D (Fx,Fy,Mz).
        '''
        outputCfg= VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        return super().check(crossSections= crossSections, outputCfg= outputCfg, threeDim= threeDim)
        
        
class NormalStressesSteelLimitStateData(ULS_LimitStateData):
    ''' Steel normal stresses data for limit state checking.'''
    def __init__(self, designSituations= default_uls_design_situations):
        '''Constructor 

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, uls_earthquake, etc. 
        '''
        super(NormalStressesSteelLimitStateData,self).__init__(limitStateLabel= 'ULS_normalStressesResistance', outputDataBaseFileName= fn.normalStressesVerificationResultsFile, designSituations= designSituations)

    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifNormStrFile())

    def check(self, setCalc, controller, appendToResFile='N', listFile='N', calcMeanCF='N'):
        ''' Perform limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param controller: object that controls the limit state checking.
        :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
               existing file of results (defaults to 'N')
        :param listFile: 'Yes','Y','y',.., if latex listing file of results 
               is desired to be generated (defaults to 'N')
        :param calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
               meant to be calculated (defaults to 'N')
        '''
        outputCfg= VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        return self.runChecking(outputCfg= outputCfg)
    
class ShearResistanceRCLimitStateData(ULS_LimitStateData):
    ''' Reinforced concrete shear resistance limit state data.'''
    def __init__(self, designSituations= default_uls_design_situations):
        '''Limit state data constructor 

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, uls_earthquake, etc. 
        '''
        super(ShearResistanceRCLimitStateData,self).__init__(limitStateLabel= 'ULS_shearResistance', outputDataBaseFileName= fn.shearVerificationResultsFile, designSituations= designSituations)
        
    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifShearFile())
        
    def check(self, setCalc, crossSections, controller, appendToResFile='N', listFile='N', calcMeanCF='N', threeDim= True):
        ''' Perform limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param crossSections: cross sections on each element.
        :param controller: object that controls the limit state checking.
        :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
               existing file of results (defaults to 'N')
        :param listFile: 'Yes','Y','y',.., if latex listing file of results 
               is desired to be generated (defaults to 'N')
        :param calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
               meant to be calculated (defaults to 'N')
        :param threeDim: true if it's 3D (Fx,Fy,Fz,Mx,My,Mz) 
               false if it's 2D (Fx,Fy,Mz).
        '''
        outputCfg= VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        return super().check(crossSections= crossSections, outputCfg= outputCfg, threeDim= threeDim)
        
class ShearResistanceSteelLimitStateData(ULS_LimitStateData):
    ''' Reinforced concrete shear resistance limit state data.'''
    def __init__(self, designSituations= default_uls_design_situations):
        '''Limit state data constructor 

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, uls_earthquake, etc. 
        '''
        super(ShearResistanceSteelLimitStateData,self).__init__(limitStateLabel= 'ULS_shearResistance', outputDataBaseFileName= fn.shearVerificationResultsFile, designSituations= designSituations)
        
    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifShearFile())
        
    def check(self, setCalc, controller, appendToResFile='N', listFile='N', calcMeanCF='N'):
        ''' Perform limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param controller: object that controls the limit state checking.
        :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
               existing file of results (defaults to 'N')
        :param listFile: 'Yes','Y','y',.., if latex listing file of results 
               is desired to be generated (defaults to 'N')
        :param calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
               meant to be calculated (defaults to 'N')
        '''
        outputCfg= VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        return self.runChecking(outputCfg= outputCfg)
    
class TorsionResistanceRCLimitStateData(ULS_LimitStateData):
    ''' Reinforced concrete torsion strength limit state data.'''

    def __init__(self, designSituations= default_uls_design_situations):
        '''Constructor.

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, uls_earthquake, etc. 
        '''
        super(TorsionResistanceRCLimitStateData,self).__init__(limitStateLabel= 'ULS_torsionResistance', outputDataBaseFileName= fn.torsionVerificationResultsFile, designSituations= designSituations)
        
    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifTorsionFile())
        
    def check(self, setCalc, crossSections, controller, appendToResFile='N', listFile='N', calcMeanCF='N', threeDim= True):
        ''' Perform limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param crossSections: cross sections on each element.
        :param controller: object that controls the limit state checking.
        :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
               existing file of results (defaults to 'N')
        :param listFile: 'Yes','Y','y',.., if latex listing file of results 
               is desired to be generated (defaults to 'N')
        :param calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
               meant to be calculated (defaults to 'N')
        :param threeDim: true if it's 3D (Fx,Fy,Fz,Mx,My,Mz) 
               false if it's 2D (Fx,Fy,Mz).
        '''
        outputCfg= VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        return super().check(crossSections= crossSections, outputCfg= outputCfg, threeDim= threeDim)

class SLS_LimitStateData(LimitStateData):
    ''' Serviceability limit state data for frequent load combinations.'''
    
    def __init__(self, limitStateLabel, outputDataBaseFileName, designSituations):
        '''Constructor.

        :param limitStateLabel: limit state check label; Something like "Fatigue"                               or "CrackControl".
        :param outputDataBaseFileName: name (whithout extension) of the 
                                       file that contains the results to
                                       display.
        :param designSituations: design situations that will be checked; 
                                 i. e. sls_quasi-permanent, sls_frequent, 
                                 sls_rare, sls_earthquake, etc. 
        '''
        super(SLS_LimitStateData,self).__init__(limitStateLabel= limitStateLabel, outputDataBaseFileName= outputDataBaseFileName, designSituations= designSituations)

    def check(self, crossSections, outputCfg= VerifOutVars(), threeDim= True):
        '''Checking of crack width under frequent loads in serviceability 
           limit states (see self.dumpCombinations).

        :param crossSections: cross sections on each element.
        :param outputCfg: instance of class VerifOutVars which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to file,
               generation or not of lists, ...)
        :param threeDim: true if it's 3D (Fx,Fy,Fz,Mx,My,Mz) 
               false if it's 2D (Fx,Fy,Mz).
        '''
        controlVarsDict= crossSections.check(limitStateData= self, matDiagType= "k", outputCfg= outputCfg, threeDim= threeDim)
        retval= cv.write_control_vars_from_phantom_elements(controlVarsDict= controlVarsDict, outputCfg= outputCfg)
        return retval

class CrackControlRCLimitStateData(SLS_LimitStateData):
    ''' Reinforced concrete crack control limit state data base class.'''
        
    def check(self, setCalc, crossSections, controller, appendToResFile='N', listFile='N', calcMeanCF='N', threeDim= True):
        ''' Perform limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param crossSections: cross sections on each element.
        :param controller: object that controls the limit state checking.
        :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
               existing file of results (defaults to 'N')
        :param listFile: 'Yes','Y','y',.., if latex listing file of results 
               is desired to be generated (defaults to 'N')
        :param calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
               meant to be calculated (defaults to 'N')
        :param threeDim: true if it's 3D (Fx,Fy,Fz,Mx,My,Mz) 
               false if it's 2D (Fx,Fy,Mz).
        '''
        outputCfg= VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        return super().check(crossSections= crossSections, outputCfg= outputCfg, threeDim= threeDim)
        
class RareLoadsCrackControlRCLimitStateData(CrackControlRCLimitStateData):
    ''' Reinforced concrete crack control under rare loads limit state data.'''
    def __init__(self, designSituations= ['sls_rare']):
        '''Limit state data constructor 

        :param designSituations: design situations that will be checked; 
                                 i. e. sls_quasi-permanent, sls_frequent, 
                                 sls_rare, sls_earthquake, etc. 
        '''
        super(RareLoadsCrackControlRCLimitStateData,self).__init__('SLS_rareLoadsCrackControl', fn.crackControlRareVerificationResultsFile, designSituations= designSituations)

class FreqLoadsCrackControlRCLimitStateData(CrackControlRCLimitStateData):
    ''' Reinforced concrete crack control under frequent loads limit state data.'''
    def __init__(self, designSituations= ['sls_frequent']):
        '''Constructor. 

        :param designSituations: design situations that will be checked; 
                                 i. e. sls_quasi-permanent, sls_frequent, 
                                 sls_rare, sls_earthquake, etc. 
        '''
        super(FreqLoadsCrackControlRCLimitStateData,self).__init__('SLS_frequentLoadsCrackControl', fn.crackControlFreqVerificationResultsFile, designSituations= designSituations)
        
    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifCrackFreqFile())
        
class QPLoadsCrackControlRCLimitStateData(CrackControlRCLimitStateData):
    ''' Reinforced concrete crack control under quasi-permanent loads limit state data.'''
    def __init__(self, designSituations= ['sls_quasi-permanent']):
        '''Constructor. 

        :param designSituations: design situations that will be checked; 
                                 i. e. sls_quasi-permanent, sls_frequent, 
                                 sls_rare, sls_earthquake, etc. 
        '''
        super(QPLoadsCrackControlRCLimitStateData,self).__init__('SLS_quasiPermanentLoadsCrackControl', fn.crackControlQpermVerificationResultsFile, designSituations= designSituations)
        
    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifCrackQpermFile())
                
class FreqLoadsDisplacementControlLimitStateData(SLS_LimitStateData):
    ''' Displacement control under frequent loads limit state data.'''
    def __init__(self, designSituations= ['sls_frequent']):
        '''Constructor. 

        :param designSituations: design situations that will be checked; 
                                 i. e. sls_quasi-permanent, sls_frequent, 
                                 sls_rare, sls_earthquake, etc. 
        '''
        super(FreqLoadsDisplacementControlLimitStateData,self).__init__('SLS_frequentLoadsDisplacementControl','', designSituations= designSituations)
        
    def check(self,reinfConcreteSections):
        '''Checking of displacements under frequent loads in
        serviceability limit states (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each element.
        '''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+"; not implemented.")

class FatigueResistanceRCLimitStateData(ULS_LimitStateData):
    ''' Reinforced concrete shear resistance limit state data.'''
    def __init__(self, designSituations= ['uls_fatigue']):
        '''Constructor. 

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, uls_accidental,
                                 uls_earthquake, etc. 
        '''
        super(FatigueResistanceRCLimitStateData,self).__init__('ULS_fatigueResistance',fn.fatigueVerificationResultsFile, designSituations= designSituations)
            
class VonMisesStressLimitStateData(ULS_LimitStateData):
    ''' Steel Von Mises stress limit state data.

    :ivar vonMisesStressId: identifier of the Von Mises stress to read
                            (see NDMaterial and MembranePlateFiberSection).
    '''
    def __init__(self, vonMisesStressId= 'max_von_mises_stress', designSituations= default_uls_design_situations):
        '''Von Mises stress limit state data constructor.

        :param vonMisesStressId: identifier of the Von Mises stress to read
                                (see NDMaterial and MembranePlateFiberSection).
        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, uls_accidental,
                                 uls_earthquake, etc. 
        '''
        super(VonMisesStressLimitStateData,self).__init__('ULS_VonMisesStressResistance', fn.vonMisesStressesVerificationResultsFile, designSituations= designSituations)
        self.vonMisesStressId= vonMisesStressId
        
    def getInternalForcesDict(self, nmbComb, elems):
        '''Creates a dictionary with the element's internal forces.

        :param nmbComb: combination name.
        :param elems: element set.
        '''
        return eif.get_internal_forces_dict(nmbComb,elems, vonMisesStressId= self.vonMisesStressId, woodArmerAlsoForAxialForces= False) # Wood-Armer has no sense here.
    
    def readInternalForces(self, setCalc):
        ''' Read the internal forces for the elements in the set argument.

        :param setCalc: elements to read internal forces for.
        '''
        return read_internal_forces_file(self.getInternalForcesFileName(), setCalc, vonMisesStressId= self.vonMisesStressId)
        
    def checkElements(self, elementsToCheck, outputCfg= VerifOutVars()):
        '''Checking of fatigue under fatigue combinations loads in
        ultimate limit states (see self.dumpCombinations).

        :param elementsToCheck: elements to check.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to a file,
               generation or not of lists, ...)
        '''
        return elementsToCheck.check(limitStateData= self, matDiagType= "d", outputCfg= outputCfg, threeDim= True)
    
    def runChecking(self,outputCfg):
        '''This method reads, for the elements in setCalc,  the internal 
        forces previously calculated and saved in the corresponding file.
        Using the 'initControlVars' and 'updateEfficiencyForSet' methods of 
        the controller, the appropriate attributes are assigned to the 
        elements and the associated limit state verification is run.
        The results are written to a file in order to be displayed or listed.

        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to the 
               result file [defatults to 'N'], generation or not
               of list file [defatults to 'N', ...)
        '''
        outputCfg.controller.vonMisesStressId= self.vonMisesStressId
        retval= super(VonMisesStressLimitStateData,self).runChecking(outputCfg, sections= [''])
        return retval
    
    def check(self, setCalc, controller, appendToResFile='N', listFile='N', calcMeanCF='N'):
        ''' Perform limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param controller: object that controls the limit state checking.
        :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
               existing file of results (defaults to 'N')
        :param listFile: 'Yes','Y','y',.., if latex listing file of results 
               is desired to be generated (defaults to 'N')
        :param calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
               meant to be calculated (defaults to 'N')
        '''
        outputCfg= VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        return self.runChecking(outputCfg= outputCfg)

freqLoadsDisplacementControl= FreqLoadsDisplacementControlLimitStateData()

rareLoadsCrackControl= RareLoadsCrackControlRCLimitStateData()
freqLoadsCrackControl= FreqLoadsCrackControlRCLimitStateData()
quasiPermanentLoadsCrackControl= QPLoadsCrackControlRCLimitStateData()

# Normal stresses.
normalStressesResistance= NormalStressesRCLimitStateData()
steelNormalStressesResistance= NormalStressesSteelLimitStateData()
woodNormalStressesResistance= NormalStressesSteelLimitStateData()

# Shear strength.
shearResistance= ShearResistanceRCLimitStateData()
steelShearResistance= ShearResistanceSteelLimitStateData()
woodShearResistance= ShearResistanceSteelLimitStateData()

# Torsion strength.
torsionResistance= TorsionResistanceRCLimitStateData()

fatigueResistance= FatigueResistanceRCLimitStateData()
# on Mises
vonMisesStressResistance= VonMisesStressLimitStateData()

class CrossSectionInternalForces(internal_forces.CrossSectionInternalForces):
    ''' Definition of the internal forces on a 3D section (6 degrees 
        of freedom) in a finite element.

    :ivar idComb: identifier of the combination to which the internal forces
                  are due.
    :ivar tagElem: identifier of the finite element.
    :ivar idSection: identifier of the section in the element.
    :ivar vonMisesStressId: identifier of the Von Mises stress to read
                            (see NDMaterial and MembranePlateFiberSection).
    '''
    def __init__(self, idComb= None, tagElem= None, idSection=None, N= 0.0,Vy= 0.0,Vz= 0.0,T= 0.0,My= 0.0,Mz= 0.0):
        '''Internal forces on a 3D section (6 degrees of freedom).

        :param idComb: identifier of the load combination.
        :param tagElem: identifier of the finite element.
        :param idSection: identifier of the section in the element.
        :param N: axial force.
        :param Vy: shear force parallel to axis y.
        :param Vz: shear force parallel to axis z.
        :param T: Torque.
        :param My: bending moment parallel to axis y.
        :param Mz: bending moment parallel to axis z.
        '''
        super().__init__(N= N,Vy= Vy,Vz= Vz,T= T,My= My,Mz= Mz)
        self.idComb= idComb
        self.tagElem= tagElem
        self.idSection= idSection

    def getDict(self):
        '''returns a dictionary whith the values of the internal forces.'''
        retval= super().getDict()
        retval['idComb']= self.idComb;
        retval['tagElem']= self.tagElem
        retval['idSection']= self.idSection
        return retval

    def setFromDict(self, dct):
        '''Sets the internal forces from the dictionary argument.'''
        super().setFromDict(dct)
        self.idComb= dct['idComb']
        self.tagElem= dct['tagElem']
        self.idSection= dct['idSection']

    def getCopy(self):
        ''' Return a copy of this object.'''
        retval= CrossSectionInternalForces()
        retval.setFromDict(self.getDict())
        return retval

def get_cross_section_internal_forces(internalForces, idComb, tagElem, key, vonMisesStressId):
    ''' Return the CrossSectionInternalForces object containing the
        internal forces in the given dictionary.

    :param internalForces: Python dictionary containing the values
                           for the internal forces.
    :param idComb: identifier of the load combination.
    :param tagElem: identifier of the finite element.
    :param key: identifier of the section in the element.
    :param vonMisesStressId: identifier of the Von Mises stress to read
                            (see NDMaterial and MembranePlateFiberSection).
    '''
    retval= CrossSectionInternalForces(idComb= idComb, tagElem= tagElem, idSection= eval(key))
    forces= internalForces[key]
    retval.setForcesFromDict(forces)
    if(vonMisesStressId in forces):
        retval.vonMisesStress= forces[vonMisesStressId]
    return retval
        
def read_int_forces_dict(intForcCombFileName, setCalc=None, vonMisesStressId= 'max_von_mises_stress'):
    '''Extracts element and combination identifiers from the internal
    forces JSON file. Return elementTags, idCombs and internal-forces values
    
    :param intForcCombFileName: name of the file containing the internal
                                forces obtained for each element for 
                                the combinations analyzed
    :param setCalc: set of elements to be analyzed (defaults to None which 
                    means that all the elements in the file of internal forces
                    results are analyzed)
    :param vonMisesStressId: identifier of the Von Mises stress to read
                            (see NDMaterial and MembranePlateFiberSection).
    '''        
    elementTags= set()
    idCombs= set()
    with open(intForcCombFileName) as json_file:
        combInternalForcesDict= json.load(json_file)
    json_file.close()
    
    internalForcesValues= defaultdict(list)
    
    if(not setCalc):
        for comb in combInternalForcesDict.keys():
            idComb= str(comb)
            idCombs.add(idComb)
            elements= combInternalForcesDict[comb]
            for elemId in elements.keys():
                tagElem= eval(elemId)
                elementData= elements[elemId]
                #elementType= elementData['type']
                internalForces= elementData['internalForces']
                for k in internalForces.keys():
                    elementTags.add(tagElem)
                    crossSectionInternalForces= get_cross_section_internal_forces(internalForces= internalForces, idComb= idComb, tagElem= tagElem, key= k, vonMisesStressId= vonMisesStressId)
                    internalForcesValues[tagElem].append(crossSectionInternalForces)
    else:
        setElTags= frozenset(setCalc.getElementTags()) # We construct a frozen set to accelerate searching.
        for idComb in combInternalForcesDict.keys():
            idCombs.add(idComb)
            elements= combInternalForcesDict[idComb]
            for elemId in elements.keys():
                tagElem= eval(elemId)
                if(tagElem in setElTags): # search element tag
                    elementData= elements[elemId]
                    #elementType= elementData['type']
                    internalForces= elementData['internalForces']
                    for k in internalForces.keys():
                        elementTags.add(tagElem)
                        crossSectionInternalForces= get_cross_section_internal_forces(internalForces= internalForces, idComb= idComb, tagElem= tagElem, key= k, vonMisesStressId= vonMisesStressId)
                        internalForcesValues[tagElem].append(crossSectionInternalForces)
    return (elementTags, idCombs, internalForcesValues)

class GaussPointStresses(stresses.Stresses3D):
    ''' Definition of the stresses in Gauss point of a 2D element

    :ivar idComb: identifier of the combination to which the stresses
                  are due.
    :ivar tagElem: identifier of the finite element.
    :ivar idGaussPoint: identifier of the section in the element.
    :ivar vonMisesStressId: identifier of the Von Mises stress to read
                            (see NDMaterial and MembranePlateFiberSection).
    '''
    def __init__(self, idComb= None, tagElem= None, idGaussPoint=None, sigma_11= 0.0, sigma_12= 0.0, sigma_13= 0.0, sigma_22= 0.0, sigma_23= 0.0, sigma_33= 0.0):
        '''Internal forces on a 3D section (6 degrees of freedom).

        :param idComb: identifier of the load combination.
        :param tagElem: identifier of the finite element.
        :param idGaussPoint: identifier of the section in the element.
        :param sigma_11:  (1,1) component of the stress tensor.
        :param sigma_12:  (1,2) component of the stress tensor.
        :param sigma_22:  (2,2) component of the stress tensor.
        '''
        super().__init__(sigma_11= sigma_11, sigma_12= sigma_12, sigma_13= sigma_13, sigma_22= sigma_22, sigma_23= sigma_23, sigma_33= sigma_33)
        self.idComb= idComb
        self.tagElem= tagElem
        self.idGaussPoint= idGaussPoint

    def getDict(self):
        '''returns a dictionary whith the values of the internal forces.'''
        retval= super().getDict()
        retval['idComb']= self.idComb;
        retval['tagElem']= self.tagElem
        retval['idGaussPoint']= self.idGaussPoint
        return retval

    def setFromDict(self, dct):
        '''Sets the internal forces from the dictionary argument.'''
        super().setFromDict(dct= dct)
        self.idComb= dct['idComb']
        self.tagElem= dct['tagElem']
        self.idGaussPoint= dct['idSection']

    def getCopy(self):
        ''' Return a copy of this object.'''
        retval= GaussPointStresses()
        retval.setFromDict(self.getDict())
        return retval
    
def get_gauss_point_stresses(stresses, idComb, tagElem, key, vonMisesStressId):
    ''' Return the CrossSectionInternalForces object containing the
        internal forces in the given dictionary.

    :param stresses: Python dictionary containing the values
                           for the stresses.
    :param idComb: identifier of the load combination.
    :param tagElem: identifier of the finite element.
    :param key: identifier of the gauss point in the element.
    :param vonMisesStressId: identifier of the Von Mises stress to read
                            (see NDMaterial and MembranePlateFiberSection).
    '''
    retval= GaussPointStresses(idComb= idComb, tagElem= tagElem, idGaussPoint= eval(key))
    strss= stresses[key]
    retval.setStressesFromDict(strss)
    if(vonMisesStressId in stresses):
        retval.vonMisesStress= stresses[vonMisesStressId]
    return retval

def read_stresses_dict(stressesCombFileName, setCalc=None, vonMisesStressId= 'max_von_mises_stress'):
    '''Extracts element and combination identifiers from the internal
    forces JSON file. Return elementTags, idCombs and stresses values
    
    :param stressesCombFileName: name of the file containing the stresses
                                 obtained for each element for 
                                 the combinations analyzed
    :param setCalc: set of elements to be analyzed (defaults to None which 
                    means that all the elements in the file of internal forces
                    results are analyzed)
    :param vonMisesStressId: identifier of the Von Mises stress to read
                            (see NDMaterial and MembranePlateFiberSection).
    '''        
    elementTags= set()
    idCombs= set()
    with open(stressesCombFileName) as json_file:
        combStressesDict= json.load(json_file)
    json_file.close()
    
    stressesValues= defaultdict(list)
    
    if(not setCalc):
        for comb in combStressesDict.keys():
            idComb= str(comb)
            idCombs.add(idComb)
            elements= combStressesDict[comb]
            for elemId in elements.keys():
                tagElem= eval(elemId)
                elementData= elements[elemId]
                #elementType= elementData['type']
                stresses= elementData['stresses']
                for k in stresses.keys():
                    elementTags.add(tagElem)
                    stresses= get_gauss_point_stresses(stresses= internalForces, idComb= idComb, tagElem= tagElem, key= k, vonMisesStressId= vonMisesStressId)
                    stressesValues[tagElem].append(stresses)
    else:
        setElTags= frozenset(setCalc.getElementTags()) # We construct a frozen set to accelerate searching.
        for idComb in combStressesDict.keys():
            idCombs.add(idComb)
            elements= combStressesDict[idComb]
            for elemId in elements.keys():
                tagElem= eval(elemId)
                if(tagElem in setElTags): # search element tag
                    elementData= elements[elemId]
                    #elementType= elementData['type']
                    internalForces= elementData['stresses']
                    for k in internalForces.keys():
                        elementTags.add(tagElem)
                        stresses= get_gauss_point_stresses(stresses= internalForces, idComb= idComb, tagElem= tagElem, key= k, vonMisesStressId= vonMisesStressId)
                        stressesValues[tagElem].append(stresses)
    return (elementTags, idCombs, stressesValues)


def old_read_int_forces_file(intForcCombFileName,setCalc=None):
    '''Extracts element and combination identifiers from the internal
    forces listing file. Return elementTags, idCombs and 
    internal-forces values
    
    :param   intForcCombFileName: name of the file containing the internal
                                  forces obtained for each element for 
                                  the combinations analyzed
    :param setCalc: set of elements to be analyzed (defaults to None which 
                    means that all the elements in the file of internal forces
                    results are analyzed) 
    '''
    # errMsg= 'old_read_int_forces_file will be deprecated soon.'
    # lmsg.error(errMsg)
    elementTags= set()
    idCombs= set()
    f= open(intForcCombFileName,"r")
    internalForcesValues= defaultdict(list)
    internalForcesListing= csv.reader(f)
    next(internalForcesListing) # skip first line (head)
    if setCalc is None:
        for lst in internalForcesListing: # lst: list of internal forces for each combination and element
            if(len(lst)>0):
                idComb= lst[0]
                idCombs.add(idComb)
                tagElem= eval(lst[1])
                idSection= eval(lst[2])
                elementTags.add(tagElem)
                crossSectionInternalForces= internal_forces.CrossSectionInternalForces()
                crossSectionInternalForces.setFromCSVString(lst,3)
                crossSectionInternalForces.idComb= idComb
                crossSectionInternalForces.tagElem= tagElem
                crossSectionInternalForces.idSection= idSection
                internalForcesValues[tagElem].append(crossSectionInternalForces)
    else:
        setElTags= frozenset(setCalc.getElementTags()) # We construct a frozen set to accelerate searching.
        for lst in internalForcesListing:
            if(len(lst)>0):
                tagElem= eval(lst[1])
                if tagElem in setElTags: # search element tag
                    idComb= lst[0]
                    idCombs.add(idComb)
                    tagElem= eval(lst[1])
                    idSection= eval(lst[2])
                    elementTags.add(tagElem)
                    crossSectionInternalForces= internal_forces.CrossSectionInternalForces()
                    crossSectionInternalForces.setFromCSVString(lst,3)
                    crossSectionInternalForces.idComb= idComb
                    crossSectionInternalForces.tagElem= tagElem
                    crossSectionInternalForces.idSection= idSection
                    internalForcesValues[tagElem].append(crossSectionInternalForces)
    f.close()
    return (elementTags,idCombs,internalForcesValues)

def read_internal_forces_file(intForcCombFileName, setCalc=None, vonMisesStressId= 'max_von_mises_stress'):
    '''Extracts element and combination identifiers from the internal
    forces listing file. Return elementTags, idCombs and 
    internal-forces values
    
    :param intForcCombFileName: name of the file containing the internal
                                forces obtained for each element for 
                                the combinations analyzed
    :param setCalc: set of elements to be analyzed (defaults to None which 
                    means that all the elements in the file of internal forces
                    results are analyzed) 
    :param vonMisesStressId: identifier of the Von Mises stress to read
                            (see NDMaterial and MembranePlateFiberSection).
    '''
    if('PlaneStress' in intForcCombFileName):
        retval= read_stresses_dict(stressesCombFileName= intForcCombFileName, setCalc= setCalc, vonMisesStressId= vonMisesStressId)
    else:
        f= open(intForcCombFileName,"r")
        c= f.read(1)
        if(c=='{'):
            retval= read_int_forces_dict(intForcCombFileName,setCalc, vonMisesStressId)
        else: # legacy file format.
            retval= old_read_int_forces_file(intForcCombFileName,setCalc)
        f.close()
    return retval

def string_el_max_axial_force(element,section,setName,combName,axialForc):
    retval='preprocessor.getElementHandler.getElement('+str(element)+').setProp("maxAxialForceSect'+str(section)+'",AxialForceControlVars('+'idSection= "' + setName + 'Sects'+str(section)+'"' + ', combName= "' + combName +'", N= ' + str(axialForc) + ')) \n'
    return retval
    
def calc_max_tension_axial_forces(setCalc,intForcCombFileName,outputFileName):
    '''Calculate maximum tension forces for the elements included in setCalc 
    among the load combinations for which internal-force results are stored in 
    intForcCombFileName. The maximum tension forces calculated are written to
    outputFileName file. 

    :param setCalc: set of elements to be analyzed.
    '''
    f=open(outputFileName,"w")
    etags,combs,intForc= read_internal_forces_file(intForcCombFileName,setCalc)
    setName=setCalc.name
    for el in etags:
        #init tension axial forces
        maxNsect1,maxNsect2=(0.0,0.0)
        maxCmbsect1,maxCmbsect2=('','')
        elIntF=intForc[el]
        for ind in range(len(combs)):
            Nsect1=elIntF[2*ind].N
            Nsect2=elIntF[2*ind+1].N
            if Nsect1>maxNsect1:
                maxNsect1=Nsect1
                maxCmbsect1=elIntF[2*ind].idComb
            if Nsect2>maxNsect2:
                maxNsect2=Nsect2
                maxCmbsect2=elIntF[2*ind+1].idComb
        strSect1=string_el_max_axial_force(el,1,setName,maxCmbsect1,maxNsect1)
        strSect2=string_el_max_axial_force(el,2,setName,maxCmbsect2,maxNsect2)
        f.write(strSect1) 
        f.write(strSect2) 
    f.close()

def calc_max_compression_axial_forces(setCalc,intForcCombFileName,outputFileName):
    '''Calculate maximum compression forces for the elements included in setCalc 
    among the load combinations for which internal-force results are stored in 
    intForcCombFileName. The maximum tension forces calculated are written to
    outputFileName file.

    :param setCalc: set of elements to be analyzed.
    '''
    f=open(outputFileName,"w")
    etags,combs,intForc= read_internal_forces_file(intForcCombFileName,setCalc)
    setName= setCalc.name
    for el in etags:
        #init compression axial forces
        maxNsect1,maxNsect2=(0.0,0.0)
        maxCmbsect1,maxCmbsect2=('','')
        elIntF=intForc[el]
        for ind in range(len(combs)):
            Nsect1=elIntF[2*ind].N
            Nsect2=elIntF[2*ind+1].N
            if Nsect1<maxNsect1:
                maxNsect1=Nsect1
                maxCmbsect1=elIntF[2*ind].idComb
            if Nsect2<maxNsect2:
                maxNsect2=Nsect2
                maxCmbsect2=elIntF[2*ind+1].idComb
        strSect1=string_el_max_axial_force(el,1,setName,maxCmbsect1,maxNsect1)
        strSect2=string_el_max_axial_force(el,2,setName,maxCmbsect2,maxNsect2)
        f.write(strSect1) 
        f.write(strSect2) 
    f.close()



