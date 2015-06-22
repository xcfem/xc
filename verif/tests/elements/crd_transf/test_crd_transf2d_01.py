# -*- coding: utf-8 -*-
# home made test
# Verificación del funcionamiento de la función «getPointLocalCoordFromGlobal».

import xc_base
import geom
import xc
from model import predefined_spaces
from model import fix_node_3dof
import math
from materials import typical_materials


L= 20 # Longitud de la barra.

# Problem type

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador   
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXY(0,0)
nod= nodos.newNodeXY(L*math.sqrt(2)/2,L*math.sqrt(2)/2)

    
# Definimos transformaciones geométricas
trfs= mdlr.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")

# Materials
seccion= typical_materials.defElasticSection2d(mdlr,"seccion",1,1,1)

# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "seccion"
#  sintaxis: beam2d_02[<tag>] 
elementos.defaultTag= 1 #Tag for next element.
beam2d= elementos.newElement("elastic_beam_2d",xc.ID([1,2]))
coordTransf= beam2d.getCoordTransf
centroideG= coordTransf.getPointGlobalCoordFromBasic(0.5)
posCentroideG= geom.Pos2d(centroideG[0],centroideG[1])
centroideL= coordTransf.getPointLocalCoordFromGlobal(centroideG)
posCentroideL= geom.Pos2d(centroideL[0],centroideL[1])
centroideB= coordTransf.getPointBasicCoordFromGlobal(centroideG)


centroideGTeor= geom.Pos2d(L*math.sqrt(2)/4,L*math.sqrt(2)/4) # Centroide en globales.
ratio1= posCentroideG.distPos2d(centroideGTeor)
centroideLTeor= geom.Pos2d(10,0) # Centroide en locales.
ratio2= posCentroideL.distPos2d(centroideLTeor)
centroideBTeor= 0.5 # Centroide en básicas.
ratio3= abs(centroideB-centroideBTeor)

''' 
print "centroide en globales: ",centroideG
print "ratio1: ",ratio1
print "centroide en locales: ",centroideL
print "ratio2: ",ratio2
print "centroide en básicas: ",centroideB
print "ratio3: ",ratio3
   '''

import os
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-14) & (ratio3<1e-15)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
