# -*- coding: utf-8 -*-

# Home made test
# Barra rígida entre dos nodos.

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
F= 1.5e3 # Magnitud de la carga en kN

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador   
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_elasticidad3D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXYZ(0,0.0,0.0)
nod= nodos.newNodeXYZ(L,0.0,0.0)


# Constraints
coacciones= mdlr.getConstraintLoader

fix_node_6dof.Nodo6DOFGirosLibres(coacciones, 1)
rr= mdlr.getConstraintLoader.newRigidRod(1,2)


# Loads definition
cargas= mdlr.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([-F,F,-F]))
#We add the load case to domain.
casos.addToDomain("0")


# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodos= mdlr.getNodeLoader
 
nod2= nodos.getNode(2)
deltaX= nod2.getDisp[0]
deltaY= nod2.getDisp[0]
deltaZ= nod2.getDisp[0]  # Desplazamiento del nodo 2


ratio1= math.sqrt(deltaX**2+deltaY**2+deltaZ**2)

''' 
    print deltaX
    print deltaY
    print deltaZ
    print ratio1
   '''

import os
fname= os.path.basename(__file__)
if ratio1<1e-11:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
