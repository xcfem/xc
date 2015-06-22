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
    self.mapSections[deckSections.seccionT.nmbSeccion]= deckSections.seccionT
    self.mapSections[deckSections.seccionL.nmbSeccion]= deckSections.seccionL

  def getInteractionDiagrams(self,mdlr,tipoDiag):
    mapInteractionDiagrams= {}
    for s in self.sections:
      diagT= s.seccionT.defInteractionDiagram(mdlr,tipoDiag)
      mapInteractionDiagrams[s.seccionT.nmbSeccion]= diagT
      diagL= s.seccionL.defInteractionDiagram(mdlr,tipoDiag)
      mapInteractionDiagrams[s.seccionL.nmbSeccion]= diagL
    return mapInteractionDiagrams

  def crackControl(self,mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(mdlr,tipoDiag)
    return calculo_fis.lanzaCalculoFISFromXCDataPlanB(mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement,self.mapSections, cc.trataResultsCombFISSIA262PlanB)

  def verifyNormalStresses(self,mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(mdlr,tipoDiag)
    return calculo_tn.lanzaCalculoTNFromXCData(mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, self.mapSections, mapID)

  def shearVerification(self,mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(mdlr,tipoDiag)
    return calculo_v.lanzaCalculoV(mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement,self.mapSections, mapID, shearSIA262.trataResultsCombV)

  def fatigueVerification(self,mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement, tipoDiag):
    mapID= self.getInteractionDiagrams(mdlr,tipoDiag)
    return calculo_fatigue.lanzaCalculoFatigueFromXCDataPlanB(mdlr,analysis,csvFile,outputFile,mapSectionsForEveryElement,self.mapSections, mapID,fc.trataResultsComb)
