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
    self.mapSections[deckSections.D2Section.nmbSeccion]= deckSections.D2Section
    self.mapSections[deckSections.D1Section.nmbSeccion]= deckSections.D1Section

  def search(self,nmb):
    ''' Returnrs section named nmb (if founded) '''
    retval= None
    for s in self.sections:
      if(s.name==nmb):
        retval= s
    return retval

  # def getInteractionDiagrams(self,mdlr,tipoDiag):
  #   mapInteractionDiagrams= {}
  #   for s in self.sections:
  #     diag2= s.D2Section.defInteractionDiagram(mdlr,tipoDiag)
  #     mapInteractionDiagrams[s.D2Section.nmbSeccion]= diag2
  #     diag1= s.D1Section.defInteractionDiagram(mdlr,tipoDiag)
  #     mapInteractionDiagrams[s.D1Section.nmbSeccion]= diag1
  #   return mapInteractionDiagrams

  def getInteractionDiagrams(self,mdlr,tipoDiag):
    '''Returns 3D interaction diagrams.'''
    mapInteractionDiagrams= {}
    for s in self.sections:
      diag2= s.D2Section.defInteractionDiagram(mdlr,tipoDiag)
      mapInteractionDiagrams[s.D2Section.nmbSeccion]= diag2
      diag1= s.D1Section.defInteractionDiagram(mdlr,tipoDiag)
      mapInteractionDiagrams[s.D1Section.nmbSeccion]= diag1
    return mapInteractionDiagrams

  def getInteractionDiagramsNMy(self,mdlr,tipoDiag):
    '''Returns 2D interaction diagrams in N-My plane.'''
    mapInteractionDiagrams= {}
    for s in self.sections:
      diag2= s.D2Section.defInteractionDiagramNMy(mdlr,tipoDiag)
      diag2.simplify() #Hasta corregir la obtención de diagramas NMy
      print "area diag2= ", diag2.getArea()
      mapInteractionDiagrams[s.D2Section.nmbSeccion]= diag2
      diag1= s.D1Section.defInteractionDiagramNMy(mdlr,tipoDiag)
      diag1.simplify() #Hasta corregir la obtención de diagramas NMy
      print "area diag1= ", diag1.getArea()
      mapInteractionDiagrams[s.D1Section.nmbSeccion]= diag1
    return mapInteractionDiagrams

  def crackControl(self,mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(mdlr,tipoDiag)
    return calculo_fis.lanzaCalculoFISFromXCDataPlanB(mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement,self.mapSections, cc.trataResultsCombFISSIA262PlanB)

  def verifyNormalStresses(self,mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(mdlr,tipoDiag)
    return calculo_tn.lanzaCalculoTNFromXCData(mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, self.mapSections, mapID)

  def verifyNormalStresses2d(self,mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagramsNMy(mdlr,tipoDiag)
    return calculo_tn.lanzaCalculoTN2dFromXCData(mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, self.mapSections, mapID)

  def shearVerification(self,mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(mdlr,tipoDiag)
    return calculo_v.lanzaCalculoV(mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement,self.mapSections, mapID, shearSIA262.trataResultsCombV)

  def fatigueVerification(self,mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(mdlr,tipoDiag)
    return calculo_fatigue.lanzaCalculoFatigueFromXCDataPlanB(mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement,self.mapSections, mapID,fc.trataResultsComb)
