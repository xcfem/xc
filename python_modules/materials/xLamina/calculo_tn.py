# -*- coding: utf-8 -*-
# Comprobación de secciones de hormigón frente a tensiones normales.

import os
import math
from miscUtils import LogMessages as lmsg
from materials.xLamina import extrae_combinaciones as ec
from materials.xLamina import modelo
from materials.xLamina import calculo_comb
from postprocess import ControlVars as cv


# Ejecuta el análisis y la comprobación frente a tensiones normales
def xLaminaCompruebaTNComb(preprocessor, nmbDiagIntSec1, nmbDiagIntSec2,controller):
  '''
  Parameters:
    preprocessor:    preprocessor name
    controller: object that control normal stress limit state.
  '''
  listaCombinaciones= []
  cargas= preprocessor.getLoadLoader
  casos= cargas.getLoadPatterns
  ts= casos.newTimeSeries("constant_ts","ts")
  casos.currentTimeSeries= "ts"
  execfile("/tmp/cargas.xci")
  listaCombinaciones= cargas.listaNombresLoadPatterns()

  for comb in listaCombinaciones:
    print("Resolviendo para acción: ",listaCombinaciones[i],"\n")
    resuelveCombEstatLin(comb)
    controller.check(comb,nmbDiagIntSec1,nmbDiagIntSec2)

  os.system("rm -f "+"/tmp/acciones.xci")
  os.system("rm -f "+"/tmp/cargas.xci")
  xLaminaPrintTN(nmbArch) # XXX Sacar de aquí la impresión de result.




'''
 Lanza la comprobación de tensiones normales en una lámina
    cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
    con los materiales que se definen en el archivo nmbArchMateriales,
    las características de las secciones que se definen en los registros
    datosScc1 y datosScc2, las combinaciones definidas en el archivo
    nmbArchDefHipELU e imprime los resultados en archivos con
    el nombre nmbArchTN.*
'''
def lanzaCalculoTNFromAnsysData(nmbArch, datosScc1, datosScc2, nmbArchDefHipELU):
  extraeDatosLST(nmbArch+".lst") 
  xLaminaConstruyeModeloFicticio(datosScc1,datosScc2)
  nmbDiagIntSec1= "diagInt"+datosScc1.sectionName
  nmbDiagIntSec2= "diagInt"+datosScc2.sectionName
  xLaminaCalculaCombEstatLin(nmbArchDefHipELU,nmbDiagIntSec1,nmbDiagIntSec2)
  meanCFs= cv.writeControlVarsFromElements("ULS_normStr",preprocessor,nmbArch+"TN",datosScc1.sectionName,datosScc2.sectionName)
  return meanCFs

def lanzaCalculoTNFromXCData(preprocessor,analysis,intForcCombFileName,outputFileName, sectionsNamesForEveryElement,mapSectionsDefinition, mapInteractionDiagrams,controller):
  '''
  Lanza la comprobación de tensiones normales en una lámina
    cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
    con los materiales que se definen en el archivo nmbArchMateriales,
    las características de las secciones que se definen en los registros
    datosScc1 y datosScc2, las combinaciones definidas en el archivo
    nmbArchDefHipELU e imprime los resultados en archivos con
    el nombre nmbArchTN.*
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
    mapSectionsDefinition:      file containing a dictionary with the two 
                                sections associated with each elements to be
                                used in the verification
    mapInteractionDiagrams:     file containing a dictionary such that                                                      associates each element with the two interactions
                                diagrams of materials to be used in the verification process
  '''
  meanCFs= -1.0
  if(controller):
    ec.extraeDatos(preprocessor,intForcCombFileName, sectionsNamesForEveryElement,mapSectionsDefinition, mapInteractionDiagrams,controller.limitStateLabel)
    elements= preprocessor.getSets.getSet("total").getElements
    calculo_comb.xLaminaCalculaCombEstatLin(preprocessor,elements,analysis,controller)
    meanCFs= cv.writeControlVarsFromElements(controller.limitStateLabel,preprocessor,outputFileName)
  else:
    lmsg.error('lanzaCalculoTNFromXCData controller not defined.')
  return meanCFs

def lanzaCalculoTN2dFromXCData(preprocessor,analysis,intForcCombFileName,outputFileName, sectionsNamesForEveryElement,mapSectionsDefinition, mapInteractionDiagrams,controller):
  '''
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
    mapSectionsDefinition:      file containing a dictionary with the two 
                                sections associated with each elements to be
                                used in the verification
    mapInteractionDiagrams:     file containing a dictionary such that                                                      associates each element with the two interactions
                                diagrams of materials to be used in the verification process
  '''
  lmsg.warning('lanzaCalculoTN2dFromXCData DEPRECATED call lanzaCalculoTNFromXCData with an uniaxial bending controller.')
  meanCFs= -1.0
  if(controller):
    ec.extraeDatos(preprocessor,intForcCombFileName, sectionsNamesForEveryElement,mapSectionsDefinition, mapInteractionDiagrams,controller.limitStateLabel)
    elements= preprocessor.getSets.getSet("total").getElements
    calculo_comb.xLaminaCalculaCombEstatLin(preprocessor,elements,analysis,controller)
    meanCFs= cv.writeControlVarsFromElements(controller.limitStateLabel,preprocessor,outputFileName)
  else:
    lmsg.error('lanzaCalculoTNFromXCData controller not defined.')
  return meanCFs


