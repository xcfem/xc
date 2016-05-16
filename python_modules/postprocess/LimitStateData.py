# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import pickle
import os
from solution import predefined_solutions
from postprocess.reports import export_internal_forces as eif

class LimitStateData(object):
  check_results_directory= './' #Path to verifRsl* files.
  internal_forces_results_directory= './' #Path to esf_el* f
  def __init__(self,limitStateLabel,inputDataFileName,outputDataBaseFileName):
    '''Limit state data constructor
       label; limit state check label; Something like "Fatigue" or "CrackControl"
       inputDataFileName: name of the file with the analysis results.
       outputDataBaseFileName: name (whithout extension) of the file that contains the results to display.
    '''
    self.label= limitStateLabel
    self.inputDataFileName= inputDataFileName
    self.outputDataBaseFileName= outputDataBaseFileName
    self.controller= None
  def getInternalForcesFileName(self):
    return self.internal_forces_results_directory+self.inputDataFileName #+ 'intForce_'+ self.label +'.csv'
  def getOutputDataBaseFileName(self):
    '''Returns the output file name without extension.'''
    return self.check_results_directory+self.outputDataBaseFileName
  def getOutputDataFileName(self):
    '''Returns the Python executable file name.'''
    return self.getOutputDataBaseFileName() + '.py'
  def loadPickleObject(objName):
    with open(name + '.pkl', 'r') as f:
      return pickle.load(f)
  def saveInternalForcesForCombs(self,model,combContainer,elemSet,fConv= 1.0):
    '''Writes internal forces for each combination'''
    feProblem= model.getFEProblem()
    preprocessor= model.getPreprocessor()
    loadCombinations= preprocessor.getLoadLoader.getLoadCombinations
    loadCombinations= self.dumpCombinations(combContainer,loadCombinations)
    fName= self.getInternalForcesFileName()
    os.system("rm -f " + fName)
    for key in loadCombinations.getKeys():
      comb= loadCombinations[key]
      feProblem.getPreprocessor.resetLoadCase()
      comb.addToDomain()
      #Solución
      analisis= predefined_solutions.simple_static_linear(feProblem)
      result= analisis.analyze(1)
      f= open(fName,"a")
      eif.exportShellInternalForces(comb.getName,elemSet,f,fConv)
      f.close()
      comb.removeFromDomain()

class NormalStressesRCLimitStateData(LimitStateData):
  ''' Reinforced concrete normal stresses limit state data.'''
  def __init__(self):
    '''Limit state data constructor '''
    super(NormalStressesRCLimitStateData,self).__init__('ULS_normalStressesResistance','intForc_normShULS.csv','verifRsl_normStrsULS')

  def dumpCombinations(self,combContainer,loadCombinations):
    '''Load into the solver the combinations needed for this limit state.'''
    loadCombinations.clear()
    combContainer.ULS.dumpCombinations(loadCombinations)
    return loadCombinations

  def check(self,sections,sectionsNamesForEveryElement):
    intForcCombFileName= self.getInternalForcesFileName()
    out= self.getOutputDataBaseFileName()
    return sections.verifyNormalStresses(intForcCombFileName,out,sectionsNamesForEveryElement, "d",self.controller)

class ShearResistanceRCLimitStateData(LimitStateData):
  ''' Reinforced concrete shear resistance limit state data.'''
  def __init__(self):
    '''Limit state data constructor '''
    super(ShearResistanceRCLimitStateData,self).__init__('ULS_shearResistance','intForc_normShULS.csv','verifRsl_shearULS')
  def dumpCombinations(self,combContainer,loadCombinations):
    '''Load into the solver the combinations needed for this limit state.'''
    loadCombinations.clear()
    combContainer.ULS.dumpCombinations(loadCombinations)
    return loadCombinations
  def check(self,sections,sectionsNamesForEveryElement):
    intForcCombFileName= self.getInternalForcesFileName()
    out= self.getOutputDataBaseFileName()
    return sections.shearVerification(intForcCombFileName,out,sectionsNamesForEveryElement, "d",self.controller)

class FreqLoadsCrackControlRCLimitStateData(LimitStateData):
  ''' Reinforced concrete crack control under frequent loads limit state data.'''
  def __init__(self):
    '''Limit state data constructor '''
    super(FreqLoadsCrackControlRCLimitStateData,self).__init__('SLS_frequentLoadsCrackControl','intForc_crackingSLS_freq.csv','verifRsl_crackingSLS_freq')
  def dumpCombinations(self,combContainer,loadCombinations):
    '''Load into the solver the combinations needed for this limit state.'''
    loadCombinations.clear()
    combContainer.SLS.freq.dumpCombinations(loadCombinations)
    return loadCombinations
  def check(self,sections,sectionsNamesForEveryElement):
    intForcCombFileName= self.getInternalForcesFileName()
    out= self.getOutputDataBaseFileName()
    return sections.crackControl(intForcCombFileName,out,sectionsNamesForEveryElement, "k", self.controller)

class QPLoadsCrackControlRCLimitStateData(LimitStateData):
  ''' Reinforced concrete crack control under quasi-permanent loads limit state data.'''
  def __init__(self):
    '''Limit state data constructor '''
    super(QPLoadsCrackControlRCLimitStateData,self).__init__('SLS_quasiPermanentLoadsLoadsCrackControl','intForc_crackingSLS_qperm.csv','verifRsl_crackingSLS_qperm')
  def dumpCombinations(self,combContainer,loadCombinations):
    '''Load into the solver the combinations needed for this limit state.'''
    loadCombinations.clear()
    combContainer.SLS.qp.dumpCombinations(loadCombinations)
    return loadCombinations
  def check(self,sections,sectionsNamesForEveryElement):
    intForcCombFileName= self.getInternalForcesFileName()
    out= self.getOutputDataBaseFileName()
    return sections.crackControl(intForcCombFileName,out,sectionsNamesForEveryElement, "k",self.controller)

class FreqLoadsDisplacementControlLimitStateData(LimitStateData):
  ''' Displacement control under frequent loads limit state data.'''
  def __init__(self):
    '''Limit state data constructor '''
    super(FreqLoadsDisplacementControlLimitStateData,self).__init__('SLS_frequentLoadsDisplacementControl','','')
  def dumpCombinations(self,combContainer,loadCombinations):
    '''Load into the solver the combinations needed for this limit state.'''
    loadCombinations.clear()
    combContainer.SLS.freq.dumpCombinations(loadCombinations)
    return loadCombinations
  def check(self,sections,sectionsNamesForEveryElement):
    print 'FreqLoadsDisplacementControlLimitStateData.check() not implemented.'


class FatigueResistanceRCLimitStateData(LimitStateData):
  ''' Reinforced concrete shear resistance limit state data.'''
  def __init__(self):
    '''Limit state data constructor '''
    super(FatigueResistanceRCLimitStateData,self).__init__('ULS_fatigueResistance','intForc_fatigueULS.csv','verifRsl_fatigueULS')
  def dumpCombinations(self,combContainer,loadCombinations):
    '''Load into the solver the combinations needed for this limit state.'''
    loadCombinations.clear()
    combContainer.ULS.fatigue.dumpCombinations(loadCombinations)
    return loadCombinations
  def check(self,sections,sectionsNamesForEveryElement):
    intForcCombFileName= self.getInternalForcesFileName()
    out= self.getOutputDataBaseFileName()
    return sections.fatigueVerification(intForcCombFileName,out,sectionsNamesForEveryElement, "d",self.controller)


freqLoadsDisplacementControl= FreqLoadsDisplacementControlLimitStateData()
freqLoadsCrackControl= FreqLoadsCrackControlRCLimitStateData()
quasiPermanentLoadsCrackControl= QPLoadsCrackControlRCLimitStateData()
normalStressesResistance= NormalStressesRCLimitStateData()
shearResistance= ShearResistanceRCLimitStateData()
fatigueResistance= FatigueResistanceRCLimitStateData()




