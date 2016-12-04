# -*- coding: utf-8 -*-
# Tomado del example 2-005 del manual de verificación de SAP 2000

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# nverborrea= 0
NumDivI= 8
NumDivJ= 8
CooMaxX= 2
CooMaxY= 2
E= 17472000 # Elastic modulus en lb/in2
nu= 0.3 # Coeficiente de Poison
G= 6720000
thickness= 0.0001 # Cross section heightexpresado in inches.
unifLoad= 0.0001 # Carga uniforme en lb/in2.
ptLoad= 0.0004 # Carga puntual en lb.
nLoad= unifLoad*CooMaxX*CooMaxY/NumDivI/NumDivJ # Carga tributaria para cada nodo

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader

predefined_spaces.gdls_resist_materiales3D(nodes)
# Definimos materiales
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

nodes.newSeedNode()

# Definimos materiales
nmb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,0.0,thickness)



seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "memb1"
seedElemLoader.defaultTag= 1
elem= seedElemLoader.newElement("shell_mitc4",xc.ID([0,0,0,0]))



puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(CooMaxX,0.0,0.0))
pt= puntos.newPntIDPos3d(3,geom.Pos3d(CooMaxX,CooMaxY,0.0))
pt= puntos.newPntIDPos3d(4,geom.Pos3d(0.0,CooMaxY,0.0))
surfaces= preprocessor.getCad.getSurfaces
surfaces.defaultTag= 1
s= surfaces.newQuadSurfacePts(1,2,3,4)
s.nDivI= NumDivI
s.nDivJ= NumDivJ



f1= preprocessor.getSets.getSet("f1")
f1.genMesh(xc.meshDir.I)
coacciones= preprocessor.getConstraintLoader
lados= s.getEdges
#Edge iterator
for l in lados:
  fix_node_6dof.fixNode6DOFLista(coacciones,l.getEdge.getNodeTags())




# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"


f1= preprocessor.getSets.getSet("f1")
nNodes= f1.getNumNodes
capa1= f1.getNodeLayers.getLayer(0)
nf= capa1.nRow
nc= capa1.nCol
for i in range(2,nf):
  for j in range(2,nc):
    nodo= capa1.getNode(i,j)
    tagNod= nodo.tag
    lp0.newNodalLoad(tagNod,xc.Vector([0,0,-nLoad,0,0,0])) # Carga tributaria



nElems= f1.getNumElements
#We add the load case to domain.
casos.addToDomain("0")


# Procedimiento de solución
analisis= predefined_solutions.simple_static_linear(prueba)
analOk= analisis.analyze(1)

f1= preprocessor.getSets.getSet("f1")

nodes= preprocessor.getNodeLoader

nodo= f1.getNodeIJK(1,NumDivI/2+1,NumDivJ/2+1)
# print "Nodo central: ",nodo.tag
# print "Coordenadas nodo central: ",nodo.getCoo
# print "Movs. nodo central: ",nodo.getDisp
UZ= nodo.getDisp[2]


UZTeor= -1.26
ratio1= (abs((UZ-UZTeor)/UZTeor))
ratio2= (abs((nElems-64)/64))

''' 
print "UZ= ",UZ
print "Num. nodos: ",nNodes
print "Num. elem: ",nElems
print "ratio1: ",ratio1
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<8e-3) & (abs(ratio2)<1e-9):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
