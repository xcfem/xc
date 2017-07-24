# -*- coding: utf-8 -*-
# Tomado del example 2-005 del manual de verificación de SAP 2000

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# nverborrea= 0
NumDivI= 32
NumDivJ= 32
CooMaxX= 10
CooMaxY= 2
E= 17472000 # Elastic modulus en lb/in2
nu= 0.3 # Poisson's ratio
G= 6720000
thickness= 0.0001 # Cross section depth expressed in inches.
unifLoad= 0.0001 # Uniform load in lb/in2.
ptLoad= 0.0004 # Punctual load in lb.
nLoad= unifLoad*CooMaxX*CooMaxY/NumDivI/NumDivJ # Tributary load on each node

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Define materials
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

nodes.newSeedNode()

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

# Constraints
f1= preprocessor.getSets.getSet("f1")
f1.genMesh(xc.meshDir.I)
lados= s.getEdges
#Edge iterator
for l in lados:
  for i in l.getEdge.getNodeTags():
    modelSpace.fixNode000_000(i)

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
    node= capa1.getNode(i,j)
    lp0.newNodalLoad(node.tag,xc.Vector([0,0,-nLoad,0,0,0])) # Carga concentrada



nElems= f1.getNumElements
#We add the load case to domain.
casos.addToDomain("0")


# Solution procedure
analisis= predefined_solutions.simple_static_linear(prueba)
analOk= analisis.analyze(1)

f1= preprocessor.getSets.getSet("f1")

nodes= preprocessor.getNodeLoader

node= f1.getNodeIJK(1,NumDivI/2+1,NumDivJ/2+1)
# print "Central node: ", node.tag
# print "Central node coordinates: ", node.getCoo
# print "Central node displacements: ", node.getDisp
UZ= node.getDisp[2]


UZTeor= -2.6
ratio1= (abs((UZ-UZTeor)/UZTeor))
ratio2= (abs((nElems-1024)/1024))

''' 
print "UZ= ",UZ
print "Number of nodes: ",nNodes
print "Number of elements: ",nElems
print "ratio1: ",ratio1
   '''
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<3e-3) & (abs(ratio2)<1e-9):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
