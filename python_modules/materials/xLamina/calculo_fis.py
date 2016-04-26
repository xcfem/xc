# -*- coding: utf-8 -*-
# Comprobación de secciones de hormigón frente a fisuración.

from materials.ehe import fisuracionEHE
from materials.sia262 import crackControlSIA262 as ccSIA
from materials.xLamina import extrae_combinaciones as ec
from materials.xLamina import calculo_comb
import os
from postprocess.reports import common_formats as fmt


def lanzaCalculoFIS(nmbArch, nmbRegDatosScc1, nmbRegDatosScc2, nmbArchDefHipELS):
  '''
   Lanza la comprobación de fisuración en una lámina
      cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
      con los materiales que se definen en el archivo nmbArchMateriales,
      las características de las secciones que se definen en los registros
      datosScc1 y datosScc2, las combinaciones definidas en el archivo
      nmbArchDefHipELS e imprime los resultados en archivos con
      el nombre nmbArchFis.*
  '''
  extraeDatosLST(nmbArch+".lst")
  xLaminaConstruyeModeloFibras(nmbRegDatosScc1,nmbRegDatosScc2)
  xLaminaCalculaCombEstatNoLin(nmbArchDefHipELS)
  xLaminaPrintFIS(nmbArch+"FIS",deref(nmbRegDatosScc1).sectionName,deref(nmbRegDatosScc2).sectionName)


def lanzaCalculoFISFromXCData(preprocessor,analysis,intForcCombFileName,outputFileName, sectionsNamesForEveryElement,controller):
  '''
   Lanza la comprobación de fisuración en una lámina
      cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
      con los materiales que se definen en el archivo nmbArchMateriales,
      las características de las secciones que se definen en mapSections,
      e imprime los resultados en archivos con
      el nombre nmbArchFis.*
  Parameters:
    preprocessor:    preprocessor name
    analysis:        type of analysis
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing tue results of the 
                     verification 
    sectionsNamesForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    controller: object that controls limit state on elements      
  '''
  meanCFs= -1.0
  if(controller):
    elems= ec.creaElems(preprocessor,intForcCombFileName, sectionsNamesForEveryElement)
    ccSIA.defVarsControlFISSIA262(elems)
    elements= preprocessor.getSets.getSet("total").getElements
    calculo_comb.xLaminaCalculaComb(preprocessor,elements,analysis,controller)
    meanCFs= cv.writeControlVarsFromElements(controller.limitStateLabel,preprocessor,outputFileName,sectionsNamesForEveryElement)
  else:
    lmsg.error('lanzaCalculoFISFromXCData controller not defined.')
  return meanCFs

def lanzaCalculoFISFromXCDataPlanB(preprocessor,analysis,intForcCombFileName,outputFileName, sectionsNamesForEveryElement,mapSectionsDefinition,controller):
  '''
   Lanza la comprobación de fisuración en una lámina
      cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
      con los materiales que se definen en el archivo nmbArchMateriales,
      las características de las secciones que se definen en mapSections,
      e imprime los resultados en archivos con
      el nombre nmbArchFis.*
  Parameters:
    preprocessor:    preprocessor name
    analysis:        type of analysis
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    outputFileName:  name of the output file containing tue results of the 
                     verification 
    sectionsNamesForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    controller: object that controls limit state on elements      
  '''
  if(controller):
    elems= ec.extraeDatos(preprocessor,intForcCombFileName, sectionsNamesForEveryElement,mapSectionsDefinition, None,controller.limitStateLabel)
    elements= preprocessor.getSets.getSet("total").getElements
    calculo_comb.xLaminaCalculaComb(elements,analysis,controller)
    cv.writeControlVarsFromElements(controller.limitStateLabel,preprocessor,outputFileName,sectionsNamesForEveryElement)
  else:
    lmsg.error('lanzaCalculoFISFromXCDataPlanB controller not defined.')
  return meanCFs


