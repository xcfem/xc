# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
from model import predefined_spaces
import sqlite3 as sqlite
import logging

def nuevosZeroLengthShell(nmbS, tagElem, offset):
# Define el modelo para probar las dos secciones de un elemento shell.
  '''
    nmbS: Nombre del material.
    tagNodo: Identificador del nodo.
    tagElem: Identificador del elemento.
  '''
  if(tagElem>=offset):
    print "nuevosZeroLengthShell; excedido el número del elementos, aumente el valor de offset\n"

  idSecc1= tagElem*10
  idNod1= idSecc1
  idNod2= idSecc1+1
  idSecc2= offset+idSecc1
  idNod3= idSecc2
  idNod4= idSecc2+1

  nodos= preprocessor.getNodeLoader
  nod1= nodos.newNodeIDZYZ(idNod1,idNod1,0,0)
  nod2= nodos.newNodeIDZYZ(idNod2,idNod1,0,0)
  nod3= nodos.newNodeIDZYZ(idNod3,0,idNod3,0)
  nod4= nodos.newNodeIDZYZ(idNod4,0,idNod3,0)

  coacciones= preprocessor.getConstraintLoader  # Condiciones de contorno
  fijaNodo6GDL(coacciones,idNod1)
  fijaNodo6GDL(coacciones,idNod3)

  # Definimos elementos
  elems= preprocessor.getElementLoader
  elems.dimElem= 3 # Tridimensional space.
  elems.defaultMaterial= nmbS
  elems.defaultTag= idSecc1 #Next element number.
  zls1= elems.newElement("zero_lenght_section",xc.ID([idNod1,iNod2]))
  elems.defaultTag= idSecc2 #Next element number.
  zls2= elems.newElement("zero_lenght_section",xc.ID([idNod3,iNod4]))


def simulaShellElemFromTable(preprocessor,nmbDBase, nmbTablaElem, nmbSeccion, offset):
  '''
  Crea los elementos a partir de la tabla creada anteriormente
     nmbDBase: Nombre de la base de datos donde se guardará la tabla.
     nmbTableElem: Nombre de la tabla que contiene los índices de elemento.
     nmbSeccion: Nombre de la sección que se asigna a cada elemento.
  '''
  nodos= preprocessor.getNodeLoader
  prdefined_spaces.gdls_resist_materiales3D(nodos)
  elements= preprocessor.getElementLoader
  elements.dimElem= 1
  elements.defaultMaterial= nmbSeccion

  con= sqlite.connect(nmbDataBase)
  con.row_factory = sqlite.Row
  idSecc= 0.0
  cur= con.cursor()
  cur.execute("select * from " + nmbTablaElem)
  for row in cur:
    idSecc= row["ELEM"]
    nuevosZeroLengthShell(nmbSeccion,int(idElem),offset)


def simulaCargasXYFromTable(nmbQuery, nmbTbEsf, idElem, offset):
  '''
  Crea las cargas sobre cada uno de los elementos a partir de las tablas creadas anteriormente
   nmbQuery: Nombre de la consulta que se empleara para obtener las cargas.
   nmbTbEsf: Nombre de la tabla que contiene los índices de sección.
   idElem: Identificador del elemento shell.
  '''
  idSecc1= idElem*10
  iNod1= idSecc1+1
  idSecc2= offset+idSecc1
  iNod2= idSecc2+1

  cargas= preprocessor.getLoadLoader
  casos= cargas.getLoadPatterns

  logging.info("Cargando elemento: ",int(idElem),"\n")
  con= sqlite.connect(nmbDataBase)
  con.row_factory = sqlite.Row
  idSecc= 0.0
  cur= con.cursor()
  cur.execute("select * from "+ nmbTbEsf + " where ELEM = " + sqlValue(idElem))
  for row in cur:
    idAccion= row['ACCION']
    lp= casos.getLoadPattern(idAccion)
    lp.setCurrentLoadPattern()
    lp.newNodalLoad(iNod1,xc.Vector([row['AXIL_X'],row['Q_X'],row['RASANTE'],row['TORSOR'],row['MOM_X'],0.0]))
    lp.newNodalLoad(iNod2,xc.Vector([row['AXIL_Y'],row['Q_Y'],row['RASANTE'],row['TORSOR'],row['MOM_Y'],0.0]))

def simulaCargasSeccionesShell(nmbDatabase, nmbTablaEsfuerzos, nmbQueryEsfuerzos, nmbTablaSecc, offset):
  # Introduce las cargas en cada una de las secciones
  resetAccionesConstantTS("constant_ts","ts",1000) # Factor 1000 (acciones en kN y kN m)

  con= sqlite.connect(nmbDataBase)
  con.row_factory = sqlite.Row
  idElem= 0
  cur= con.cursor()
  cur.execute("select * from " + nmbTablaSecc)
  for row in cur:
    idElem= row['ELEM']
    simulaCargasXYFromTable(nmbQueryEsfuerzos,nmbTablaEsfuerzos,idElem,offset)
 
