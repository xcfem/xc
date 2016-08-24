# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
import sys
from model import predefined_spaces
from materials import typical_materials

CooMaxX= 2
CooMaxY= 1
E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad
old_stderr= sys.stderr

numNodes= 0
numElem= 0
# Problem type
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
prueba.errFileName= "/tmp/borrar.err" # Para no imprimir mensajes de advertencia.
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader

predefined_spaces.gdls_elasticidad2D(nodes)# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)

nodes.newSeedNode()

elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)

seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast2d"
elem= seedElemLoader.newElement("quad4n",xc.ID([0,0,0,0]))



puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(CooMaxX/2,0.0,0.0))
pt= puntos.newPntIDPos3d(3,geom.Pos3d(CooMaxX,0.0,0.0))
pt= puntos.newPntIDPos3d(4,geom.Pos3d(0.0,CooMaxY,0.0))
pt= puntos.newPntIDPos3d(5,geom.Pos3d(CooMaxX/2,CooMaxY,0.0))
pt= puntos.newPntIDPos3d(6,geom.Pos3d(CooMaxX,CooMaxY,0.0))

surfaces= preprocessor.getCad.getSurfaces
surfaces.defaultTag= 1
s1= surfaces.newQuadSurfacePts(1,2,5,4)
s1.nDivI= 1
s1.nDivJ= 1



#f1= preprocessor.getSets.getSet("f1")
s1.genMesh(xc.meshDir.I)



surfaces= preprocessor.getCad.getSurfaces
surfaces.defaultTag= 2
s2= surfaces.newQuadSurfacePts(2,3,6,5)
s2.nDivI= 2
s2.nDivJ= 2



f2= preprocessor.getSets.getSet("f2")
s2.genMesh(xc.meshDir.I)

total= preprocessor.getSets.getSet("total")
numNodes= total.getNodes.size
numElem= total.getElements.size



''' 
print "Número de nodos: ",numNodes
print "Número de elementos: ",numElem
   '''

ratio1= abs(numNodes-8)
ratio2= abs(numElem-3)

import os
fname= os.path.basename(__file__)
if (ratio1<=1e-10) & (ratio2<=1e-10) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
