# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
# Macros
from materials.xLamina import calcsLauncher
#from materials.sia262 import fatigueControlSIA262 as fc
#from materials.sia262 import shearSIA262
#from materials.sia262 import crackControlSIA262 as cc
from solution import predefined_solutions

# TO ENHANCE: Interactions diagrams ("d" and "k") are calculated each time we call
# the check routines. Maybe it's a good idea to calculate them once and write them
# in a file to use them as needed.

class SectionContainer(object):
  sections= []
  mapSections= {}

  def append(self, deckSections):
    self.sections.append(deckSections)
    self.mapSections[deckSections.D2Section.sectionName]= deckSections.D2Section
    self.mapSections[deckSections.D1Section.sectionName]= deckSections.D1Section

  def search(self,nmb):
    ''' Returnrs section named nmb (if founded) '''
    retval= None
    for s in self.sections:
      if(s.name==nmb):
        retval= s
    return retval

  # def getInteractionDiagrams(self,preprocessor,matDiagType):
  #   mapInteractionDiagrams= {}
  #   for s in self.sections:
  #     diag2= s.D2Section.defInteractionDiagram(preprocessor,matDiagType)
  #     mapInteractionDiagrams[s.D2Section.sectionName]= diag2
  #     diag1= s.D1Section.defInteractionDiagram(preprocessor,matDiagType)
  #     mapInteractionDiagrams[s.D1Section.sectionName]= diag1
  #   return mapInteractionDiagrams

  def getInteractionDiagrams(self,preprocessor,matDiagType):
    '''Returns 3D interaction diagrams.
    Parameters:
      preprocessor:    FEA problem preprocessor
    '''
    mapInteractionDiagrams= {}
    for s in self.sections:
      s.D2Section.defRCSimpleSection(preprocessor,matDiagType)
      diag2= s.D2Section.defInteractionDiagram(preprocessor)
      mapInteractionDiagrams[s.D2Section.sectionName]= diag2
      s.D1Section.defRCSimpleSection(preprocessor,matDiagType)
      diag1= s.D1Section.defInteractionDiagram(preprocessor)
      mapInteractionDiagrams[s.D1Section.sectionName]= diag1
    return mapInteractionDiagrams

  def getInteractionDiagramsNMy(self,preprocessor,matDiagType):
    '''Returns 2D interaction diagrams in N-My plane.
    Parameters:
      preprocessor:    FEA problem preprocessor
    '''
    mapInteractionDiagrams= {}
    for s in self.sections:
      diag2= s.D2Section.defInteractionDiagramNMy(preprocessor,matDiagType)
      diag2.simplify() #Hasta corregir la obtención de diagramas NMy
      print "area diag2= ", diag2.getArea()
      mapInteractionDiagrams[s.D2Section.sectionName]= diag2
      diag1= s.D1Section.defInteractionDiagramNMy(preprocessor,matDiagType)
      diag1.simplify() #Hasta corregir la obtención de diagramas NMy
      print "area diag1= ", diag1.getArea()
      mapInteractionDiagrams[s.D1Section.sectionName]= diag1
    return mapInteractionDiagrams

  def crackControl(self,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, matDiagType,controller):
    '''
    Parameters:
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing the results of the 
                     verification 
    sectionsNamesForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    controller: object that controls crack limit state.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    retval= calcsLauncher.lanzaCalculoFISFromXCDataPlanB(preprocessor,analysis,intForcCombFileName,outputFileName,sectionsNamesForEveryElement,self.mapSections, controller)
    tmp.clearAll()
    return retval

  def verifyNormalStresses(self,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, matDiagType,controller):
    '''
    Parameters:
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing the results of the 
                     verification 
    sectionsNamesForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    controller: object that controls normal stresses limit state on elements.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    retval= calcsLauncher.lanzaCalculoTNFromXCData(preprocessor,analysis,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, self.mapSections, mapID,controller)
    tmp.clearAll()
    return retval

  def verifyNormalStresses2d(self,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, matDiagType,controller):
    '''
    Parameters:
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing the results of the 
                     verification 
    sectionsNamesForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    controller: object that controls normal stresses limit state on elements.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    mapID= self.getInteractionDiagramsNMy(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    retval= calcsLauncher.lanzaCalculoTN2dFromXCData(preprocessor,analysis,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, self.mapSections, mapID,controller)
    tmp.clearAll()
    return retval

  def shearVerification(self,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, matDiagType,controller):
    '''
    Parameters:
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing the results of the 
                     verification 
    sectionsNamesForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    controller: object that controls shear limit state.
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    retval= calcsLauncher.lanzaCalculoV(preprocessor,analysis,intForcCombFileName,outputFileName,sectionsNamesForEveryElement,self.mapSections, mapID, controller)
    tmp.clearAll()
    return retval

  def fatigueVerification(self,intForcCombFileName,outputFileName,sectionsNamesForEveryElement, matDiagType,controller):
    '''
    Parameters:
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing the results of the 
                     verification 
    sectionsNamesForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    controller: object that controls fatigue limit state
    '''
    tmp= xc.ProblemaEF()
    preprocessor= tmp.getPreprocessor
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    analysis= predefined_solutions.simple_static_linear(tmp)
    retval= calcsLauncher.lanzaCalculoFatigueFromXCDataPlanB(preprocessor,analysis,intForcCombFileName,outputFileName,sectionsNamesForEveryElement,self.mapSections, mapID,controller)
    tmp.clearAll()
    return retval
