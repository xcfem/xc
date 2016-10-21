# -*- coding: utf-8 -*-
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import xc_base
import geom
import xc
# Macros
from solution import predefined_solutions
from postprocess import PhantomModel as phm

# TO ENHANCE: Interactions diagrams ("d" and "k") are calculated each time we call
# the check routines. Maybe it's a good idea to calculate them once and write them
# in a file to use them as needed.

class SectionContainer(object):
  sections= []
  mapSections= {}

  def append(self, deckSections):
    self.sections.append(deckSections)
    for i in range(len(deckSections.lstRCSects)):
      self.mapSections[deckSections.lstRCSects[i].sectionName]= deckSections.lstRCSects[i]
    return
  
  def search(self,nmb):
    ''' Returnrs section named nmb (if founded) '''
    retval= None
    for s in self.sections:
      if(s.name==nmb):
        retval= s
    return retval


  def getInteractionDiagrams(self,preprocessor,matDiagType):
    '''Returns 3D interaction diagrams.

    :param preprocessor:    FEA problem preprocessor
    '''
    mapInteractionDiagrams= {}
    for s in self.sections:
      for i in range(len(s.lstRCSects)):
        s.lstRCSects[i].defRCSimpleSection(preprocessor,matDiagType)
        diag= s.lstRCSects[i].defInteractionDiagram(preprocessor)
        mapInteractionDiagrams[s.lstRCSects[i].sectionName]= diag
    return mapInteractionDiagrams

  def getInteractionDiagramsNMy(self,preprocessor,matDiagType):
    '''Returns 2D interaction diagrams in N-My plane.

    :param preprocessor:    FEA problem preprocessor
    '''
    mapInteractionDiagrams= {}
    for s in self.sections:
      diag2= s.lstRCSects[1].defInteractionDiagramNMy(preprocessor,matDiagType)
      diag2.simplify() #Hasta corregir la obtención de diagramas NMy
      print "area diag2= ", diag2.getArea()
      mapInteractionDiagrams[s.lstRCSects[1].sectionName]= diag2
      diag1= s.lstRCSects[0].defInteractionDiagramNMy(preprocessor,matDiagType)
      diag1.simplify() #Hasta corregir la obtención de diagramas NMy
      print "area diag1= ", diag1.getArea()
      mapInteractionDiagrams[s.lstRCSects[0].sectionName]= diag1
    return mapInteractionDiagrams

  def crackControl(self,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, matDiagType,controller):
    '''

    :param intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    :param outputFileName:  name of the output file containing the results of the 
                     verification 
    :param sectionsNamesForEveryElement: file containing a dictionary  such that for each 
                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    :param controller: object that controls crack limit state.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    phantomModel= phm.PhantomModel(preprocessor,sectionsNamesForEveryElement,self.mapSections, mapID)
    retval= phantomModel.runChecking(intForcCombFileName,analysis,controller,outputFileName)
    tmp.clearAll()
    return retval

  def verifyNormalStresses(self,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, matDiagType,controller):
    '''

    :param intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    :param outputFileName:  name of the output file containing the results of the 
                     verification 
    :param sectionsNamesForEveryElement: file containing a dictionary  such that for each
                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    :param controller: object that controls normal stresses limit state on elements.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    phantomModel= phm.PhantomModel(preprocessor,sectionsNamesForEveryElement,self.mapSections, mapID)
    retval= phantomModel.runChecking(intForcCombFileName,analysis,controller,outputFileName)
    tmp.clearAll()
    return retval

  def verifyNormalStresses2d(self,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, matDiagType,controller):
    '''

    :param intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    :param outputFileName:  name of the output file containing the results of the 
                     verification 
    :param sectionsNamesForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    :param controller: object that controls normal stresses limit state on elements.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    mapID= self.getInteractionDiagramsNMy(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    phantomModel= phm.PhantomModel(preprocessor,sectionsNamesForEveryElement,self.mapSections, mapID)
    retval= phantomModel.runChecking(intForcCombFileName,analysis,controller,outputFileName)
    tmp.clearAll()
    return retval

  def shearVerification(self,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, matDiagType,controller):
    '''

    :param intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    :param outputFileName:  name of the output file containing the results of the 
                     verification 
    :param sectionsNamesForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    :param controller: object that controls shear limit state.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    phantomModel= phm.PhantomModel(preprocessor,sectionsNamesForEveryElement,self.mapSections, mapID)
    retval= phantomModel.runChecking(intForcCombFileName,analysis,controller,outputFileName)
    tmp.clearAll()
    return retval

  def fatigueVerification(self,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, matDiagType,controller):
    '''

    :param intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    :param outputFileName:  name of the output file containing the results of the 
                     verification 
    :param sectionsNamesForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    :param controller: object that controls fatigue limit state
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    phantomModel= phm.PhantomModel(preprocessor,sectionsNamesForEveryElement,self.mapSections, mapID)
    retval= phantomModel.runChecking(intForcCombFileName,analysis,controller,outputFileName)
    tmp.clearAll()
    return retval
