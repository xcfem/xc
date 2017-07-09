# -*- coding: utf-8 -*-
# home made test
# Verificación del funcionamiento de la función «getPointLocalCoordFromGlobal».

from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 20 # Bar length.

# Problem type

centroideG= None # Centroide en globales.
centroideL= None # Centroide en locales.
centroideB= None # Centroide en básicas.

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0,0)
nod= nodes.newNodeXYZ(L*math.sqrt(3)/3,L*math.sqrt(3)/3,L*math.sqrt(3)/3)

# Materials
seccion= typical_materials.defElasticSection3d(preprocessor, "seccion",1,1,1,1,1,1)
    
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,-1,0])
    
# Elements definition
elements= preprocessor.getElementLoader

elements.defaultTransformation= "lin"
elements.defaultMaterial= "seccion"
elements.defaultTag= 1 #Tag for next element.
beam3d= elements.newElement("elastic_beam_3d",xc.ID([1,2]));

centroideG= beam3d.getCoordTransf.getPointGlobalCoordFromBasic(0.5)
posCentroideG= geom.Pos3d(centroideG[0],centroideG[1],centroideG[2])
centroideL= beam3d.getCoordTransf.getPointLocalCoordFromGlobal(centroideG)
posCentroideL= geom.Pos2d(centroideL[0],centroideL[1])
centroideB= beam3d.getCoordTransf.getPointBasicCoordFromGlobal(centroideG)

centroideGTeor= geom.Pos3d(L*math.sqrt(3)/6,L*math.sqrt(3)/6,L*math.sqrt(3)/6) # Centroide en globales.
ratio1= posCentroideG.distPos3d(centroideGTeor)
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
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-14) & (ratio3<1e-15)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
