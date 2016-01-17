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

 
def extraeIdsElem(preprocessor,intForcCombFileName, mapSectionsForEveryElement, mapSectionsDefinition, mapInteractionDiagrams):
  ''' Extrae los identificadores de elementos de un archivo de salida con resultados
  de combinaciones generado en XC 
  Parameters:
    preprocessor:        preprocessor name
    intForcCombFileName: name of the file containing the forces and bending moments 
                         obtained for each element for the combinations analyzed
   mapSectionsForEveryElement:  file containing a dictionary  such that for each                                        element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    mapSectionsDefinition:      file containing a dictionary with the two 
                                sections associated with each elements to be
                                used in the verification
    mapInteractionDiagrams:     file containing a dictionary such that                                                      associates each element with the two interactions
                                diagrams of materials to be used in the verification  
  '''
  idElements= set()
  idCombs= set()
  retval= []
  f= open(intForcCombFileName,"r")
  listado= csv.reader(f)
  for lst in listado:
    if(len(lst)>0):
      idComb= lst[0]
      idCombs.add(idComb)
      idElem= eval(lst[1])
      idElements.add(idElem)
  f.close()
   
  nodes= preprocessor.getNodeLoader
  predefined_spaces.gdls_resist_materiales3D(nodes)
  elements= preprocessor.getElementLoader
  coacciones= preprocessor.getConstraintLoader
  # Definimos materiales
  scc= sf.sccFICT.defSeccShElastica3d(preprocessor) # El problema es isóstático, así que la sección da igual
  elements.dimElem= 1
  tagsNodesToLoad1= {}
  tagsNodesToLoad2= {}
  elements.defaultMaterial= sf.sccFICT.nmb
  for tagElem in idElements:
    print "******************* tagElem: ", tagElem, "num. elementos: ", len(mapSectionsForEveryElement)
    nmbScc1= mapSectionsForEveryElement[tagElem][0]
    n1= nodes.newNodeXYZ(0,0,0)
    n3= nodes.newNodeXYZ(0,0,0)
    fix_node_6dof.fixNode6DOF(coacciones,n1.tag)
    tagsNodesToLoad1[tagElem]= n3.tag
    e1= elements.newElement("zero_length_section",xc.ID([n1.tag,n3.tag]))
    retval.append(e1)
    e1.setProp("idElem", tagElem) #Elemento que se comprueba
    e1.setProp("dir",1)
    e1.setProp("idSection", nmbScc1) #Section to verify
    scc= e1.getSection()
    scc.setProp("datosSecc", mapSectionsDefinition[nmbScc1]) #Section definition (XXX duplicated)
    e1.setProp("HIPCP", "nil") #Hipótesis que produce el caso pésimo
    e1.setProp("NCP", 0.0) #Valor del axil en la hipótesis que produce el caso pésimo.
    e1.setProp("MyCP", 0.0) #Valor del momento en torno al eje y en la hipótesis que produce el caso pésimo.
    e1.setProp("MzCP", 0.0) #Valor del momento en torno al eje z en la hipótesis que produce el caso pésimo.
    e1.setProp("FCCP", 0.0) #Valor del factor de capacidad en la hipótesis que produce el caso pésimo.
    if(mapInteractionDiagrams != None):
      diagIntScc1= mapInteractionDiagrams[nmbScc1]
      e1.setProp("diagInt",diagIntScc1) 

    nmbScc2= mapSectionsForEveryElement[tagElem][1]
    n2= nodes.newNodeIDXYZ(tagElem*10+2,0,0,0)
    n4= nodes.newNodeIDXYZ(tagElem*10+4,0,0,0)
    fix_node_6dof.fixNode6DOF(coacciones,n2.tag)
    tagsNodesToLoad2[tagElem]= n4.tag
    e2= elements.newElement("zero_length_section",xc.ID([n2.tag,n4.tag])) 
    retval.append(e2)
    e2.setProp("idElem", tagElem) #Elemento que se comprueba
    e2.setProp("dir",2)
    e2.setProp("idSection", nmbScc2) #Section to verify
    scc= e2.getSection()
    scc.setProp("datosSecc",  mapSectionsDefinition[nmbScc2]) #Section definition.
    e2.setProp("HIPCP", "nil") #Hipótesis que produce el caso pésimo
    e2.setProp("NCP", 0.0) #Valor del axil en la hipótesis que produce el caso pésimo.
    e2.setProp("MyCP", 0.0) #Valor del momento en torno al eje y en la hipótesis que produce el caso pésimo.
    e2.setProp("MzCP", 0.0) #Valor del momento en torno al eje z en la hipótesis que produce el caso pésimo.
    e2.setProp("FCCP", 0.0) #Valor del factor de capacidad en la hipótesis que produce el caso pésimo.
    if(mapInteractionDiagrams != None):
      diagIntScc2= mapInteractionDiagrams[nmbScc2]
      e2.setProp("diagInt",diagIntScc2)
 
 
  cargas= preprocessor.getLoadLoader
  casos= cargas.getLoadPatterns
  #Load modulation.
  ts= casos.newTimeSeries("constant_ts","ts")
  casos.currentTimeSeries= "ts"
  #Load case definition
  mapCombs= {}
  for comb in idCombs:
    mapCombs[comb]= casos.newLoadPattern("default",comb)

  f= open(intForcCombFileName,"r")
  listado= csv.reader(f)
  for lst in listado:
    if(len(lst)>0):
      nmbComb= lst[0]
      idElem= eval(lst[1])
      lp= mapCombs[nmbComb]
      tagNode3= tagsNodesToLoad1[idElem]
      '''Adoptamos el método de Wood para evaluar los momentos para dimensionar a flexión'''
      # AXIL_X,Q_X,RASANTE,TORSOR=0,MOM_X*,0.0
      #      2,  8,      4,     7,    5,0.0
      N= eval(lst[2])
      Qx= eval(lst[8])
      R= eval(lst[4])
      T= 0.0
      Mx= eval(lst[5])
      s= float(numpy.sign(Mx))
      Mx+= s*abs(eval(lst[7]))
      My= 0.0
      lp.newNodalLoad(tagNode3,xc.Vector([N,Qx,R,T,Mx,My]))
 
      tagNode4= tagsNodesToLoad2[idElem]
      # AXIL_Y,Q_Y,RASANTE,TORSOR=0,MOM_Y*,0.0
      #      3,  9,      4,     7,    6,0.0
      N= eval(lst[3])
      Qx= eval(lst[9])
      R= eval(lst[4])
      T= 0.0
      Mx= eval(lst[6])
      s= float(numpy.sign(Mx))
      Mx+= s*abs(eval(lst[7]))
      My= 0.0
      lp.newNodalLoad(tagNode4,xc.Vector([N,Qx,R,T,Mx,My]))

  f.close()
  return retval

def extraeCargasLamina(nmbArch, nmbArchCargas):
  ''' Define las cargas en el extremo libre de dada elemento a partir
   de las combinaciones de un archivo de salida generado en XC '''
  cargas= open(nmbArchCargas,"w")
  nmbComb= "nil"

  f= open(nmbArch,"r")
  listado= csv.reader(f)
  for lst in listado:
    if(len(lst)>0):
      if(nmbComb != lst[0]): # Nueva combinación.
        nmbComb= lst[0]
        tagNode3= eval(lst[1])*10+3
        '''Adoptamos el método de Wood para evaluar los momentos para dimensionar a flexión'''
        # AXIL_X,Q_X,RASANTE,TORSOR=0,MOM_X*,0.0
        #      2,  8,      4,     7,    5,0.0
        loadNode3= lst[2]+","+lst[8]+","+lst[4]+",0.0,"+lst[5]+str(numpy.sign(eval(lst[5]))*abs(eval(lst[7])))+",0.0\n"
        cargas.write(nmbComb+","+str(tagNode3)+","+loadNode3)
        tagNode4= eval(lst[1])*10+3
        # AXIL_Y,Q_Y,RASANTE,TORSOR=0,MOM_Y*,0.0
        #      3,  9,      4,     7,    6,0.0
        loadNode4= lst[3]+","+lst[9]+","+lst[4]+",0.0,"+lst[6]+str(numpy.sign(eval(lst[6]))*abs(eval(lst[7])))+",0.0\n"
        cargas.write(nmbComb+","+str(tagNode4)+","+loadNode4)

  f.close()
  cargas.close()


def extraeDatos(preprocessor,intForcCombFileName, mapSectionsForEveryElement, mapSectionsDefinition, mapInteractionDiagrams):
  ''' Define las cargas en el extremo libre de cada elemento a partir
   de las combinaciones de un archivo de salida generado en XC 
  Parameters:
    preprocessor:    preprocessor name
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    mapSectionsForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
    mapSectionsDefinition:      file containing a dictionary with the two 
                                sections associated with each elements to be
                                used in the verification
    mapInteractionDiagrams:     file containing a dictionary such that                                                      associates each element with the two interactions
                                diagrams of materials to be used in the verification process
  '''
  return extraeIdsElem(preprocssor,intForcCombFileName,mapSectionsForEveryElement, mapSectionsDefinition, mapInteractionDiagrams)

def creaElems(preprocessor,intForcCombFileName, mapSectionsForEveryElement):
  ''' Extrae los identificadores de elementos de un archivo de salida con resultados
  de combinaciones generado en XC 
  Parameters:
    preprocessor:    preprocessor name
    intForcCombFileName: name of the file containing the forces and bending moments 
                     obtained for each element for the combinations analyzed
    mapSectionsForEveryElement: file containing a dictionary  such that for each                                element of the model stores two names 
                                (for the sections 1 and 2) to be employed 
                                in verifications
  '''
  idElements= set()
  idCombs= set()
  retval= []
  f= open(intForcCombFileName,"r")
  listado= csv.reader(f)
  for lst in listado:
    if(len(lst)>0):
      idComb= lst[0]
      idCombs.add(idComb)
      idElem= eval(lst[1])
      idElements.add(idElem)
  f.close()
   
  nodes= preprocessor.getNodeLoader
  predefined_spaces.gdls_resist_materiales3D(nodes)
  elements= preprocessor.getElementLoader
  coacciones= preprocessor.getConstraintLoader
  # Definimos materiales
  elements.dimElem= 1
  tagsNodesToLoad1= {}
  tagsNodesToLoad2= {}
  for tagElem in idElements:
    nmbScc1= mapSectionsForEveryElement[tagElem][0]
    n1= nodes.newNodeXYZ(0,0,0)
    n3= nodes.newNodeXYZ(0,0,0)
    fix_node_6dof.fixNode6DOF(coacciones,n1.tag)
    tagsNodesToLoad1[tagElem]= n3.tag
    elements.defaultMaterial= nmbScc1
    print "nmbScc1= ", nmbScc1
    e1= elements.newElement("zero_length_section",xc.ID([n1.tag,n3.tag]))
    retval.append(e1)
    e1.setProp("idElem", tagElem) #Elemento que se comprueba
    e1.setProp("dir",1)
    e1.setProp("idSection", nmbScc1) #Section to verify

    nmbScc2= mapSectionsForEveryElement[tagElem][1]
    elements.defaultMaterial= nmbScc2
    print "nmbScc2= ", nmbScc2
    n2= nodes.newNodeXYZ(0,0,0)
    n4= nodes.newNodeXYZ(0,0,0)
    fix_node_6dof.fixNode6DOF(coacciones,n2.tag)
    tagsNodesToLoad2[tagElem]= n4.tag
    e2= elements.newElement("zero_length_section",xc.ID([n2.tag,n4.tag])) 
    retval.append(e2)
    e2.setProp("idElem", tagElem) #Elemento que se comprueba
    e2.setProp("dir",2)
    e2.setProp("idSection", nmbScc2) #Section to verify
 
 
  cargas= preprocessor.getLoadLoader
  casos= cargas.getLoadPatterns
  #Load modulation.
  ts= casos.newTimeSeries("constant_ts","ts")
  casos.currentTimeSeries= "ts"
  #Load case definition
  mapCombs= {}
  for comb in idCombs:
    mapCombs[comb]= casos.newLoadPattern("default",comb)

  f= open(intForcCombFileName,"r")
  listado= csv.reader(f)
  for lst in listado:
    if(len(lst)>0):
      nmbComb= lst[0]
      idElem= eval(lst[1])
      lp= mapCombs[nmbComb]
      tagNode3= tagsNodesToLoad1[idElem]
      '''Adoptamos el método de Wood para evaluar los momentos para dimensionar a flexión'''
      # AXIL_X,Q_X,RASANTE,TORSOR=0,MOM_X*,0.0
      #      2,  8,      4,     7,    5,0.0
      N= eval(lst[2])
      Qx= eval(lst[8])
      R= eval(lst[4])
      T= 0.0
      Mx= eval(lst[5])
      s= float(numpy.sign(Mx))
      Mx+= s*abs(eval(lst[7]))
      My= 0.0
      lp.newNodalLoad(tagNode3,xc.Vector([N,Qx,R,T,Mx,My]))

      tagNode4= tagsNodesToLoad2[idElem]
      # AXIL_Y,Q_Y,RASANTE,TORSOR=0,MOM_Y*,0.0
      #      3,  9,      4,     7,    6,0.0
      N= eval(lst[3])
      Qx= eval(lst[9])
      R= eval(lst[4])
      T= 0.0
      Mx= eval(lst[6])
      s= float(numpy.sign(Mx))
      Mx+= s*abs(eval(lst[7]))
      My= 0.0
      lp.newNodalLoad(tagNode4,xc.Vector([N,Qx,R,T,Mx,My]))

  f.close()
  return retval


