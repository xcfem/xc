# -*- coding: utf-8 -*-

import pickle

class LimitStateData(object):
  check_results_directory= './' #Path to results_verif* files.
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
  def getInternalForcesFileName(self):
    return self.internal_forces_results_directory+self.inputDataFileName
  def getOutputDataBaseFileName(self):
    '''Returns the output file name without extension.'''
    return self.check_results_directory+self.outputDataBaseFileName
  def getOutputDataFileName(self):
    '''Returns the Python executable file name.'''
    return self.getOutputDataBaseFileName() + '.py'
  def loadPickleObject(objName):
    with open(name + '.pkl', 'r') as f:
      return pickle.load(f)

class NormalStressesRCLimitStateData(LimitStateData):
  ''' Reinforced concrete normal stresses limit state data.'''
  def __init__(self):
    '''Limit state data constructor '''
    super(NormalStressesRCLimitStateData,self).__init__('normalStressesResistance','esf_elu_t2.csv','results_verif_tn_eluT2')

  def createCombinations(self,combContainer,loadCombinations):
    '''Load into the solver the combinations needed for this limit state.'''
    loadCombinations.clear()
    combContainer.ULS.createCombinations(loadCombinations)
    return loadCombinations

  def check(self,sections,preprocessor,analysis,mapSectionsForEveryElement):
    intForcCombFileName= self.getInternalForcesFileName()
    out= self.getOutputDataBaseFileName()
    return sections.verifyNormalStresses(preprocessor,analysis,intForcCombFileName,out,mapSectionsForEveryElement, "d")

class ShearResistanceRCLimitStateData(LimitStateData):
  ''' Reinforced concrete shear resistance limit state data.'''
  def __init__(self):
    '''Limit state data constructor '''
    super(ShearResistanceRCLimitStateData,self).__init__('shearResistance','esf_elu_t2.csv','results_verif_v_eluT2')
  def createCombinations(self,combContainer,loadCombinations):
    '''Load into the solver the combinations needed for this limit state.'''
    loadCombinations.clear()
    combContainer.ULS.createCombinations(loadCombinations)
    return loadCombinations
  def check(self,sections,preprocessor,analysis,mapSectionsForEveryElement):
    intForcCombFileName= self.getInternalForcesFileName()
    out= self.getOutputDataBaseFileName()
    return sections.shearVerification(preprocessor,analysis,intForcCombFileName,out,mapSectionsForEveryElement, "d")

class FreqLoadsCrackControlRCLimitStateData(LimitStateData):
  ''' Reinforced concrete crack control under frequent loads limit state data.'''
  def __init__(self):
    '''Limit state data constructor '''
    super(FreqLoadsCrackControlRCLimitStateData,self).__init__('frequentLoadsCrackControl','esf_els_freq.csv','results_verif_fis_els_freq')
  def createCombinations(self,combContainer,loadCombinations):
    '''Load into the solver the combinations needed for this limit state.'''
    loadCombinations.clear()
    combContainer.SLS.freq.createCombinations(loadCombinations)
    return loadCombinations
  def check(self,sections,preprocessor,analysis,mapSectionsForEveryElement):
    intForcCombFileName= self.getInternalForcesFileName()
    out= self.getOutputDataBaseFileName()
    return sections.crackControl(preprocessor,analysis,intForcCombFileName,out,mapSectionsForEveryElement, "k")

class QPLoadsCrackControlRCLimitStateData(LimitStateData):
  ''' Reinforced concrete crack control under quasi-permanent loads limit state data.'''
  def __init__(self):
    '''Limit state data constructor '''
    super(QPLoadsCrackControlRCLimitStateData,self).__init__('quasiPermanentLoadsLoadsCrackControl','esf_els_qp.csv','results_verif_fis_els_qp')
  def createCombinations(self,combContainer,loadCombinations):
    '''Load into the solver the combinations needed for this limit state.'''
    loadCombinations.clear()
    combContainer.SLS.qp.createCombinations(loadCombinations)
    return loadCombinations
  def check(self,sections,preprocessor,analysis,mapSectionsForEveryElement):
    intForcCombFileName= self.getInternalForcesFileName()
    out= self.getOutputDataBaseFileName()
    return sections.crackControl(preprocessor,analysis,intForcCombFileName,out,mapSectionsForEveryElement, "k")


class FatigueResistanceRCLimitStateData(LimitStateData):
  ''' Reinforced concrete shear resistance limit state data.'''
  def __init__(self):
    '''Limit state data constructor '''
    super(FatigueResistanceRCLimitStateData,self).__init__('fatigueResistance','../results/internalForces/esf_elu_fatigue.csv','results_verif_fatigue')
  def createCombinations(self,combContainer,loadCombinations):
    '''Load into the solver the combinations needed for this limit state.'''
    loadCombinations.clear()
    combContainer.ULS.fatigue.createCombinations(loadCombinations)
    return loadCombinations


freqLoadsCrackControl= FreqLoadsCrackControlRCLimitStateData()
quasiPermanentLoadsCrackControl= QPLoadsCrackControlRCLimitStateData()
normalStressesResistance= NormalStressesRCLimitStateData()
shearResistance= ShearResistanceRCLimitStateData()
fatigueResistance= FatigueResistanceRCLimitStateData()




