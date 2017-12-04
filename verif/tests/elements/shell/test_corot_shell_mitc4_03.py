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
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.

mov= xc.Vector([1,1,1,0,0,0])

# Problem type
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/borrar.log" # Ignore warning messages
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.newNodeIDXYZ(1,0,0,0)
nodes.newNodeIDXYZ(2,2,0,0)
nodes.newNodeIDXYZ(3,2,1,1)
nodes.newNodeIDXYZ(4,0,1,1)

# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)
elements= preprocessor.getElementLoader

elements.defaultMaterial= "memb1"
elem= elements.newElement("CorotShellMITC4",xc.ID([1,2,3,4]))

nodes= preprocessor.getSets.getSet("total").getNodes
for n in nodes:
  n.setTrialDisp(mov)

movLocalN1= None
movLocalN2= None
movLocalN3= None
movLocalN4= None

elements= preprocessor.getSets.getSet("total").getElements
for e in elements:
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
print "displacement of node 1 in local coordinate system: ",movLocalN1
print "displacement of node 2 in local coordinate system: ",movLocalN2
print "displacement of node 3 in local coordinate system: ",movLocalN3
print "displacement of node 4 in local coordinate system: ",movLocalN4
print "ratio1: ",ratio1
print "ratio2: ",ratio2
print "ratio3: ",ratio3
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-15) & (ratio3<1e-15)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

