# -*- coding: utf-8 -*-
# home made test
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials
import math

E= 2.1e6 # Módulo de Young del acero en kg/cm2.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Espesor.
dens= 1.33 # Densidad kg/m2.

v1= xc.Vector([0,math.sqrt(2)/2,math.sqrt(2)/2])
v2= xc.Vector([0,-math.sqrt(2)/2,math.sqrt(2)/2])
C= xc.Vector([1,0.5,0.5])

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.newNodeIDXYZ(1,0,0,0)
nodos.newNodeIDXYZ(2,2,0,0)
nodos.newNodeIDXYZ(3,2,1,1)
nodos.newNodeIDXYZ(4,0,1,1)


# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)


elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "memb1"
elem= elementos.newElement("corot_shell_mitc4",xc.ID([1,2,3,4]))

ratio1= (elem.getCoordTransf.getG2Vector-v1).Norm()
ratio2= (elem.getCoordTransf.getG3Vector-v2).Norm()
ratio3= (elem.getCoordTransf.getVPosCentroide-C).Norm()




''' 
print "v1= ",v1
print "v2= ",v2
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
 '''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

