# -*- coding: utf-8 -*-
# home made test

E= 2.1e6 # Módulo de Young del acero en kg/cm2.
nu= 0.3 # Coeficiente de Poisson.
h= 0.1 # Espesor.
dens= 1.33 # Densidad kg/m2.

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

r= 1.0/math.sqrt(5)
v1= xc.Vector([2*r,r,0])
v2= xc.Vector([-r,2*r,0])
v3= xc.Vector([0,0,1])

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader

# Problem type
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.newNodeIDXYZ(1,0,0,0)
nodos.newNodeIDXYZ(2,1,0,0)
nodos.newNodeIDXYZ(3,1,1,0)
nodos.newNodeIDXYZ(4,0,1,0)

# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(mdlr,"memb1",E,nu,dens,h)

elementos= mdlr.getElementLoader
elementos.defaultMaterial= "memb1"
elem= elementos.newElement("shell_mitc4",xc.ID([1,2,3,1]))

g1= elem.getCoordTransf.getG1Vector
g2= elem.getCoordTransf.getG2Vector
g3= elem.getCoordTransf.getG3Vector
ratio1= (g1-v1).Norm()
ratio2= (g2-v2).Norm()
ratio3= (g3-v3).Norm()

''' 
print "g1= ",g1
print "g2= ",g2
print "g3= ",g3
print "v1= ",v1
print "v2= ",v2
print "v3= ",v3
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
 '''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

