# -*- coding: utf-8 -*-
# vector3d_edge_load_global test.
# Home made test.

import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

E= 2.1e6 # Steel Young modulus.
nu= 0.3 # Poisson's coefficient.
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.
L= 2
p= 10

# Problem type
prueba= xc.ProblemaEF()
prep= prueba.getPreprocessor
nodos= prep.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodos.newNodeXYZ(0,0,0)
n2= nodos.newNodeXYZ(L,0,0)
n3= nodos.newNodeXYZ(L,L,0)
n4= nodos.newNodeXYZ(0,L,0)

# Material definition

memb1= typical_materials.defElasticMembranePlateSection(prep,"memb1",E,nu,dens,h)


elementos= prep.getElementHandler
elementos.defaultMaterial= "memb1"
elem= elementos.newElement("shell_mitc4",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))


# Constraints
modelSpace.fixNode000_000(n1.tag)
modelSpace.fixNode000_000(n2.tag)

# Loads definition.
lp0= modelSpace.newLoadPattern(name= '0')

elem.vector3dEdgeLoadGlobal(xc.ID(n3.tag,n4.tag),xc.Vector(0,0,p))

modelSpace.addLoadCaseToDomain(lp0.name)

#Solución
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodos.calculateNodalReactions(True)

RNX1= n1.getReaction[0]
RNY1= n1.getReaction[1]
RNZ1= n1.getReaction[2]
MX1= n1.getReaction[3] 
RNX2= n2.getReaction[0]
RNY2= n2.getReaction[1]
RNZ2= n2.getReaction[2]
MX2= n2.getReaction[3] 




RTeor= -p*L/2
MTeor= RTeor*L

ratio1= (abs((RNX1+RNX2)/RTeor))
ratio2= (abs((RNY1+RNY2)/RTeor))
ratio3= (abs((RNZ1-RTeor)/RTeor))
ratio4= (abs((RNZ2-RTeor)/RTeor))
ratio5= (abs((MX1-MTeor)/MTeor))
ratio6= (abs((MX2-MTeor)/MTeor))



''' 
print "RNX1= ",RNX1
print "RNY1= ",RNY1
print "RNZ1= ",RNZ1
print "MX1= ",MX1
print "ratio1= ",ratio1
print "RNX2= ",RNX2
print "RNY2= ",RNY2
print "RNZ2= ",RNZ2
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
print "ratio5= ",ratio5
print "ratio6= ",ratio6
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-12) & (ratio4 < 1e-12) & (ratio5 < 1e-12) & (ratio6 < 1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

