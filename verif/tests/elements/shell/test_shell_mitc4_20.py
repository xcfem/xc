# -*- coding: utf-8 -*-
# Tomado del control número 1 que figura en el apartado 6.7 del libro
# «Los forjados reticulares: diseño, análisis, construcción y patología»
# de Florentino Regalado Tesoro.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from model import cargas_nodo
from materials import typical_materials
import math

CooMaxX= 10
CooMaxY= 10
xMidP= CooMaxX/2
yMidP= CooMaxY/2
xMidL= xMidP
yMidL= CooMaxY
NumDivI= int(math.ceil(CooMaxX/0.25))
NumDivJ= int(math.ceil(CooMaxY/0.25))
E= 2.1e10 # Elastic modulus en N/m2
nu= 0.0 # Coeficiente de Poison
G= E/2/(1+nu)
thickness= 0.2 # Cross section depth expressed in meters.
unifLoad= 20e3 # Carga uniforme en N/m2.
nLoad= unifLoad*CooMaxX*CooMaxY/NumDivI/NumDivJ # Carga tributaria para cada nodo

tagElemCentro= 0
tagElemLado= 0

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
nodes= preprocessor.getNodeLoader

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.newSeedNode()


# Define materials
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

# Define materials
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


coacciones= preprocessor.getConstraintLoader

f1= preprocessor.getSets.getSet("f1")
f1.genMesh(xc.meshDir.I)
lados= s.getEdges
#Edge iterator
for l in lados:
  fix_node_6dof.fixNode6DOFLista(coacciones,l.getEdge.getNodeTags())


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
ts= casos.newTimeSeries("constant_ts","ts")
# \constant_ts["ts"]{ \factor{1.0} } # Time series: constant_ts[nombre]{factor}
casos.currentTimeSeries= "ts"
lp0= casos.newLoadPattern("default","0") 
#casos.currentLoadPattern= "0"

cargas_nodo.CargaNodosInterioresCara(f1,lp0,[0,0,-nLoad,0,0,0])
f1= preprocessor.getSets.getSet("f1")

tagElemCentro= f1.getNearestElement(geom.Pos3d(xMidP,yMidP,0.0)).tag
tagElemLado= f1.getNearestElement(geom.Pos3d(xMidL,yMidL,0.0)).tag
nodes= preprocessor.getNodeLoader

nNodes= f1.getNumNodes

nElems= f1.getNumElements
#We add the load case to domain.
casos.addToDomain("0")


# Procedimiento de solución
analisis= predefined_solutions.simple_static_linear(prueba)
analOk= analisis.analyze(1)

m1Centro= 0.0
m2CentroLado= 0.0
f1= preprocessor.getSets.getSet("f1")

nodes= preprocessor.getNodeLoader

nodo= f1.getNodeIJK(1,NumDivI/2+1,NumDivJ/2+1)

# \print{"Nodo central: ",tag
# print "Coordenadas nodo central: ",coord
# print "Movs. nodo central: ",nodo.getDisp
UZ= nodo.getDisp[2]


elemCentro= preprocessor.getElementLoader.getElement(tagElemCentro)
elemCentro.getResistingForce()
m1Centro= elemCentro.getMeanInternalForce("m1")
elemLado= preprocessor.getElementLoader.getElement(tagElemLado)
elemLado.getResistingForce()
m2CentroLado= elemLado.getMeanInternalForce("m2")

UZTeor= -1.821e-2
ratio1= (abs((UZ-UZTeor)/UZTeor))
ratio2= (abs((nElems-1600)/1600))
ratio3= (abs((m1Centro+35.20e3)/35.20e3))
ratio4= (abs((m2CentroLado-103.09e3)/103.09e3))

''' 
print "tagElemCentro= ",tagElemCentro
print "tagElemLado= ",tagElemLado
print "UZ= ",UZ
print "m1Centro= ",m1Centro/1e3," kN \n"
print "m2CentroLado= ",m2CentroLado/1e3," kN \n"
print "Num. nodos: ",nNodes
print "Num. elem: ",nElems
print "ratio1: ",ratio1
print "ratio2: ",ratio2
print "ratio3: ",ratio3
print "ratio4: ",ratio4
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<0.5e-2) & (abs(ratio2)<1e-9) & (abs(ratio3)<2e-4) & (abs(ratio4)<0.11):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
