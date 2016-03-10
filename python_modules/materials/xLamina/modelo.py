# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
from materials.xLamina import seccion_ficticia_elementos as sf
from model import fix_node_6dof
from model import predefined_spaces
import pickle
import os
from postprocess.reports import exporta_esfuerzos
from solution import predefined_solutions

def defSectionsForShellSet(preprocessor,elemSet,trsvSection,longSection):
  '''
  Parameters:
    preprocessor:    preprocessor name
  '''
  mapSectionsForEveryElement= {}
  for e in elemSet:
    mapSectionsForEveryElement[e.tag]= [trsvSection,longSection]
  return mapSectionsForEveryElement

def saveSectionsForShellSet(mapSections,fName):
  with open(fName + '.pkl', 'wb') as f:
    pickle.dump(mapSections, f, pickle.HIGHEST_PROTOCOL)

def saveInternalForcesForCombs(feProblem,fName,loadCombinations,elemSet,fConv= 1.0):
  os.system("rm -f " + fName)
  for key in loadCombinations.getKeys():
    comb= loadCombinations[key]
    feProblem.getPreprocessor.resetLoadCase()
    comb.addToDomain()
    #Solución
    analisis= predefined_solutions.simple_static_linear(feProblem)
    result= analisis.analyze(1)
    f= open(fName,"a")
    exporta_esfuerzos.exportShellInternalForces(comb.getName,elemSet,f,fConv)
    f.close()
    comb.removeFromDomain()

# Construye el modelo para la comprobación de tensiones normales
def xLaminaConstruyeModeloFicticio(preprocessor,datosScc1, datosScc2):
  '''
  Parameters:
    preprocessor:    preprocessor name
  '''
  nodos= preprocessor.getNodeLoader

  predefined_spaces.gdls_resist_materiales3D(nodos)

  # Definimos materiales
  scc= sf.sccFICT.defSeccShElastica3d(preprocessor) # El problema es isóstático, así que la sección da igual
  elementos= preprocessor.getElementLoader
  elementos.dimElem= 1
  elementos.defaultMaterial= sf.sccFICT.nmb

  execfile("/tmp/elementos_scc1.xci")
  execfile("/tmp/elementos_scc2.xci")
  for e in elementos:
    HIPCP= "nil" #Hipótesis que produce el caso pésimo
    NCP= 0.0 #Valor del axil en la hipótesis que produce el caso pésimo.
    MyCP= 0.0 #Valor del momento en torno al eje y en la hipótesis que produce el caso pésimo.
    MzCP= 0.0 #Valor del momento en torno al eje z en la hipótesis que produce el caso pésimo.
    FCCP= 0.0 #Valor del factor de capacidad en la hipótesis que produce el caso pésimo.
    # Definimos los diagramas de interacción
    hormigon= preprocessor.getMaterialLoader.getMaterial(codHormigon)
    tagHorm= hormigon.getProp("matTagD")
    armadura= preprocessor.getMaterialLoader.getMaterial(codArmadura)
    tagHorm= armadura.getProp("matTagD")
    diagInt1= preprocessor.getMaterialLoader.newInteractionDiagram(datosScc1.sectionName,tagHorm,tagArmadura)
    diagInt2= preprocessor.getMaterialLoader.newInteractionDiagram(datosScc2.sectionName,tagHorm,tagArmadura)

    os.sys("rm -f "+"/tmp/elementos_scc1.xci")
    os.sys("rm -f "+"/tmp/elementos_scc2.xci")

# Construye el modelo para la comprobación a cortante
def xLaminaConstruyeModeloFibras(nmbRegDatosScc1, nmbRegDatosScc2):
  nodos= preprocessor.getNodeLoader

  predefined_spaces.gdls_resist_materiales3D(nodos)
  elementos= preprocessor.getElementLoader
  elementos.defaultMaterial= deref(nmbRegDatosScc1).sectionName
  execfile("/tmp/elementos_scc1.xci")
  for e in elementos:
    nmbRegDefSecc= "nil" #Nombre del registro que define la sección.
    if(odd(e.tag)):
      nmbRegDefSecc= nmbRegDatosScc1 # Sección 1
    else: 
      nmbRegDefSecc= nmbRegDatosScc2 # Sección 2
    HIPCP= "nil" #Hipótesis que produce el caso pésimo
    NCP= 0.0 #Valor del axil en la hipótesis que produce el caso pésimo.
    MyCP= 0.0 #Valor del momento en torno al eje y en la hipótesis que produce el caso pésimo.
    MzCP= 0.0 #Valor del momento en torno al eje z en la hipótesis que produce el caso pésimo.
    VyCP= 0.0 #Valor del cortante según y en la hipótesis que produce el caso pésimo.
    VzCP= 0.0 #Valor del cortante según z en la hipótesis que produce el caso pésimo.
    thetaCP= 0.0 #Ángulo de las bielas de hormigón en el caso pésimo.
    VcuCP= 0.0 #Valor de la contribución del hormigón a la resistencia al esfuerzo cortante en el caso pésimo.
    VsuCP= 0.0 #Valor de la contribución de las reinforcement a la resistencia al esfuerzo cortante en el caso pésimo.
    Vu1CP= 0.0 #Valor del agotamiento por compresión oblicua del alma en el caso pésimo.
    Vu2CP= 0.0 #Valor del agotamiento por tracción en el alma en el caso pésimo.
    VuCP= 0.0 #Valor del cortante último en la hipótesis que produce el caso pésimo.
    WkCP= 0.0 #Apertura de fisura en la hipótesis que produce el caso pésimo.
    FCCP= 0.0 #Valor del factor de cumplimiento en la hipótesis que produce el caso pésimo. 
  os.sys("rm -f "+"/tmp/elementos_scc1.xci")
  os.sys("rm -f "+"/tmp/elementos_scc2.xci")



