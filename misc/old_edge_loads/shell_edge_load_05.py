# -*- coding: utf-8 -*-
# vector3d_edge_load_global test (home made).
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
n1= nodos.newNodeXYZ(1,0,0,0)
n2= nodos.newNodeXYZ(2,L,0,0)
n3= nodos.newNodeXYZ(3,L,L,0)
n4= nodos.newNodeXYZ(4,0,L,0)

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
# Define load pattern
lp0= loadPatterns.newLoadPattern("default","0")
loadPatterns.currentLoadPattern= lp0.name


elements= prep.getElementHandler
for e in elements:
    e.vector3dEdgeLoadGlobal(xc.ID(1,2),xc.Vector(0,p/2,0))
    e.vector3dEdgeLoadGlobal(xc.ID(2,1),xc.Vector(0,p/2,0))
    
lp0.addToDomain()


# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodos.calculateNodalReactions(True)
RN1= n1.getReaction[1] 
RN2= n2.getReaction[1] 
RN3= n3.getReaction[1] 
RN4= n4.getReaction[1] 

RTeor= -p*L/2

ratio1= (abs((RN1-RTeor)/RTeor))
ratio2= (abs((RN2-RTeor)/RTeor))
ratio3= (abs((RN3)))
ratio4= (abs((RN4)))

''' 
print "RN1= ",RN1
print "ratio1= ",ratio1
print "RN2= ",RN2
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
 '''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-12) & (ratio4 < 1e-12):
    print "test ",fname,": ok."
else:
    print "test ",fname,": ERROR."

