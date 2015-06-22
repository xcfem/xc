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

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader

# Problem type
predefined_spaces.gdls_resist_materiales3D(nodos)
delta= 10.1
nodos.newNodeIDXYZ(1,0+delta,0+delta,0)
nodos.newNodeIDXYZ(2,1+delta,0+delta,0)
nodos.newNodeIDXYZ(3,1+delta,1+delta,0)
nodos.newNodeIDXYZ(4,0+delta,1+delta,0)

# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(mdlr,"memb1",E,nu,dens,h)

elementos= mdlr.getElementLoader
elementos.defaultMaterial= "memb1"
elem= elementos.newElement("shell_mitc4",xc.ID([1,2,3,4]))

O= elem.getCoordTransf.getLocalCoordinates(geom.Pos3d(0.5+delta,0.5+delta,10))
p1= elem.getCoordTransf.getLocalCoordinates(geom.Pos3d(0+delta,0+delta,5))
p2= elem.getCoordTransf.getLocalCoordinates(geom.Pos3d(1+delta,0+delta,25))
p3= elem.getCoordTransf.getLocalCoordinates(geom.Pos3d(1+delta,1+delta,3))
p4= elem.getCoordTransf.getLocalCoordinates(geom.Pos3d(0+delta,1+delta,-3))
ratio1= abs(O.x)
ratio2= abs(O.y)

# print O
# print p1
# print p2
# print p3
# print p4
# print "ratio1= ",ratio1
# print "ratio2= ",ratio2


import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

