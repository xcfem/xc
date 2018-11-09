# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2016,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

import pickle
import os
from solution import predefined_solutions
from postprocess.reports import export_internal_forces as eif
from postprocess.reports import export_displacements as edisp
from miscUtils import LogMessages as lmsg
from materials.sections import internal_forces
from collections import defaultdict
import csv

def defaultAnalysis(feProb,steps= 1):
    '''Default analysis procedure for saveAll method.'''
    analysis= predefined_solutions.simple_static_linear(feProb)
    result= analysis.analyze(steps) #Same with the number of steps.
    return result

class LimitStateData(object):
    check_results_directory= './' #Path to verifRsl* files.
    internal_forces_results_directory= './' #Path to esf_el* f
    def __init__(self,limitStateLabel,outputDataBaseFileName):
        '''Limit state data constructor
        label; limit state check label; Something like "Fatigue" or "CrackControl"
        outputDataBaseFileName: name (whithout extension) of the file that contains
        the results to display.
        '''
        self.label= limitStateLabel
        self.outputDataBaseFileName= outputDataBaseFileName
        self.controller= None
    def getInternalForcesFileName(self):
        '''Return the file name to read: combination name, element number and 
        internal forces.'''
        return self.internal_forces_results_directory+'intForce_'+ self.label +'.csv'
    def getDisplacementsFileName(self):
        '''Return the file name to read: combination name, node number and 
        displacements (ux,uy,uz,rotX,rotY,rotZ).'''
        return self.internal_forces_results_directory+'displ_'+ self.label +'.csv'
    def getOutputDataBaseFileName(self):
        '''Return the output file name without extension.'''
        return self.check_results_directory+self.outputDataBaseFileName
    def getOutputDataFileName(self):
        '''Return the Python executable file name.'''
        return self.getOutputDataBaseFileName() + '.py'
    def loadPickleObject(objName):
        '''Read a Python object from a pickle file.'''
        with open(name + '.pkl', 'r') as f:
            return pickle.load(f)
    def saveAll(self,feProblem,combContainer,setCalc,fConvIntForc= 1.0,analysisToPerform= defaultAnalysis):
        '''Write internal forces, displacements, .., for each combination

        :param feProblem: XC finite element problem to deal with.
        :param setCalc: set of entities for which the verification is 
                          going to be performed
        :param fConvIntForc: conversion factor between the unit of force 
                               in which the calculation is performed and that 
                               one desired for the displaying of internal forces
                               (The use of this factor won't be allowed in
                                future versions)
        '''
        if fConvIntForc != 1.0:
          lmsg.warning('fConvIntForc= ' + fConvIntForc + 'conversion factor between units is DEPRECATED' )
        preprocessor= feProblem.getPreprocessor
        loadCombinations= preprocessor.getLoadHandler.getLoadCombinations
        #Putting combinations inside XC.
        loadCombinations= self.dumpCombinations(combContainer,loadCombinations)
        elemSet= setCalc.getElements
        nodSet= setCalc.getNodes
        fNameInfForc= self.getInternalForcesFileName()
        fNameDispl= self.getDisplacementsFileName()
        os.system("rm -f " + fNameInfForc) #Clear obsolete files.
        os.system("rm -f " + fNameDispl)
        fIntF= open(fNameInfForc,"a")
        fDisp= open(fNameDispl,"a")
        fIntF.write(" Comb. , Elem. , Sect. , N , Vy , Vz , T , My , Mz \n")
        fDisp.write(" Comb. , Node , Ux , Uy , Uz , ROTx , ROTy , ROTz \n")
        fIntF.close()
        fDisp.close()
        for key in loadCombinations.getKeys():
            comb= loadCombinations[key]
            feProblem.getPreprocessor.resetLoadCase()
            comb.addToDomain() #Combination to analyze.
            #Solution
            result= analysisToPerform(feProblem)
            #Writing results.
            fIntF= open(fNameInfForc,"a")
            fDisp= open(fNameDispl,"a")
            eif.exportInternalForces(comb.getName,elemSet,fIntF)
            edisp.exportDisplacements(comb.getName,nodSet,fDisp)
            fIntF.close()
            fDisp.close()
            comb.removeFromDomain() #Remove combination from the model.

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

    def check(self,reinfConcreteSections,setCalc=None):
        '''Checking of normal stresses in ultimate limit states
        (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each element.
        :param setCalc: set of elements to be analyzed (defaults to None which 
                        means that all the elements in the file of internal forces
                        results are analyzed) 
         '''
        return reinfConcreteSections.internalForcesVerification3D(self, "d",setCalc)

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
    def check(self,reinfConcreteSections,setCalc=None):
        '''Checking of shear resistance in ultimate limit states 
        (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each element.
        :param setCalc: set of elements to be analyzed (defaults to None which 
                        means that all the elements in the file of internal forces
                        results are analyzed) 
        '''
        return reinfConcreteSections.internalForcesVerification3D(self,"d",setCalc)

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
    def check(self,reinfConcreteSections,setCalc=None):
        '''Checking of crack width under frequent loads in serviceability limit states 
           (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each element.
        :param setCalc: set of elements to be analyzed (defaults to None which 
                        means that all the elements in the file of internal forces
                        results are analyzed) 
        '''
        return reinfConcreteSections.internalForcesVerification3D(self, "k",setCalc)

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

    def check(self,reinfConcreteSections,setCalc=None):
        '''Checking of crack width under quasi-permanent loads in
        serviceability limit states (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each element.
        :param setCalc: set of elements to be analyzed (defaults to None which 
                        means that all the elements in the file of internal forces
                        results are analyzed) 
        '''
        return reinfConcreteSections.internalForcesVerification3D(self,"k",setCalc)

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
    def check(self,reinfConcreteSections,setCalc=None):
        '''Checking of fatigue under fatigue combinations loads in
        ultimate limit states (see self.dumpCombinations).

        :param reinfConcreteSections: Reinforced concrete sections on each element.
        :param setCalc: set of elements to be analyzed (defaults to None which 
                        means that all the elements in the file of internal forces
                        results are analyzed) 
        '''
        return reinfConcreteSections.internalForcesVerification3D(self, "d",setCalc)


freqLoadsDisplacementControl= FreqLoadsDisplacementControlLimitStateData()
freqLoadsCrackControl= FreqLoadsCrackControlRCLimitStateData()
quasiPermanentLoadsCrackControl= QPLoadsCrackControlRCLimitStateData()
normalStressesResistance= NormalStressesRCLimitStateData()
shearResistance= ShearResistanceRCLimitStateData()
fatigueResistance= FatigueResistanceRCLimitStateData()

def readIntForcesFile(intForcCombFileName,setCalc=None):
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
    elementTags= set()
    idCombs= set()
    f= open(intForcCombFileName,"r")
    internalForcesValues= defaultdict(list)
    internalForcesListing= csv.reader(f)
    internalForcesListing.next()    #skip first line (head)
    if setCalc==None:
        for lst in internalForcesListing:    #lst: list of internal forces for each combination and element
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
        setElTags=setCalc.getElementTags()
        for lst in internalForcesListing:
            if(len(lst)>0):
                tagElem= eval(lst[1])
                if tagElem in setElTags:
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



