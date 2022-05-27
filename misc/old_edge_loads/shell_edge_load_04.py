# -*- coding: utf-8 -*-
# ector3d_edge_load_global test (home made).
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

E= 2.1e6 # Steel Young modulus.
nu= 0.3 # Poisson's coefficient.
h= 0.1 # Espesor.
dens= 1.33 # Density kg/m2.
L= 2
p= 10

# Problem type
prueba= xc.ProblemaEF()
prep= prueba.getPreprocessor
nodos= prep.getNodeHandler
predefined_spaces.gdls_resist_materials3D(nodos)
n1= nodos.newNodeXYZ(0,0,0)
n2= nodos.newNodeXYZ(L,0,0)
n3= nodos.newNodeXYZ(L,L,0)
n4= nodos.newNodeXYZ(0,L,0)


# Material definition
memb1= typical_materials.defElasticMembranePlateSection(prep,"memb1",E,nu,dens,h)


elementos= prep.getElementHandler
elementos.defaultMaterial= "memb1"
elem= elementos.newElement("shell_mitc4",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))


# Condiciones de contorno
coacciones= prep.getBoundaryCondHandler

fix_node_6dof.fixNode6DOF(coacciones,n1.tag)
fix_node_6dof.fixNode6DOF(coacciones,n2.tag)


# Define loads
loads= prep.getLoadHandler

loadPatterns= loads.getLoadPatterns

# Load modulation.
ts= loadPatterns.newTimeSeries("constant_ts","ts")
loadPatterns.currentTimeSeries= "ts"
# Define load pattern.
lp0= loadPatterns.newLoadPattern("default","0")
loadPatterns.currentLoadPattern= "0"

elements= prep.getElementHandler
for e in elements: 
    e.vector3dEdgeLoadGlobal(xc.ID(3,2),xc.Vector(0,p,0))
    e.vector3dEdgeLoadGlobal(xc.ID(4,1),xc.Vector(0,p,0))


lp0.addToDomain()


#Solución
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodos.calculateNodalReactions(True)

n1Reaction= n1.getReaction
RNX1= n1Reaction[0]
RNY1= n1Reaction[1]
RNZ1= n1Reaction[2]
MX1= n1Reaction[3] 
n2Reaction= n2.getReaction
RNX2= n2Reaction[0]
RNY2= n2Reaction[1]
RNZ2= n2Reaction[2]
MX2= n3Reaction[3] 

RTeor= -p*L

ratio1= (abs((RNX1+RNX2)/RTeor))
ratio2= (abs((RNZ1+RNZ2)/RTeor))
ratio3= (abs((RNY1-RTeor)/RTeor))
ratio4= (abs((RNY2-RTeor)/RTeor))


''' 
print "RNX1= ",RNX1
print "RNY1= ",RNY1
print "RNZ1= ",RNZ1
print "MX1= ",MX1
print "RNX2= ",RNX2
print "RNY2= ",RNY2
print "RNZ2= ",RNZ2
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
 '''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-12) & (ratio4 < 1e-12) :
    print "test ",fname,": ok."
else:
    print "test ",fname,": ERROR."

