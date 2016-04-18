# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
# Macros
from materials.xLamina import calculo_fis
from materials.xLamina import calculo_tn
from materials.xLamina import calculo_v
from materials.xLamina import calculo_fatigue
from materials.sia262 import fatigueControlSIA262 as fc
from materials.sia262 import shearSIA262
from materials.sia262 import crackControlSIA262 as cc


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
      preprocessor:    preprocessor name
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
      preprocessor:    preprocessor name
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

  def crackControl(self,preprocessor,analysis,intForcCombFileName,outputFileName,mapSectionsForEveryElement, matDiagType):
    '''
    Parameters:
      preprocessor:    preprocessor name
    analysis:        type of analysis
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing the results of the 
                     verification 
    mapSectionsForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    '''
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    return calculo_fis.lanzaCalculoFISFromXCDataPlanB(preprocessor,analysis,intForcCombFileName,outputFileName,mapSectionsForEveryElement,self.mapSections, cc.procesResultVerifFISSIA262PlanB)

  def verifyNormalStresses(self,preprocessor,analysis,intForcCombFileName,outputFileName,mapSectionsForEveryElement, matDiagType):
    '''
    Parameters:
      preprocessor:    preprocessor name
    analysis:        type of analysis
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing the results of the 
                     verification 
    mapSectionsForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    '''
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    return calculo_tn.lanzaCalculoTNFromXCData(preprocessor,analysis,intForcCombFileName,outputFileName,mapSectionsForEveryElement, self.mapSections, mapID)

  def verifyNormalStresses2d(self,preprocessor,analysis,intForcCombFileName,outputFileName,mapSectionsForEveryElement, matDiagType):
    '''
    Parameters:
      preprocessor:    preprocessor name
    analysis:        type of analysis
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing the results of the 
                     verification 
    mapSectionsForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    '''
    mapID= self.getInteractionDiagramsNMy(preprocessor,matDiagType)
    return calculo_tn.lanzaCalculoTN2dFromXCData(preprocessor,analysis,intForcCombFileName,outputFileName,mapSectionsForEveryElement, self.mapSections, mapID)

  def shearVerification(self,preprocessor,analysis,intForcCombFileName,outputFileName,mapSectionsForEveryElement, matDiagType):
    '''
    Parameters:
      preprocessor:    preprocessor name
    analysis:        type of analysis
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing the results of the 
                     verification 
    mapSectionsForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    '''
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    return calculo_v.lanzaCalculoV(preprocessor,analysis,intForcCombFileName,outputFileName,mapSectionsForEveryElement,self.mapSections, mapID, shearSIA262.procesResultVerifV)

  def fatigueVerification(self,preprocessor,analysis,intForcCombFileName,outputFileName,mapSectionsForEveryElement, matDiagType):
    '''
    Parameters:
      preprocessor:    preprocessor name
    analysis:        type of analysis
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing the results of the 
                     verification 
    mapSectionsForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    '''
    mapID= self.getInteractionDiagrams(preprocessor,matDiagType)
    return calculo_fatigue.lanzaCalculoFatigueFromXCDataPlanB(preprocessor,analysis,intForcCombFileName,outputFileName,mapSectionsForEveryElement,self.mapSections, mapID,fc.procesResultVerif)
