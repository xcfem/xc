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
from solution import predefined_solutions
from postprocess.reports import export_internal_forces as eif
from postprocess.reports import export_reactions as er
from misc_utils import log_messages as lmsg
from materials.sections import internal_forces
from collections import defaultdict
import csv
from postprocess import control_vars as cv
import json

defaultSolutionProcedureType=  predefined_solutions.SimpleStaticLinear

class VerifOutVars(object):
    '''Variables that control the output of limit state verifications.

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
    '''
    def __init__(self,setCalc=None,appendToResFile='N',listFile='N',calcMeanCF='N', controller= None):
        self.setCalc= setCalc
        self.appendToResFile= appendToResFile
        self.listFile= listFile
        self.calcMeanCF= calcMeanCF
        self.controller= controller

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

    def runChecking(self, intForcCombFileName, outputDataBaseFileName, sections):
        '''Launch checking.

        :param intForcCombFileName: name of the file to read the internal force results.
        :param sections: names of the sections to write the output for.
        '''
        retval=None
        if(self.setCalc):
            self.controller.initControlVars(self.setCalc)
            self.controller.checkSetFromIntForcFile(intForcCombFileName,self.setCalc)
            prep= self.setCalc.getPreprocessor
            retval= cv.writeControlVarsFromElements(prep,outputDataBaseFileName,self, sections)
        else:
            lmsg.error("Result file hasn't been created, you must specify a valid set of elements")
        return retval
        

class LimitStateData(object):
    ''' Data used when checking limit states.

    :ivar label: limit state check label; Something like "Fatigue" 
                 or "CrackControl".
    :ivar outputDataBaseFileName: name (whithout extension) of the 
                                       file that contains the results to
                                       display.
    '''
    
    envConfig= None

    def __init__(self,limitStateLabel,outputDataBaseFileName):
        '''Limit state data constructor.


        :param label: limit state check label; Something like "Fatigue" 
                      or "CrackControl".
        :param outputDataBaseFileName: name (whithout extension) of the 
                                       file that contains the results to
                                       display.
        '''
        self.label= limitStateLabel
        self.outputDataBaseFileName= outputDataBaseFileName
        
    def getInternalForcesFileName(self):
        '''Return the name of the file where internal forces are stored.'''
        return self.envConfig.projectDirTree.getInternalForcesResultsPath()+'intForce_'+ self.label +'.json'
    
    def getReactionsFileName(self):
        '''Return the name of the file where reactions are stored.'''
        return self.envConfig.projectDirTree.getReactionsResultsPath()+'reactions_'+ self.label +'.json'
        
    def readInternalForces(self, setCalc):
        ''' Read the internal forces for the elements in the set argument.

        :param setCalc: elements to read internal forces for.
        '''
        return readIntForcesFile(self.getInternalForcesFileName(), setCalc)
    
    def readReactions(self, nodeSet):
        ''' Read the reactions for the nodes in the set argument.

        :param nodeSet: nodes to read reactions for.
        '''
        return readReactionsFile(self.getReactionsFileName(), nodeSet)

    def getInternalForcesDict(self, nmbComb, elems):
        '''Creates a dictionary with the element's internal forces.

         :param nmbComb: combination name.
         :param elems: element set.
        '''
        return eif.getInternalForcesDict(nmbComb,elems)
    
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
    
    def loadPickleObject(objName):
        '''Read a Python object from a pickle file.'''
        with open(name + '.pkl', 'r') as f:
            return pickle.load(f)

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
        os.system("rm -f " + self.fNameIntForc) #Clear obsolete files.
        os.system("rm -f " + self.fNameReactions)
        os.system("rm -f " + self.fNameDispl)
        fDisp= open(self.fNameDispl,"w")
        fDisp.write('Comb., Node, uX, uY, uZ, rotX, rotY , rotZ\n')
        fDisp.close()

    def writeDisplacements(self, combNm, nodSet):
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

    def writeInternalForces(self, internalForcesDict):
        '''Write the internal forces results.

        :param internalForcesDict: dictionary containing the internal forces.
        '''
        with open(self.fNameIntForc, 'w') as outfile:
            json.dump(internalForcesDict, outfile)
        outfile.close()
        
    def writeReactions(self, reactionsDict):
        '''Write the reactions.

        :param reactionsDict: dictionary containing the reactions.
        '''
        with open(self.fNameReactions, 'w') as outfile:
            json.dump(reactionsDict, outfile)
        outfile.close()
        
    def saveAll(self, combContainer, setCalc, solutionProcedureType= defaultSolutionProcedureType, lstSteelBeams=None, constrainedNodeSet= None):
        '''Write internal forces, displacements, .., for each combination

        :param setCalc: set of entities for which the verification is 
                          going to be performed
        :param solutionProcedureType: type of the solution strategy to solve
                                      the finite element problem.
        :param lstSteelBeams: list of steel beams to analyze (defaults to None)
        :param constrainedNodeSet: constrained nodes (defaults to None)
        '''
        preprocessor= setCalc.getPreprocessor
        feProblem= preprocessor.getProblem
        solutionProcedure= solutionProcedureType(feProblem)
        preprocessor= feProblem.getPreprocessor
        loadCombinations= preprocessor.getLoadHandler.getLoadCombinations
        #Putting combinations inside XC.
        loadCombinations= self.dumpCombinations(combContainer,loadCombinations)
        elemSet= setCalc.elements
        nodSet= setCalc.nodes
        self.createOutputFiles()
        internalForcesDict= dict()
        reactionsDict= dict()
        for key in loadCombinations.getKeys():
            comb= loadCombinations[key]
            preprocessor.resetLoadCase()
            preprocessor.getDomain.revertToStart()
            comb.addToDomain() #Combination to analyze.
            #Solution
            result= solutionProcedure.solve()
            if lstSteelBeams:
                for sb in lstSteelBeams:
                    sb.updateReductionFactors()
            #Writing results.
            internalForcesDict.update(self.getInternalForcesDict(comb.getName,elemSet))
            reactionsDict.update(self.getReactionsDict(comb.getName,constrainedNodeSet))
            self.writeDisplacements(comb.getName,nodSet)
            comb.removeFromDomain() #Remove combination from the model.
        self.writeInternalForces(internalForcesDict)
        self.writeReactions(reactionsDict)
#20181117
    def runChecking(self, outputCfg, sections= ['Sect1', 'Sect2']):
        '''This method reads, for the elements in setCalc,  the internal 
        forces previously calculated and saved in the corresponding file.
        Using the 'initControlVars' and 'checkSetFromIntForcFile' methods of 
        the controller, the appropiate attributes are assigned to the 
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
        return outputCfg.runChecking(intForcCombFileName, self.getOutputDataBaseFileName(), sections)

#20181117 end

class NormalStressesRCLimitStateData(LimitStateData):
    ''' Reinforced concrete normal stresses data for limit state checking.'''
    def __init__(self):
        '''Constructor '''
        super(NormalStressesRCLimitStateData,self).__init__('ULS_normalStressesResistance','verifRsl_normStrsULS')

    def dumpCombinations(self,combContainer,loadCombinations):
        '''Load into the solver the combinations needed for this limit state.

        :param combContainer: container with the definition of the different
                              combination families (ULS, fatigue, SLS,...)
                              see actions/combinations module.
        :param loadCombinations: load combination handler inside the XC solver.
        '''
        loadCombinations.clear()
        combContainer.ULS.perm.dumpCombinations(loadCombinations)
        return loadCombinations

    def check(self,reinfConcreteSections,outputCfg= VerifOutVars()):
        '''Checking of normal stresses in ultimate limit states
        (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each 
               element.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to a file,
               generation or not of lists, ...)
         '''
        return reinfConcreteSections.internalForcesVerification3D(self, "d",outputCfg)

class ShearResistanceRCLimitStateData(LimitStateData):
    ''' Reinforced concrete shear resistance limit state data.'''
    def __init__(self):
        '''Limit state data constructor '''
        super(ShearResistanceRCLimitStateData,self).__init__('ULS_shearResistance','verifRsl_shearULS')
        
    def dumpCombinations(self,combContainer,loadCombinations):
        '''Load into the solver the combinations needed for this limit state.

        :param combContainer: container with the definition of the different
                              combination families (ULS, fatigue, SLS,...)
                              see actions/combinations module.
        :param loadCombinations: load combination handler inside the XC solver.
        '''
        loadCombinations.clear()
        combContainer.ULS.perm.dumpCombinations(loadCombinations)
        return loadCombinations
    def check(self,reinfConcreteSections,outputCfg= VerifOutVars()):
        '''Checking of shear resistance in ultimate limit states 
        (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each 
               element.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to a file,
               generation or not of lists, ...)
        '''
        return reinfConcreteSections.internalForcesVerification3D(self,"d",outputCfg)

class FreqLoadsCrackControlRCLimitStateData(LimitStateData):
    ''' Reinforced concrete crack control under frequent loads limit state data.'''
    def __init__(self):
        '''Limit state data constructor '''
        super(FreqLoadsCrackControlRCLimitStateData,self).__init__('SLS_frequentLoadsCrackControl','verifRsl_crackingSLS_freq')
    def dumpCombinations(self,combContainer,loadCombinations):
        '''Load into the solver the combinations needed for this limit state.

        :param combContainer: container with the definition of the different
                              combination families (ULS, fatigue, SLS,...)
                              see actions/combinations module.
        :param loadCombinations: load combination handler inside the XC solver.
        '''
        loadCombinations.clear()
        combContainer.SLS.freq.dumpCombinations(loadCombinations)
        return loadCombinations
    def check(self,reinfConcreteSections,outputCfg= VerifOutVars()):
        '''Checking of crack width under frequent loads in serviceability limit states 
           (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each 
               element.
        :param outputCfg: instance of class VerifOutVars which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to file,
               generation or not of lists, ...)
         '''
        return reinfConcreteSections.internalForcesVerification3D(self, "k",outputCfg)

class QPLoadsCrackControlRCLimitStateData(LimitStateData):
    ''' Reinforced concrete crack control under quasi-permanent loads limit state data.'''
    def __init__(self):
        '''Limit state data constructor '''
        super(QPLoadsCrackControlRCLimitStateData,self).__init__('SLS_quasiPermanentLoadsLoadsCrackControl','verifRsl_crackingSLS_qperm')
    def dumpCombinations(self,combContainer,loadCombinations):
        '''Load into the solver the combinations needed for this limit state.

        :param combContainer: container with the definition of the different
                              combination families (ULS, fatigue, SLS,...)
                              see actions/combinations module.
        :param loadCombinations: load combination handler inside the XC solver.
        '''
        loadCombinations.clear()
        combContainer.SLS.qp.dumpCombinations(loadCombinations)
        return loadCombinations

    def check(self,reinfConcreteSections,outputCfg= VerifOutVars()):
        '''Checking of crack width under quasi-permanent loads in
        serviceability limit states (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each 
               element.
        :param outputCfg: instance of class VerifOutVars which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to file,
               generation or not of lists, ...)
        '''
        return reinfConcreteSections.internalForcesVerification3D(self,"k",outputCfg)

class FreqLoadsDisplacementControlLimitStateData(LimitStateData):
    ''' Displacement control under frequent loads limit state data.'''
    def __init__(self):
        '''Limit state data constructor '''
        super(FreqLoadsDisplacementControlLimitStateData,self).__init__('SLS_frequentLoadsDisplacementControl','')
    def dumpCombinations(self,combContainer,loadCombinations):
        '''Load into the solver the combinations needed for this limit state.

        :param combContainer: container with the definition of the different
                              combination families (ULS, fatigue, SLS,...)
                              see actions/combinations module.
        :param loadCombinations: load combination handler inside the XC solver.
        '''
        loadCombinations.clear()
        combContainer.SLS.freq.dumpCombinations(loadCombinations)
        return loadCombinations
    def check(self,reinfConcreteSections):
        '''Checking of displacements under frequent loads in
        serviceability limit states (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each element.
        '''
        lmsg.error('FreqLoadsDisplacementControlLimitStateData.check() not implemented.')

class FatigueResistanceRCLimitStateData(LimitStateData):
    ''' Reinforced concrete shear resistance limit state data.'''
    def __init__(self):
        '''Limit state data constructor '''
        super(FatigueResistanceRCLimitStateData,self).__init__('ULS_fatigueResistance','verifRsl_fatigueULS')
        
    def dumpCombinations(self,combContainer,loadCombinations):
        '''Load into the solver the combinations needed for this limit state.

        :param combContainer: container with the definition of the different
                              combination families (ULS, fatigue, SLS,...)
                              see actions/combinations module.
        :param loadCombinations: load combination handler inside the XC solver.
        '''
        loadCombinations.clear()
        combContainer.ULS.fatigue.dumpCombinations(loadCombinations)
        return loadCombinations
    
    def check(self,reinfConcreteSections,outputCfg= VerifOutVars()):
        '''Checking of fatigue under fatigue combinations loads in
        ultimate limit states (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each 
               element.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to a file,
               generation or not of lists, ...)
        '''
        return reinfConcreteSections.internalForcesVerification3D(self, "d",outputCfg)

class VonMisesStressLimitStateData(LimitStateData):
    ''' Steel Von Mises stress limit state data.'''
    def __init__(self, vonMisesStressId= 'max_von_mises_stress'):
        '''Von Mises stress limit state data constructor.

        :param vonMisesStressId: identifier of the Von Mises stress to read
                                (see NDMaterial and MembranePlateFiberSection).
        '''
        super(VonMisesStressLimitStateData,self).__init__('ULS_VonMisesStressResistance','verifRsl_VonMisesStressULS')
        self.vonMisesStressId= vonMisesStressId
        
    def getInternalForcesDict(self, nmbComb, elems):
        '''Creates a dictionary with the element's internal forces.

        :param nmbComb: combination name.
        :param elems: element set.
        '''
        return eif.getInternalForcesDict(nmbComb,elems, vonMisesStressId= self.vonMisesStressId)
    
    def readInternalForces(self, setCalc):
        ''' Read the internal forces for the elements in the set argument.

        :param setCalc: elements to read internal forces for.
        '''
        return readIntForcesFile(self.getInternalForcesFileName(), setCalc, vonMisesStressId= self.vonMisesStressId)
    
    def dumpCombinations(self,combContainer,loadCombinations):
        '''Load into the solver the combinations needed for this limit state.

        :param combContainer: container with the definition of the different
                              combination families (ULS, fatigue, SLS,...)
                              see actions/combinations module.
        :param loadCombinations: load combination handler inside the XC solver.
        '''
        loadCombinations.clear()
        combContainer.ULS.perm.dumpCombinations(loadCombinations)
        return loadCombinations
    
    def check(self,elementsToCheck,outputCfg= VerifOutVars()):
        '''Checking of fatigue under fatigue combinations loads in
        ultimate limit states (see self.dumpCombinations).

        :param elementsToCheck: elements to check.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to a file,
               generation or not of lists, ...)
        '''
        return elementsToCheck.internalForcesVerification3D(self, "d",outputCfg)
    
    def runChecking(self,outputCfg):
        '''This method reads, for the elements in setCalc,  the internal 
        forces previously calculated and saved in the corresponding file.
        Using the 'initControlVars' and 'checkSetFromIntForcFile' methods of 
        the controller, the appropiate attributes are assigned to the 
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


freqLoadsDisplacementControl= FreqLoadsDisplacementControlLimitStateData()
freqLoadsCrackControl= FreqLoadsCrackControlRCLimitStateData()
quasiPermanentLoadsCrackControl= QPLoadsCrackControlRCLimitStateData()
normalStressesResistance= NormalStressesRCLimitStateData()
shearResistance= ShearResistanceRCLimitStateData()
fatigueResistance= FatigueResistanceRCLimitStateData()
vonMisesStressResistance= VonMisesStressLimitStateData()

def readIntForcesDict(intForcCombFileName,setCalc=None, vonMisesStressId= 'max_von_mises_stress'):
    '''Extracts element and combination identifiers from the internal
    forces JSON file. Return elementTags, idCombs and 
    internal-forces values
    
    :param   intForcCombFileName: name of the file containing the internal
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
                elementType= elementData['type']
                internalForces= elementData['internalForces']
                for k in internalForces.keys():
                    idSection= eval(k)
                    elementTags.add(tagElem)
                    crossSectionInternalForces= internal_forces.CrossSectionInternalForces()
                    forces= internalForces[k]
                    crossSectionInternalForces.setFromDict(forces)
                    crossSectionInternalForces.idComb= idComb
                    crossSectionInternalForces.tagElem= tagElem
                    crossSectionInternalForces.idSection= idSection
                    if(vonMisesStressId in forces):
                        crossSectionInternalForces.vonMisesStress= forces[vonMisesStressId]
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
                    elementType= elementData['type']
                    internalForces= elementData['internalForces']
                    for k in internalForces.keys():
                        idSection= eval(k)
                        elementTags.add(tagElem)
                        crossSectionInternalForces= internal_forces.CrossSectionInternalForces()
                        forces= internalForces[k]
                        crossSectionInternalForces.setFromDict(forces)
                        crossSectionInternalForces.idComb= idComb
                        crossSectionInternalForces.tagElem= tagElem
                        crossSectionInternalForces.idSection= idSection
                        if(vonMisesStressId in forces):
                            crossSectionInternalForces.vonMisesStress= forces[vonMisesStressId]
                        internalForcesValues[tagElem].append(crossSectionInternalForces)
    return (elementTags,idCombs,internalForcesValues)

def oldReadIntForcesFile(intForcCombFileName,setCalc=None):
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
    errMsg= 'oldReadIntForcesFile will be deprecated soon.'
    #lmsg.error(errMsg)
    elementTags= set()
    idCombs= set()
    f= open(intForcCombFileName,"r")
    internalForcesValues= defaultdict(list)
    internalForcesListing= csv.reader(f)
    next(internalForcesListing) # skip first line (head)
    if setCalc==None:
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

def readIntForcesFile(intForcCombFileName, setCalc=None, vonMisesStressId= 'max_von_mises_stress'):
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
    f= open(intForcCombFileName,"r")
    c= f.read(1)
    if(c=='{'):
        return readIntForcesDict(intForcCombFileName,setCalc, vonMisesStressId)
    else:
        return oldReadIntForcesFile(intForcCombFileName,setCalc)
    f.close()


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
    etags,combs,intForc=readIntForcesFile(intForcCombFileName,setCalc)
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
    etags,combs,intForc=readIntForcesFile(intForcCombFileName,setCalc)
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



