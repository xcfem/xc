# -*- coding: utf-8 -*-
# Home made test
# Barra rígida entre dos nodes.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from model import fix_node_6dof
import math


# Geometry
L= 15 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude (kN)

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


# Constraints
coacciones= preprocessor.getConstraintLoader

fix_node_6dof.fixNode6DOF(coacciones,1)
rr= preprocessor.getConstraintLoader.newRigidBeam(1,2)


# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([-F,F,-F,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nod2= nodes.getNode(2)
deltaX= nod2.getDisp[0]
deltaY= nod2.getDisp[1]
deltaZ= nod2.getDisp[2]  # Node 2 displacement


ratio1= math.sqrt(deltaX**2+deltaY**2+deltaZ**2)

''' 
print deltaX
print deltaY
print deltaZ
print ratio1
  '''

import os
fname= os.path.basename(__file__)
if ratio1<1e-9:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
