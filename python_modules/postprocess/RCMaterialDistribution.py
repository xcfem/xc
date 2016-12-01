# -*- coding: utf-8 -*-

import ElementSectionMap
import pickle
from miscUtils import LogMessages as lmsg
import xc_base
import geom
import xc
# Macros
from solution import predefined_solutions
from postprocess import PhantomModel as phm
from materials.xLamina import RCsectionsContainer as sc

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
    self.sectionDistribution= ElementSectionMap.ElementSectionMap()

  def assign(self,elemSet,setRCSects):
    '''Assigns the sections names: setRCSectsName+'1', setRCSectsName+'2', ...
       to the elements of the set.

       :param elemSet: set of elements that receive the section name property.
       :param setRCSects: RC section definition, name, concrete type,
                          rebar positions,...
    '''
    self.sectionDistribution.assign(elemSet,setRCSects)

  def getSectionNamesForElement(self,tagElem):
    '''Returns the section names for the element which tag is being passed
       as a parameter.'''
    return self.sectionDistribution[tagElem]

  def getSectionDefinition(self,sectionName):
    '''Returns the section definition which has the name being passed
       as a parameter.''' 
    return self.sectionDefinition[sectionName]

  def getSectionDefinitionsForElement(self,tagElem):
    '''Returns the section names for the element which tag is being passed
       as a parameter.'''
    retval= []
    sectionNames= self.getSectionNamesForElement(tagElem)
    for s in sectionNames:
      retval.append(self.sectionDefinition.mapSections[s])
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

  def verifyNormalStresses(self,intForcCombFileName,outputFileName, matDiagType,controller):
    '''

    :param intForcCombFileName: name of the file containing the forces
                                and bending moments obtained for each 
                                element for the combinations analyzed
    :param outputFileName:  name of the output file containing the results 
                            of the verification 
    :param controller: object that controls normal stresses limit state 
                       on elements.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    self.sectionDefinition.calcInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    phantomModel= phm.PhantomModel(preprocessor,self)
    retval= phantomModel.runChecking(intForcCombFileName,analysis,controller,outputFileName)
    tmp.clearAll()
    return retval


  def verifyNormalStresses2d(self,intForcCombFileName,outputFileName, matDiagType,controller):
    '''

    :param intForcCombFileName: name of the file containing the forces
                                and bending moments obtained for each 
                                element for the combinations analyzed
    :param outputFileName:  name of the output file containing the results 
                            of the verification 
    :param controller: object that controls normal stresses limit state 
                       on elements.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    self.sectionDefinition.calcInteractionDiagrams(preprocessor,matDiagType,'NMy')
    analysis= predefined_solutions.simple_static_linear(tmp)
    phantomModel= phm.PhantomModel(preprocessor,self)
    retval= phantomModel.runChecking(intForcCombFileName,analysis,controller,outputFileName)
    tmp.clearAll()
    return retval

  def shearVerification(self,intForcCombFileName,outputFileName, matDiagType,controller):
    '''

    :param intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    :param outputFileName:  name of the output file containing the results 
                            of the verification 
    :param controller: object that controls shear limit state.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    self.sectionDefinition.calcInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    phantomModel= phm.PhantomModel(preprocessor,self)
    retval= phantomModel.runChecking(intForcCombFileName,analysis,controller,outputFileName)
    tmp.clearAll()
    return retval


  def crackControl(self,intForcCombFileName,outputFileName,matDiagType,controller):
    '''

    :param intForcCombFileName: name of the file containing the forces and 
                                bending moments obtained for each element for
                                the combinations analyzed
    :param outputFileName:  name of the output file containing the results 
                            of the verification 
    :param controller: object that controls crack limit state.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    self.sectionDefinition.calcInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    phantomModel= phm.PhantomModel(preprocessor,self)
    retval= phantomModel.runChecking(intForcCombFileName,analysis,controller,outputFileName)
    tmp.clearAll()
    return retval
  
  def fatigueVerification(self,intForcCombFileName,outputFileName, matDiagType,controller):
    '''

    :param intForcCombFileName: name of the file containing the forces and
                                bending moments. 
                     obtained for each element for the combinations analyzed
    :param outputFileName:  name of the output file containing the results of
                            the verification. 
    :param controller: object that controls fatigue limit state
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    self.sectionDefinition.calcInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    phantomModel= phm.PhantomModel(preprocessor,self)
    retval= phantomModel.runChecking(intForcCombFileName,analysis,controller,outputFileName)
    tmp.clearAll()
    return retval

def loadRCMaterialDistribution():
  '''Load the reinforced concrete sections on each element from file.'''
  with open(RCMaterialDistribution.mapSectionsFileName, 'rb') as f:
    return pickle.load(f)

