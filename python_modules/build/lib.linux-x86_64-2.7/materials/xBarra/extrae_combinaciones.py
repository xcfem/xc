# -*- coding: utf-8 -*-
import csv
import re
import numpy
import xc_base
import geom
import xc
from model import predefined_spaces
from model import fix_node_6dof
from materials.xLamina import seccion_ficticia_elementos as sf
import math

def extraeIdsElem(mdlr,nmbArchComb, diagIntScc):
  ''' Extrae los identificadores de elementos de un archivo de salida con resultados
  de combinaciones generado en XC '''
  idElements= set()
  idCombs= set()

  f= open(nmbArchComb,"r")
  listado= csv.reader(f)
  for lst in listado:
    if(len(lst)>0):
      idComb= lst[0]
      idCombs.add(idComb)
      idElem= eval(lst[1])
      idElements.add(idElem)
  f.close()
   
  nodes= mdlr.getNodeLoader
  predefined_spaces.gdls_resist_materiales3D(nodes)
  elements= mdlr.getElementLoader
  coacciones= mdlr.getConstraintLoader
  # Definimos materiales
  scc= sf.sccFICT.defSeccShElastica3d(mdlr) # El problema es isóstático, así que la sección da igual
  elements.dimElem= 1
  elements.defaultMaterial= sf.sccFICT.nmb
  for tagElem in idElements:
    n1= nodes.newNodeIDXYZ(tagElem*10+1,0,0,0)
    n2= nodes.newNodeIDXYZ(tagElem*10+2,0,0,0)
    fix_node_6dof.fixNode6DOF(coacciones,n1.tag)
    e1= elements.newElement("zero_length_section",xc.ID([n1.tag,n2.tag]))
    e1.setProp("idElem", int(math.floor(n1.tag/10))) #Elemento que se comprueba
    e1.setProp("HIPCP", "nil") #Hipótesis que produce el caso pésimo
    e1.setProp("NCP", 0.0) #Valor del axil en la hipótesis que produce el caso pésimo.
    e1.setProp("MyCP", 0.0) #Valor del momento en torno al eje y en la hipótesis que produce el caso pésimo.
    e1.setProp("MzCP", 0.0) #Valor del momento en torno al eje z en la hipótesis que produce el caso pésimo.
    e1.setProp("FCCP", 0.0) #Valor del factor de capacidad en la hipótesis que produce el caso pésimo.
    e1.setProp("diagInt",diagIntScc) 
 
  cargas= mdlr.getLoadLoader
  casos= cargas.getLoadPatterns
  #Load modulation.
  ts= casos.newTimeSeries("constant_ts","ts")
  casos.currentTimeSeries= "ts"
  #Load case definition
  mapCombs= {}
  for comb in idCombs:
    mapCombs[comb]= casos.newLoadPattern("default",comb)

  f= open(nmbArchComb,"r")
  listado= csv.reader(f)
  for lst in listado:
    if(len(lst)>0):
      nmbComb= lst[0]
      idElem= eval(lst[1])
      lp= mapCombs[nmbComb]
      tagNode3= idElem*10+2
      # AXIL_X,Q_X,Q_Y,TORSOR,MOM_Y,MOM_Z
      #      2,  3,  4,     5,    6,    7
      N= eval(lst[2])
      Qy= eval(lst[3])
      Qz= eval(lst[4])
      T= eval(lst[5])
      My= eval(lst[6])
      Mz= eval(lst[7])
      lp.newNodalLoad(tagNode3,xc.Vector([N,Qy,Qz,T,My,Mz]))

  f.close()

def extraeDatos(mdlr,nmbArchLST, diagIntScc):
  ''' Define las cargas en el extremo libre de cada elemento a partir
   de las combinaciones de un archivo de salida generado en XC '''
  extraeIdsElem(mdlr,nmbArchLST,diagIntScc)

