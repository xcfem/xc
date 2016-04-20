# -*- coding: utf-8 -*-
# Comprobación de secciones de hormigón frente a tensiones normales.

import os
import math
from materials.xLamina import extrae_combinaciones as ec
from materials.xLamina import modelo
from materials.xLamina import calculo_comb
import geom
from postprocess import ControlVars as cv


def even(number):
   return number % 2 == 0
def odd(number):
   return number % 2 != 0

# Ejecuta el análisis y la comprobación frente a tensiones normales
def xLaminaCompruebaTNComb(preprocessor, nmbDiagIntSec1, nmbDiagIntSec2):
  '''
  Parameters:
    preprocessor:    preprocessor name
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
    procesResultVerif(comb,nmbDiagIntSec1,nmbDiagIntSec2)

  os.system("rm -f "+"/tmp/acciones.xci")
  os.system("rm -f "+"/tmp/cargas.xci")
  xLaminaPrintTN(nmbArch) # XXX Sacar de aquí la impresión de result.

def procesResultVerifTN(preprocessor, nmbComb):
  '''
  Parameters:
    preprocessor:    preprocessor name
  '''
  #print "Postproceso combinación: ",nmbComb
  elements= preprocessor.getSets["total"].getElements
  for e in elements:
    e.getResistingForce()
    TagTmp= e.tag
    scc= e.getSection()
    idSection= e.getProp("idSection")
    Ntmp= scc.getStressResultantComponent("N")
    MyTmp= scc.getStressResultantComponent("My")
    MzTmp= scc.getStressResultantComponent("Mz")
    posEsf= geom.Pos3d(Ntmp,MyTmp,MzTmp)
    diagInt= e.getProp("diagInt")
    CFtmp= diagInt.getCapacityFactor(posEsf)
    if(CFtmp>e.getProp("ULS_normStr").CF):
      e.setProp("ULS_normStr",cv.BiaxialBendingControlVars(idSection,nmbComb,CFtmp,Ntmp,MyTmp,MzTmp)) # Worst case.

def procesResultVerifTN2d(preprocessor, nmbComb):
  '''
  Parameters:
    preprocessor:    preprocessor name
  '''
  #print "Postproceso combinación: ",nmbComb
  elements= preprocessor.getSets["total"].getElements
  for e in elements:
    e.getResistingForce()
    TagTmp= e.tag
    scc= e.getSection()
    idSection= e.getProp("idSection")
    Ntmp= scc.getStressResultantComponent("N")
    MyTmp= scc.getStressResultantComponent("My")
    posEsf= geom.Pos2d(Ntmp,MyTmp)
    diagInt= e.getProp("diagInt")
    CFtmp= diagInt.getCapacityFactor(posEsf)
    if(CFtmp>e.getProp("ULS_normStr").CF):
      e.setProp("ULS_normStr",cv.BiaxialBendingControlVars(idSection,nmbComb,CFtmp,Ntmp,MyTmp)) # Worst case.



'''
 Lanza la comprobación de tensiones normales en una lámina
    cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
    con los materiales que se definen en el archivo nmbArchMateriales,
    las características de las secciones que se definen en los registros
    datosScc1 y datosScc2, las combinaciones definidas en el archivo
    nmbArchDefHipELU e imprime los resultados en archivos con
    el nombre nmbArchTN.*
'''
def lanzaCalculoTNFromAnnsysData(nmbArch, datosScc1, datosScc2, nmbArchDefHipELU):
  extraeDatosLST(nmbArch+".lst") 
  xLaminaConstruyeModeloFicticio(datosScc1,datosScc2)
  nmbDiagIntSec1= "diagInt"+datosScc1.sectionName
  nmbDiagIntSec2= "diagInt"+datosScc2.sectionName
  xLaminaCalculaCombEstatLin(nmbArchDefHipELU,nmbDiagIntSec1,nmbDiagIntSec2)
  meanCFs= cv.writeControlVarsFromElements("ULS_normStr",preprocessor,nmbArch+"TN",datosScc1.sectionName,datosScc2.sectionName)
  return meanCFs

'''
 Lanza la comprobación de tensiones normales en una lámina
    cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
    con los materiales que se definen en el archivo nmbArchMateriales,
    las características de las secciones que se definen en los registros
    datosScc1 y datosScc2, las combinaciones definidas en el archivo
    nmbArchDefHipELU e imprime los resultados en archivos con
    el nombre nmbArchTN.*
'''
def lanzaCalculoTNFromXCData(preprocessor,analysis,intForcCombFileName,outputFileName, sectionsNamesForEveryElement,mapSectionsDefinition, mapInteractionDiagrams):
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
  controlVarName= "ULS_normStr"
  ec.extraeDatos(preprocessor,intForcCombFileName, sectionsNamesForEveryElement,mapSectionsDefinition, mapInteractionDiagrams,controlVarName)
  calculo_comb.xLaminaCalculaCombEstatLin(preprocessor,analysis,procesResultVerifTN)
  meanCFs= cv.writeControlVarsFromElements(controlVarName,preprocessor,outputFileName)
  return meanCFs

'''
 Lanza la comprobación de tensiones normales en una lámina
    cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
    con los materiales que se definen en el archivo nmbArchMateriales,
    las características de las secciones que se definen en los registros
    datosScc1 y datosScc2, las combinaciones definidas en el archivo
    nmbArchDefHipELU e imprime los resultados en archivos con
    el nombre nmbArchTN.*
    Emplea diagramas de interacción en 2D en lugar de 3D, en principio
    debería preferirse este modo pero aún queda comprobar la obtención
    de diagramas 2D. 
'''
def lanzaCalculoTN2dFromXCData(preprocessor,analysis,intForcCombFileName,outputFileName, sectionsNamesForEveryElement,mapSectionsDefinition, mapInteractionDiagrams):
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
  controlVarName= "ULS_normStr"
  ec.extraeDatos(preprocessor,intForcCombFileName, sectionsNamesForEveryElement,mapSectionsDefinition, mapInteractionDiagrams,controlVarName)
  calculo_comb.xLaminaCalculaCombEstatLin(preprocessor,analysis,procesResultVerifTN2d)
  meanCFs= cv.writeControlVarsFromElements(controlVarName,preprocessor,outputFileName)
  return meanCFs


