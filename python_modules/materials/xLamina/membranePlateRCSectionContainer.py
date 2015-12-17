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

  # def getInteractionDiagrams(self,preprocessor,tipoDiag):
  #   mapInteractionDiagrams= {}
  #   for s in self.sections:
  #     diag2= s.D2Section.defInteractionDiagram(preprocessor,tipoDiag)
  #     mapInteractionDiagrams[s.D2Section.sectionName]= diag2
  #     diag1= s.D1Section.defInteractionDiagram(preprocessor,tipoDiag)
  #     mapInteractionDiagrams[s.D1Section.sectionName]= diag1
  #   return mapInteractionDiagrams

  def getInteractionDiagrams(self,preprocessor,tipoDiag):
    '''Returns 3D interaction diagrams.'''
    mapInteractionDiagrams= {}
    for s in self.sections:
      s.D2Section.defSeccionHASimple(preprocessor,tipoDiag)
      diag2= s.D2Section.defInteractionDiagram(preprocessor)
      mapInteractionDiagrams[s.D2Section.sectionName]= diag2
      s.D1Section.defSeccionHASimple(preprocessor,tipoDiag)
      diag1= s.D1Section.defInteractionDiagram(preprocessor)
      mapInteractionDiagrams[s.D1Section.sectionName]= diag1
    return mapInteractionDiagrams

  def getInteractionDiagramsNMy(self,preprocessor,tipoDiag):
    '''Returns 2D interaction diagrams in N-My plane.'''
    mapInteractionDiagrams= {}
    for s in self.sections:
      diag2= s.D2Section.defInteractionDiagramNMy(preprocessor,tipoDiag)
      diag2.simplify() #Hasta corregir la obtención de diagramas NMy
      print "area diag2= ", diag2.getArea()
      mapInteractionDiagrams[s.D2Section.sectionName]= diag2
      diag1= s.D1Section.defInteractionDiagramNMy(preprocessor,tipoDiag)
      diag1.simplify() #Hasta corregir la obtención de diagramas NMy
      print "area diag1= ", diag1.getArea()
      mapInteractionDiagrams[s.D1Section.sectionName]= diag1
    return mapInteractionDiagrams

  def crackControl(self,preprocessor,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(preprocessor,tipoDiag)
    return calculo_fis.lanzaCalculoFISFromXCDataPlanB(preprocessor,analysis,csvFile,outputFile,mapSectionsForEveryElement,self.mapSections, cc.trataResultsCombFISSIA262PlanB)

  def verifyNormalStresses(self,preprocessor,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(preprocessor,tipoDiag)
    return calculo_tn.lanzaCalculoTNFromXCData(preprocessor,analysis,csvFile,outputFile,mapSectionsForEveryElement, self.mapSections, mapID)

  def verifyNormalStresses2d(self,preprocessor,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagramsNMy(preprocessor,tipoDiag)
    return calculo_tn.lanzaCalculoTN2dFromXCData(preprocessor,analysis,csvFile,outputFile,mapSectionsForEveryElement, self.mapSections, mapID)

  def shearVerification(self,preprocessor,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(preprocessor,tipoDiag)
    return calculo_v.lanzaCalculoV(preprocessor,analysis,csvFile,outputFile,mapSectionsForEveryElement,self.mapSections, mapID, shearSIA262.trataResultsCombV)

  def fatigueVerification(self,preprocessor,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(preprocessor,tipoDiag)
    return calculo_fatigue.lanzaCalculoFatigueFromXCDataPlanB(preprocessor,analysis,csvFile,outputFile,mapSectionsForEveryElement,self.mapSections, mapID,fc.trataResultsComb)
