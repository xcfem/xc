# -*- coding: utf-8 -*-
# Comprobación de secciones de hormigón frente a tensiones normales.

import os
from materials.xBarra import extrae_combinaciones as ec
from materials.xBarra import calculo_comb
import geom
import math
from postprocess.reports import common_formats as fmt
import scipy

def even(number):
   return number % 2 == 0
def odd(number):
   return number % 2 != 0

# Ejecuta el análisis y la comprobación frente a tensiones normales
def xBarraCompruebaTNComb(preprocessor, nmbDiagIntSec):
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
    trataResultsComb(comb,nmbDiagIntSec)

  os.system("rm -f "+"/tmp/acciones.xci")
  os.system("rm -f "+"/tmp/cargas.xci")
  xBarraPrintTN(nmbArch) # XXX Sacar de aquí la impresión de result.

def trataResultsCombTN(preprocessor, nmbComb, diagIntSec):
  #print "Postproceso combinación: ",nmbComb
  elements= preprocessor.getSets["total"].getElements
  for e in elements:
    e.getResistingForce()
    TagTmp= e.tag
    scc= e.getSection()
    Ntmp= scc.getStressResultantComponent("N")
    MyTmp= scc.getStressResultantComponent("My")
    MzTmp= scc.getStressResultantComponent("Mz")
    posEsf= geom.Pos3d(Ntmp,MyTmp,MzTmp)
    FCtmp= diagIntSec.getCapacityFactor(posEsf)
    if(FCtmp>e.getProp("FCCP")):
      e.setProp("FCCP",FCtmp) # Caso pésimo
      e.setProp("HIPCP",nmbComb)
      e.setProp("NCP",Ntmp)
      e.setProp("MyCP",MyTmp)
      e.setProp("MzCP",MzTmp)

# Imprime los resultados de la comprobación frente a tensiones normales
def xBarraPrintTN(preprocessor,nmbArchSalida, sectionName):
  texOutput= open("/tmp/texOutput.tmp","w")
  texOutput.write("Section\n")
  ansysOutput= open(nmbArchSalida+".mac","w")
  #printCabeceraListadoFactorCapacidad("texOutput"," ("+ sectionName1 +")")
  fcs= [] #Capacity factors at section.
  elementos= preprocessor.getSets["total"].getElements
  for e in elementos:
    eTag= e.getProp("idElem")
    FCCP= e.getProp("FCCP")
    HIPCP= e.getProp("HIPCP")
    NCP= e.getProp("NCP")
    MyCP= e.getProp("MyCP")
    MzCP= e.getProp("MzCP")
    outStr= str(eTag)+" & "+HIPCP+" & "+fmt.Esf.format(NCP/1e3)+" & "+fmt.Esf.format(MyCP/1e3)+" & "+fmt.Esf.format(MzCP/1e3)+" & "+fmt.Esf.format(FCCP)+"\\\\\n"
    ansOutStr1= "detab,"+str(eTag)+",FC,"+str(FCCP)+"\n"
    ansOutStr2= "detab,"+str(eTag)+",N1,"+str(NCP/1e3)+"\n"
    ansOutStr3= "detab,"+str(eTag)+",My1,"+str(MyCP/1e3)+"\n"
    ansOutStr4= "detab,"+str(eTag)+",Mz1,"+str(MzCP/1e3)+"\n"
    fcs.append(FCCP)
    texOutput.write(outStr)
    ansysOutput.write(ansOutStr1)
    ansysOutput.write(ansOutStr2)
    ansysOutput.write(ansOutStr3)
    ansysOutput.write(ansOutStr4)
  
  #printCierreListadoFactorCapacidad("texOutput")
  texOutput.close()
  ansysOutput.close()
    
  os.system("cat /tmp/texOutput.tmp > "+nmbArchSalida+".tex")
    
  os.system("rm -f "+"/tmp/texOutput.tmp")
  return scipy.mean(fcs)



'''
 Lanza la comprobación de tensiones normales en una lámina
    cuyos esfuerzos se dan en el archivo de nombre nmbArch.lst
    con los materiales que se definen en el archivo nmbArchMateriales,
    las características de las secciones que se definen en los registros
    datosScc, las combinaciones definidas en el archivo
    nmbArchDefHipELU e imprime los resultados en archivos con
    el nombre nmbArchTN.*
'''
def lanzaCalculoTNFromXCData(preprocessor,analysis,nmbArchCsv,nmbArchSalida, diagIntScc):
  ec.extraeDatos(preprocessor,nmbArchCsv, diagIntScc)
  #nmbDiagIntSec= "diagInt"+datosScc.sectionName
  calculo_comb.xBarraCalculaCombEstatLin(preprocessor,analysis,diagIntScc,trataResultsCombTN)
  meanFCs= xBarraPrintTN(preprocessor,nmbArchSalida,"geomSecHA")
  return meanFCs


