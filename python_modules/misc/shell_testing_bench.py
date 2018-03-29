# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
from model import predefined_spaces
import sqlite3 as sqlite
from miscUtils import LogMessages as lmsg

def nuevosZeroLengthShell(nmbS, tagElem, offset):
# Define el modelo para probar las dos secciones de un elemento shell.
  '''
    nmbS: Nombre del material.
    nodeTag: node identifier.
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

  nodes= preprocessor.getNodeHandler
  nod1= nodes.newNodeIDZYZ(idNod1,idNod1,0,0)
  nod2= nodes.newNodeIDZYZ(idNod2,idNod1,0,0)
  nod3= nodes.newNodeIDZYZ(idNod3,0,idNod3,0)
  nod4= nodes.newNodeIDZYZ(idNod4,0,idNod3,0)

  constraints= preprocessor.getBoundaryCondHandler  # Boundary conditions.
  constraints.newSPConstraint(idNod1,0,0.0) # idNod1, DOF, constrValue
  constraints.newSPConstraint(idNod1,1,0.0)
  constraints.newSPConstraint(idNod1,2,0.0)
  constraints.newSPConstraint(idNod1,3,0.0)
  constraints.newSPConstraint(idNod1,4,0.0)
  constraints.newSPConstraint(idNod1,5,0.0)
  constraints.newSPConstraint(idNod2,0,0.0) # idNod2, DOF, constrValue
  constraints.newSPConstraint(idNod2,1,0.0)
  constraints.newSPConstraint(idNod2,2,0.0)
  constraints.newSPConstraint(idNod2,3,0.0)
  constraints.newSPConstraint(idNod2,4,0.0)
  constraints.newSPConstraint(idNod2,5,0.0)

  # Definimos elementos
  elems= preprocessor.getElementHandler
  elems.dimElem= 3 # Tridimensional space.
  elems.defaultMaterial= nmbS
  elems.defaultTag= idSecc1 #Next element number.
  zls1= elems.newElement("ZeroLengthSection",xc.ID([idNod1,iNod2]))
  elems.defaultTag= idSecc2 #Next element number.
  zls2= elems.newElement("ZeroLengthSection",xc.ID([idNod3,iNod4]))


def simulaShellElemFromTable(preprocessor,nmbDBase, nmbTablaElem, sectionName, offset):
  '''
  Crea los elementos a partir de la tabla creada anteriormente
     nmbDBase: Nombre de la base de datos donde se guardará la tabla.
     nmbTableElem: Nombre de la tabla que contiene los índices de elemento.
     sectionName: Nombre de la sección que se asigna a cada elemento.
  '''
  nodes= preprocessor.getNodeHandler
  modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
  elements= preprocessor.getElementHandler
  elements.dimElem= 1
  elements.defaultMaterial= sectionName

  con= sqlite.connect(nmbDataBase)
  con.row_factory = sqlite.Row
  idSecc= 0.0
  cur= con.cursor()
  cur.execute("select * from " + nmbTablaElem)
  for row in cur:
    idSecc= row["ELEM"]
    nuevosZeroLengthShell(sectionName,int(idElem),offset)


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

  cargas= preprocessor.getLoadHandler
  casos= cargas.getLoadPatterns

  lmsg.info("Cargando elemento: ",int(idElem),"\n")
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
 
