# -*- coding: utf-8 -*-

# Launcher of various calculations for RC elements of type xLamina, related to the 
# verificacion of Ultimate and Serviceability Limit States 

import os
import math
import geom
from miscUtils import LogMessages as lmsg
from materials.ehe import comprobVEHE08
from materials.ehe import cortanteEHE
from materials.ehe import torsionEHE
from materials.ehe import fisuracionEHE
from materials.sia262 import shearSIA262
from materials.sia262 import fatigueControlSIA262 as fcSIA
from materials.sia262 import crackControlSIA262 as ccSIA
from postprocess import ControlVars as cv
from postprocess import PhantomModel as phm
from postprocess.reports import common_formats as fmt

print 'Module calcsLauncher is DEPRECATED.' 

###                 *** Verifications related to 
###                     ULS of failure under Normal Stresses ***

def xLaminaCompruebaTNComb(preprocessor, nmbDiagIntSec1, nmbDiagIntSec2,controller):
  '''
  Perfoms the verification under normal stresses.

  :param preprocessor:    preprocessor name
  :param controller: object that control normal stress limit state.
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


def xLaminaCalculaCombEstatLin(preprocessor,elements, analysis, controller):
  '''
  Lanza el analisis (lineal) y la comprobacion en las combinaciones que se pasan como parametros

  :param  elements: elements to check
  :param  controller: object that controls limit state in elements.
  '''
  combs= preprocessor.getLoadLoader.getLoadPatterns #Here each load pattern represents a combination.
  for key in combs.getKeys():
    comb= combs[key]
    #print "Resolviendo para acción: ",key
    resuelve_combinacion.resuelveComb(preprocessor,key,analysis,1)
    controller.check(elements,key)

# Construye el modelo para la comprobación de tensiones normales
def xLaminaConstruyeModeloFicticio(preprocessor,datosScc1, datosScc2):
  '''
  :param  preprocessor:    preprocessor name
  '''
  nodos= preprocessor.getNodeLoader

  predefined_spaces.gdls_resist_materiales3D(nodos)

  # Definimos materiales
  scc= sccFICT.defSeccShElastica3d(preprocessor) # El problema es isóstático, así que la sección da igual
  elementos= preprocessor.getElementLoader
  elementos.dimElem= 1
  elementos.defaultMaterial= sccFICT.nmb

  execfile("/tmp/elementos_scc1.xci")
  execfile("/tmp/elementos_scc2.xci")
  for e in elementos:
    HIPCP= "nil" #Worst case hypothesis
    NCP= 0.0 #Valor del axil en la worst case hypothesis.
    MyCP= 0.0 #Valor del momento en torno al eje y en la worst case hypothesis.
    MzCP= 0.0 #Valor del momento en torno al eje z en la worst case hypothesis.
    FCCP= 0.0 #Valor del factor de capacidad en la worst case hypothesis.
    # Definimos los diagramas de interacción
    hormigon= preprocessor.getMaterialLoader.getMaterial(codHormigon)
    tagHorm= hormigon.getProp("matTagD")
    reinforcement= preprocessor.getMaterialLoader.getMaterial(codArmadura)
    tagHorm= reinforcement.getProp("matTagD")
    diagInt1= preprocessor.getMaterialLoader.newInteractionDiagram(datosScc1.sectionName,tagHorm,tagArmadura)
    diagInt2= preprocessor.getMaterialLoader.newInteractionDiagram(datosScc2.sectionName,tagHorm,tagArmadura)

    os.sys("rm -f "+"/tmp/elementos_scc1.xci")
    os.sys("rm -f "+"/tmp/elementos_scc2.xci")

def lanzaCalculoTNFromAnsysData(nmbArch, datosScc1, datosScc2, nmbArchDefHipELU):
  '''
  Lanza la comprobacion de tensiones normales en una lamina
  cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
  con los materiales que se definen en el archivo nmbArchMateriales,
  las caracteristicas de las secciones que se definen en los registros
  datosScc1 y datosScc2, las combinaciones definidas en el archivo
  nmbArchDefHipELU e imprime los resultados en archivos con
  el nombre nmbArchTN.*
  '''
  extraeDatosLST(nmbArch+".lst") 
  xLaminaConstruyeModeloFicticio(datosScc1,datosScc2)
  nmbDiagIntSec1= "diagInt"+datosScc1.sectionName
  nmbDiagIntSec2= "diagInt"+datosScc2.sectionName
  xLaminaCalculaCombEstatLin(nmbArchDefHipELU,nmbDiagIntSec1,nmbDiagIntSec2)
  meanCFs= cv.writeControlVarsFromElements("ULS_normStr",preprocessor,nmbArch+"TN",datosScc1.sectionName,datosScc2.sectionName)
  return meanCFs

def getListaCombinaciones(nmbArchDefHipELU):
  '''
  Define las combinaciones descritas en el archivo que se pasa como parametro
  y coloca sus nombres en la lista que devuelve.
  nmbArchDefHipELU: Archivo en el que se definen las combinaciones a calcular. 
  '''
  lstCombRetval= []
  cargas= preprocessor.getLoadLoader
  casos= cargas.getLoadPatterns
  ts= casos.newTimeSeries("constant_ts","ts")
  casos.currentTimeSeries= "ts"
  execfile("/tmp/acciones.xci")
  execfile("/tmp/cargas.xci")
  execfile(nmbArchDefHipELU)

  lstCombRetval= cargas.listaNombresCombinaciones()
  os.sys("rm -f "+"/tmp/acciones.xci")
  os.sys("rm -f "+"/tmp/cargas.xci")
  return lstCombRetval

def xLaminaCalculaCombEstatNoLin(elements,nmbArchDefHipELU,controller):
  '''
  Lanza el analisis (no lineal) y la comprobacion en las combinaciones que se pasan como parametros

  :param  nmbArchDefHipELU: Archivo en el que se definen las combinaciones a calcular.
  :param  controller: object that controls limit state in elements.
  '''
  # Solution method definition.
  print "XXX Definir el procedimiento de solution (simple_newton_raphson_band_genr)"
  listaCombinaciones= getListaCombinaciones(nmbArchDefHipELU)
  for comb in listaCombinaciones:
    print "Resolviendo para acción: ",comb
    resuelveCombEstatNoLin(comb)
    controller.check(elements,comb,nmbDiagIntSec1,nmbDiagIntSec2)
  os.sys("rm -f "+"/tmp/acciones.xci")
  os.sys("rm -f "+"/tmp/cargas.xci")

# Builds the model for shear checking.
def xLaminaConstruyeModeloFibras(nmbRegDatosScc1, nmbRegDatosScc2):
  nodos= preprocessor.getNodeLoader

  predefined_spaces.gdls_resist_materiales3D(nodos)
  elementos= preprocessor.getElementLoader
  elementos.defaultMaterial= deref(nmbRegDatosScc1).sectionName
  execfile("/tmp/elementos_scc1.xci")
  for e in elementos:
    nmbRegDefSecc= "nil" #Name of the record that defines cross section.
    if(odd(e.tag)):
      nmbRegDefSecc= nmbRegDatosScc1 # cross section 1
    else: 
      nmbRegDefSecc= nmbRegDatosScc2 # cross section 2
    HIPCP= "nil" #worst case hypothesis
    NCP= 0.0 #Valor del axil for the worst case hypothesis.
    MyCP= 0.0 #Bending moment value en torno al eje y for the worst case hypothesis.
    MzCP= 0.0 #Bending moment value en torno al eje z for the worst case hypothesis.
    VyCP= 0.0 #Shear value según y for the worst case hypothesis.
    VzCP= 0.0 #Shear value según z for the worst case hypothesis.
    thetaCP= 0.0 #Concrete struts angle in the worst case.
    VcuCP= 0.0 #Concrete contribution to shear resistance in the worst case.
    VsuCP= 0.0 #Reinforcement contribution to shear resistance in the worst case.
    Vu1CP= 0.0 #Ultimate shear due to oblique compression in the web in the worst case.
    Vu2CP= 0.0 #Ultimate shear due to web traction in the worst case.
    VuCP= 0.0 #Ultimate shear value for the worst case hypothesis.
    WkCP= 0.0 #Crack opening for the worst case hypothesis.
    FCCP= 0.0 #Capacity factor for the worst case hypothesis. 
  os.sys("rm -f "+"/tmp/elementos_scc1.xci")
  os.sys("rm -f "+"/tmp/elementos_scc2.xci")


def lanzaCalculoVFromAnsysData(preprocessor,nmbArch, nmbRegDatosScc1, nmbRegDatosScc2, nmbArchDefHipELU):
  '''
  Lanza la comprobacion de cortante en una lamina
  cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
  con los materiales que se definen en el archivo nmbArchMateriales,
  las caracteristicas de las secciones que se definen en los registros
  datosScc1 y datosScc2, las combinaciones definidas en el archivo
  nmbArchDefHipELU e imprime los resultados en archivos con
  el nombre nmbArchTN.*
  '''
  extraeDatosLST(nmbArch+".lst")
  xLaminaConstruyeModeloFibras(nmbRegDatosScc1,nmbRegDatosScc2)
  xLaminaCalculaCombEstatNoLin(nmbArchDefHipELU)
  cv.writeControlVarsFromElementsForAnsys('ULS_shear',preprocessor,nmbArch+"V",deref(nmbRegDatosScc1).sectionName,deref(nmbRegDatosScc2).sectionName)

def strElementProp(eTag,nmbProp,vProp):
  retval= "preprocessor.getElementLoader.getElement("
  retval+= str(eTag)
  retval+= ").setProp("
  retval+= '"' + nmbProp + '"'
  retval+= ',' + str(vProp) + ")\n"
  return retval

def xLaminaPrintFatigueSIA262(preprocessor,outputFileName, mapSections):
  '''

  :param  preprocessor:    preprocessor name
  '''
  # Prints crack control checking results.
  texOutput1= open("/tmp/texOutput1.tmp","w")
  texOutput2= open("/tmp/texOutput2.tmp","w")
  xcOutput= open(outputFileName+".py","w")
  elementos= preprocessor.getSets.getSet("total").getElements
  strHeader0= "eTag & idSection & N0 kN & My0 kN m/m & Mz0 kN m/m & Vy0 kN m/m & Vz0 kN m/m & $sg_{s,0} MPa & $sg_{c,0} MPa \\\\\n"
  strHeader1= "     &           & N1 kN & My1 kN m/m & Mz1 kN m/m & Vy1 kN m/m & Vz1 kN m/m & $sg_{s,1} MPa & $sg_{c,1} MPa \\\\\n"
  texOutput1.write(strHeader0)
  texOutput1.write(strHeader1)
  texOutput2.write(strHeader0)
  texOutput2.write(strHeader1)
  for e in elementos:
    tag= e.getProp("idElem")
    idSection= e.getProp("idSection")
    N0= e.getProp("N0")
    My0= e.getProp("My0")
    Mz0= e.getProp("Mz0")
    Vy0= e.getProp("Vy0")
    Vz0= e.getProp("Vz0")
    sg_sPos0= e.getProp("sg_sPos0")
    sg_sNeg0= e.getProp("sg_sNeg0")
    sg_c0= e.getProp("sg_c0")
    N1= e.getProp("N1")
    My1= e.getProp("My1")
    Mz1= e.getProp("Mz1")
    Vy1= e.getProp("Vy1")
    Vz1= e.getProp("Vz1")
    sg_sPos1= e.getProp("sg_sPos1")
    sg_sNeg1= e.getProp("sg_sNeg1")
    sg_c1= e.getProp("sg_c1")
    inc_sg_sPos= sg_sPos1-sg_sPos0
    inc_sg_sNeg= sg_sNeg1-sg_sNeg0
    inc_sg_s= max(inc_sg_sPos,inc_sg_sNeg)
    inc_sg_c= sg_c1-sg_c0
    Mu= e.getProp("Mu")
    Vu= e.getProp("Vu")
    lim_sg_c= e.getProp("lim_sg_c")
    fc_sg_c= e.getProp("fc_sg_c")
    lim_v= e.getProp("lim_v")
    fc_v= e.getProp("fc_v")

    strEsf0= "N0=  &"+fmt.Esf.format(N0/1e3)+" & "+"My0=  &"+fmt.Esf.format(My0/1e3)+" & "+"Mz0=  &"+fmt.Esf.format(Mz0/1e3)+" & "+"Vy0=  &"+fmt.Esf.format(Vy0/1e3)+" & "+"Vz0=  &"+fmt.Esf.format(Vz0/1e3)
    strStress0= "sgAxPos0=  &"+fmt.Stress.format(sg_sPos0/1e6)+" & "+"sgAxNeg0=  &"+fmt.Stress.format(sg_sNeg0/1e6)+" & "+"sgC0=  &"+fmt.Stress.format(sg_c0/1e6)
    strEsf1= "N1=  &"+fmt.Esf.format(N1/1e3)+" & "+"My1=  &"+fmt.Esf.format(My1/1e3)+" & "+"Mz1=  &"+fmt.Esf.format(Mz1/1e3)+" & "+"Vy1=  &"+fmt.Esf.format(Vy1/1e3)+" & "+"Vz1=  &"+fmt.Esf.format(Vz1/1e3)
    strStress1= "sgAxPos1=  &"+fmt.Stress.format(sg_sPos1/1e6)+" & "+"sgAxNeg1=  &"+fmt.Stress.format(sg_sNeg1/1e6)+" & "+"sgC1=  &"+fmt.Stress.format(sg_c1/1e6)
    strEsfUl= fmt.Esf.format(Mu/1e3)+" & "+fmt.Esf.format(Vu/1e3)

    strOut0= str(tag)+" & " +idSection + " & "+strEsf0+" & "+strStress0+"\\\\\n"
    strOut1= str(tag)+" & " +idSection + " & "+strEsf1+" & "+strStress1+" & "+strEsfUl+"\\\\\n"

    if(e.getProp("dir")==1):
      texOutput1.write(strOut0)
      texOutput1.write(strOut1)
      xcOutput.write(strElementProp(tag,"sg_sPos01",sg_sPos0/1e6))
      xcOutput.write(strElementProp(tag,"sg_sNeg01",sg_sNeg0/1e6))
      xcOutput.write(strElementProp(tag,"sg_c01",sg_c0/1e6))
      xcOutput.write(strElementProp(tag,"N01",N0/1e3))
      xcOutput.write(strElementProp(tag,"My01",My0/1e3))
      xcOutput.write(strElementProp(tag,"Mz01",Mz0/1e3))
      xcOutput.write(strElementProp(tag,"Vy01",Vy0/1e3))
      xcOutput.write(strElementProp(tag,"Vz01",Vz0/1e3))
      xcOutput.write(strElementProp(tag,"sg_sPos11",sg_sPos1/1e6))
      xcOutput.write(strElementProp(tag,"sg_sNeg11",sg_sNeg1/1e6))
      xcOutput.write(strElementProp(tag,"N11",N0/1e3))
      xcOutput.write(strElementProp(tag,"My11",My1/1e3))
      xcOutput.write(strElementProp(tag,"Mz11",Mz1/1e3))
      xcOutput.write(strElementProp(tag,"Vy11",Vy1/1e3))
      xcOutput.write(strElementProp(tag,"Vz11",Vz1/1e3))
      xcOutput.write(strElementProp(tag,"inc_sg_sPos1",inc_sg_sPos/1e6))
      xcOutput.write(strElementProp(tag,"inc_sg_sNeg1",inc_sg_sNeg/1e6))
      xcOutput.write(strElementProp(tag,"inc_sg_s1",inc_sg_s/1e6))
      xcOutput.write(strElementProp(tag,"inc_sg_c1",inc_sg_c/1e6))
      xcOutput.write(strElementProp(tag,"lim_sg_c1",lim_sg_c/1e6))
      xcOutput.write(strElementProp(tag,"fc_sg_c1",fc_sg_c))
      xcOutput.write(strElementProp(tag,"Mu1",Mu/1e3))
      xcOutput.write(strElementProp(tag,"Vu1",Vu/1e3))
      xcOutput.write(strElementProp(tag,"lim_v1",lim_v/1e6))
      xcOutput.write(strElementProp(tag,"fc_v1",fc_v))
    else:
      texOutput2.write(strOut0)
      texOutput2.write(strOut1)
      xcOutput.write(strElementProp(tag,"sg_sPos02",sg_sPos0/1e6))
      xcOutput.write(strElementProp(tag,"sg_sNeg02",sg_sNeg0/1e6))
      xcOutput.write(strElementProp(tag,"sg_c02",sg_c0/1e6))
      xcOutput.write(strElementProp(tag,"N02",N0/1e3))
      xcOutput.write(strElementProp(tag,"My02",My0/1e3))
      xcOutput.write(strElementProp(tag,"Mz02",Mz0/1e3))
      xcOutput.write(strElementProp(tag,"Vy02",Vy0/1e3))
      xcOutput.write(strElementProp(tag,"Vz02",Vz0/1e3))
      xcOutput.write(strElementProp(tag,"sg_sPos12",sg_sPos1/1e6))
      xcOutput.write(strElementProp(tag,"sg_sNeg12",sg_sNeg1/1e6))
      xcOutput.write(strElementProp(tag,"N12",N0/1e3))
      xcOutput.write(strElementProp(tag,"My12",My1/1e3))
      xcOutput.write(strElementProp(tag,"Mz12",Mz1/1e3))
      xcOutput.write(strElementProp(tag,"Vy12",Vy1/1e3))
      xcOutput.write(strElementProp(tag,"Vz12",Vz1/1e3))
      xcOutput.write(strElementProp(tag,"inc_sg_sPos2",inc_sg_sPos/1e6))
      xcOutput.write(strElementProp(tag,"inc_sg_sNeg2",inc_sg_sNeg/1e6))
      xcOutput.write(strElementProp(tag,"inc_sg_s2",inc_sg_s/1e6))
      xcOutput.write(strElementProp(tag,"inc_sg_c2",inc_sg_c/1e6))
      xcOutput.write(strElementProp(tag,"lim_sg_c2",lim_sg_c/1e6))
      xcOutput.write(strElementProp(tag,"fc_sg_c2",fc_sg_c))
      xcOutput.write(strElementProp(tag,"Mu2",Mu/1e3))
      xcOutput.write(strElementProp(tag,"Vu2",Vu/1e3))
      xcOutput.write(strElementProp(tag,"lim_v2",lim_v/1e6))
      xcOutput.write(strElementProp(tag,"fc_v2",fc_v))


  texOutput1.close()
  texOutput2.close()
  xcOutput.close()
    
  os.system("cat /tmp/texOutput1.tmp /tmp/texOutput2.tmp > "+outputFileName+".tex")
    
  # os.system("rm -f "+"/tmp/acciones.xci")
  # os.system("rm -f "+"/tmp/cargas.xci")
  # os.system("rm -f "+"/tmp/elementos.xci")
  os.system("rm -f "+"/tmp/texOutput1.tmp")
  os.system("rm -f "+"/tmp/texOutput2.tmp")


###                 *** Verifications related to 
###                     Cracking SLS  ***

# def lanzaCalculoFIS(nmbArch, nmbRegDatosScc1, nmbRegDatosScc2, nmbArchDefHipELS):
#   '''
#    Runs crack control for a shell
#       cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
#       con los materiales que se definen en el archivo nmbArchMateriales,
#       las caracteristicas de las secciones que se definen en los registros
#       datosScc1 y datosScc2, las combinaciones definidas en el archivo
#       nmbArchDefHipELS e imprime los resultados en archivos con
#       el nombre nmbArchFis.*
#   '''
#   extraeDatosLST(nmbArch+".lst")
#   xLaminaConstruyeModeloFibras(nmbRegDatosScc1,nmbRegDatosScc2)
#   xLaminaCalculaCombEstatNoLin(nmbArchDefHipELS)
#   xLaminaPrintFIS(nmbArch+"FIS",deref(nmbRegDatosScc1).sectionName,deref(nmbRegDatosScc2).sectionName)


