# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

import element_section_map
import pickle
from miscUtils import LogMessages as lmsg
import xc_base
import geom
import xc
# Macros
from solution import predefined_solutions
from postprocess import phantom_model as phm
from materials.sections import RCsectionsContainer as sc
from model.sets import sets_mng as sUtils
from postprocess import limit_state_data as lsd

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2016, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

class RCMaterialDistribution(object):
  '''Spatial distribution of reinforced concrete material 
     (RC sections distribution over the elements).
  It refers to the reinforced concrete sections associated with the element
  (i.e. for shell elements we typically define two RC sections, one for each
  main direction; in the case of beam elements the most common way is to define
  RC sections in the front and back ends of the elements)
  '''
  mapSectionsFileName= './mapSectionsReinforcement.pkl'
  
  def __init__(self):
    '''Constructor.

    '''
    # sectionDefinition: Container with the section definitions 
    #                   (see RCsectionsContainer module).
    self.sectionDefinition= sc.SectionContainer()
    # sectionDistribution: dictionary that stores a section name(s)
    #                      for each element number. This way it defines
    #                      a spatial distribution of the sections over
    #                      the structure.
    self.sectionDistribution= element_section_map.ElementSectionMap()
    self.elementSetNames= list() #Elements sets with an assigned section.

  def assign(self,elemSet,setRCSects):
    '''Assigns the sections names: setRCSectsName+'1', setRCSectsName+'2', ...
       to the elements of the set.

       :param elemSet: set of elements that receive the section name property.
       :param setRCSects: RC section definition, name, concrete type,
                          rebar positions,...
    '''
    self.sectionDistribution.assign(elemSet,setRCSects)
    self.elementSetNames.append(elemSet.owner.name)

  def getElementSet(self,preprocessor):
    '''Returns an XC set that contains all the elements with an
       assigned section.'''
    retvalName= ''
    for name in self.elementSetNames:
      retvalName+= '|'+name
    retval= preprocessor.getSets.defSet(retvalName)
    sets= list()
    for name in self.elementSetNames:
      sets.append(preprocessor.getSets.getSet(name))
    sUtils.append_sets(retval,sets)
    return retval

  def getSectionNamesForElement(self,tagElem):
    '''Returns the section names for the element which tag is being passed
       as a parameter.'''
    if tagElem in self.sectionDistribution.keys():
      return self.sectionDistribution[tagElem]
    else:
      lmsg.error("RCMaterialDistribution::getSectionNamesForElement; element with tag: "+str(tagElem)+" not found.")
      return None

  def getSectionDefinition(self,sectionName):
    '''Returns the section definition which has the name being passed
       as a parameter.''' 
    return self.sectionDefinition[sectionName]

  def getSectionDefinitionsForElement(self,tagElem):
    '''Returns the section names for the element which tag is being passed
       as a parameter.'''
    retval= []
    sectionNames= self.getSectionNamesForElement(tagElem)
    if(sectionNames):
      for s in sectionNames:
        retval.append(self.sectionDefinition.mapSections[s])
    else:
      lmsg.error("section names for element with tag: "+str(tagElem)+" not found.")
    return retval
  
  def dump(self):
    '''Writes this object in a pickle file.'''
    with open(self.mapSectionsFileName, 'wb') as f:
      pickle.dump(self, f, pickle.HIGHEST_PROTOCOL)

  def load(self):
    '''Reads this object from a pickle file.'''
    with open(self.mapSectionsFileName, 'rb') as f:
      self.sectionDistribution= pickle.load(f)
      self.sectionDefinition= pickle.load(f)
    f.close()

  def runChecking(self,limitStateData,matDiagType,threeDim= True,outputCfg= lsd.VerifOutVars()):
    '''Creates the phantom model and runs the verification on it.

    :param limitStateData: object that contains the name of the file
                           containing the internal forces 
                           obtained for each element 
                           for the combinations analyzed and the
                           controller to use for the checking.
    :param matDiagType: type of the material diagram (d: design, 
           k: characteristic).
    :param threeDim: true if it's 3D (Fx,Fy,Fz,Mx,My,Mz) 
           false if it's 2D (Fx,Fy,Mz).
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to a file,
               generation or not of lists, ...)
    '''
    feProblem= xc.FEProblem()
    preprocessor= feProblem.getPreprocessor
    if 'straight' in str(limitStateData.controller).lower():
       for s in self.sectionDefinition.sections:
         s.concrType.initTensStiff='Y'
    self.sectionDefinition.createRCsections(preprocessor,matDiagType) #creates
                      #for each element in the container the fiber sections
                      #(RCsimpleSections) associated with it.
    if(threeDim):
      self.sectionDefinition.calcInteractionDiagrams(preprocessor,matDiagType)
    else:
      self.sectionDefinition.calcInteractionDiagrams(preprocessor,matDiagType,'NMy')
    limitStateData.controller.analysis= limitStateData.controller.analysisToPerform(feProblem)
    phantomModel= phm.PhantomModel(preprocessor,self)
    result= phantomModel.runChecking(limitStateData,outputCfg)
    return (feProblem, result)

  def internalForcesVerification3D(self,limitStateData,matDiagType,outputCfg):
    '''Limit state verification based on internal force (Fx,Fy,Fz,Mx,My,Mz) values.

    :param limitStateData: object that contains the name of the file
                           containing the internal forces 
                           obtained for each element 
                           for the combinations analyzed and the
                           controller to use for the checking.
    :param matDiagType: type of the material diagram (d: design, k: characteristic).
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to a file,
               generation or not of lists, ...)
    '''
    (tmp, retval)= self.runChecking(limitStateData, matDiagType,True,outputCfg)
    tmp.clearAll() #Free memory.
    return retval

  def internalForcesVerification2D(self,limitStateData, matDiagType,setCalc=None):
    '''Limit state verification based on internal force (Fx,Fy,Mz) values.

    :param limitStateData: object that contains the name of the file
                           containing the internal forces 
                           obtained for each element 
                           for the combinations analyzed and the
                           controller to use for the checking.
    :param matDiagType: type of the material diagram (d: design, 
           k: characteristic).
    :param setCalc: set of elements to be analyzed (defaults to None which 
                    means that all the elements in the file of internal forces
                    results are analyzed) 
    '''
    (tmp, retval)= self.runChecking(limitStateData,outputFileName, matDiagType,False,setCalc)
    tmp.clearAll() #Free memory.
    return retval


def loadRCMaterialDistribution():
  '''Load the reinforced concrete sections on each element from file.'''
  with open(RCMaterialDistribution.mapSectionsFileName, 'rb') as f:
    return pickle.load(f)

