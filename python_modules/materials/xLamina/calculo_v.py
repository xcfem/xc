# -*- coding: utf-8 -*-
# Comprobación de secciones de hormigón frente a cortante.

import math
from materials.xLamina import extrae_combinaciones as ec
from materials.xLamina import calculo_comb
from materials.ehe import comprobVEHE08
from materials.ehe import cortanteEHE
from materials.ehe import torsionEHE
from materials.sia262 import shearSIA262
import geom
from postprocess.reports import common_formats as fmt
from postprocess import ControlVars as cv

 
def lanzaCalculoVFromAnsysData(preprocessor,nmbArch, nmbRegDatosScc1, nmbRegDatosScc2, nmbArchDefHipELU):
  '''
   Lanza la comprobación de cortante en una lámina
      cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
      con los materiales que se definen en el archivo nmbArchMateriales,
      las características de las secciones que se definen en los registros
      datosScc1 y datosScc2, las combinaciones definidas en el archivo
      nmbArchDefHipELU e imprime los resultados en archivos con
      el nombre nmbArchTN.*
  '''
  extraeDatosLST(nmbArch+".lst")
  xLaminaConstruyeModeloFibras(nmbRegDatosScc1,nmbRegDatosScc2)
  xLaminaCalculaCombEstatNoLin(nmbArchDefHipELU)
  cv.writeControlVarsFromElementsForAnsys('ULS_shear',preprocessor,nmbArch+"V",deref(nmbRegDatosScc1).sectionName,deref(nmbRegDatosScc2).sectionName)

def lanzaCalculoV(preprocessor,analysis,intForcCombFileName,outputFileName, sectionsNamesForEveryElement,mapSectionsDefinition, mapInteractionDiagrams,procesResultVerifV):
  '''
   Lanza la comprobación de cortante en una lámina
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
    procesResultVerif:          processing of the results of the verification          
  '''
  controlVarName= "ULS_shear"
  elems= ec.extraeDatos(preprocessor,intForcCombFileName, sectionsNamesForEveryElement,mapSectionsDefinition, mapInteractionDiagrams,controlVarName)
  calculo_comb.xLaminaCalculaComb(preprocessor,analysis,procesResultVerifV)
  meanFCs= cv.writeControlVarsFromElements(controlVarName,preprocessor,outputFileName)
  return meanFCs

