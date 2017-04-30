# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

E= 2.1e6 # Young modulus of the steel en kg/cm2.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Espesor.
dens= 1.33 # Densidad kg/m2.

mov= xc.Vector([1,1,1,0,0,0])

# Problem type
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Ignore warning messages
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodes)
nodes.newNodeIDXYZ(1,0,0,0)
nodes.newNodeIDXYZ(2,2,0,0)
nodes.newNodeIDXYZ(3,2,1,1)
nodes.newNodeIDXYZ(4,0,1,1)

# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)
elementos= preprocessor.getElementLoader

elementos.defaultMaterial= "memb1"
elem= elementos.newElement("corot_shell_mitc4",xc.ID([1,2,3,4]))

nodes= preprocessor.getSets.getSet("total").getNodes
for n in nodes:
  n.setTrialDisp(mov)

movLocalN1= None
movLocalN2= None
movLocalN3= None
movLocalN4= None

elementos= preprocessor.getSets.getSet("total").getElements
for e in elementos:
  crdT= e.getCoordTransf
  crdT.update()
  R0= crdT.getR0
  R= crdT.getR
  movLocalN1= crdT.getBasicTrialDisp(0)
  movLocalN2= crdT.getBasicTrialDisp(1)
  movLocalN3= crdT.getBasicTrialDisp(2)
  movLocalN4= crdT.getBasicTrialDisp(3)


ratio1= (movLocalN1-movLocalN2).Norm()
ratio2= (movLocalN1-movLocalN3).Norm()
ratio3= (movLocalN1-movLocalN4).Norm()

''' 
print "mov= ",mov
print "matriz R0= ",R0
print "matriz R= ",R
print "mov. local nodo 1: ",movLocalN1
print "mov. local nodo 2: ",movLocalN2
print "mov. local nodo 3: ",movLocalN3
print "mov. local nodo 4: ",movLocalN4
print "ratio1: ",ratio1
print "ratio2: ",ratio2
print "ratio3: ",ratio3
   '''

import os
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-15) & (ratio3<1e-15)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

