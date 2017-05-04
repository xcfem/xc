# -*- coding: utf-8 -*-
''' home made test. Verifica el buen comportamiento del
 elemento «ZeroLength» cuando no está alineado con los ejes globales. '''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

K= 1000 # Spring constant
F= 1 # Force magnitude

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_3dof
from materials import typical_materials
import math

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Model definition
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(1,1)
nod= nodes.newNodeXY(1,1)

# Materials definition
k= typical_materials.defElasticMaterial(preprocessor, "k",K)


''' Se definen nodos en los puntos de aplicación de
    la carga. Puesto que no se van a determinar tensiones
    se emplea una sección arbitraria de área unidad '''
    
# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "k"
elementos.dimElem= 3
elementos.deraultTag= 1
zl1= elementos.newElement("zero_length",xc.ID([1,2]))
zl1.setupVectors(xc.Vector([1,1,0]),xc.Vector([-1,1,0]))
zl1.clearMaterials()
zl1.setMaterial(0,"k")
zl2= elementos.newElement("zero_length",xc.ID([1,2]))
zl2.setupVectors(xc.Vector([-1,1,0]),xc.Vector([-1,-1,0]))
zl2.setMaterial(0,"k")

# Constraints
coacciones= preprocessor.getConstraintLoader
spc= coacciones.newSPConstraint(1,0,0.0) # Nodo 1
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(1,2,0.0)
spc= coacciones.newSPConstraint(2,2,0.0) # Nodo 2


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F*math.sin(math.pi/4),F*math.cos(math.pi/4),0]))

#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader
nod2= nodes.getNode(2)
deltax= nod2.getDisp[0]
deltay= nod2.getDisp[1]
deltat= nod2.getDisp[2] 
nod1= nodes.getNode(1)
RX= nod1.getReaction[0]
RY= nod1.getReaction[1]
RT= nod1.getReaction[2] 


R= RX**2+RY**2
d= math.sqrt(deltax**2+deltay**2)
ratio1= R/F-1
ratio2= (K*d)/F-1

''' 
print "R= ",R
print "d= ",d
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
   '''
  
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
