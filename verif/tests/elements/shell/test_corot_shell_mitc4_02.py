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
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import math

E= 2.1e6 # Young modulus of the steel en kg/cm2.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.

angRotacion= math.radians(10)
rotacion= xc.Matrix([[1,0,0],[0,math.cos(angRotacion),math.sin(angRotacion)],[0,-math.sin(angRotacion),math.cos(angRotacion)]])


# Problem type
prb= xc.ProblemaEF()
prb.logFileName= "/tmp/borrar.log" # Ignore warning messages
preprocessor=  prb.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod1= nodes.newNodeIDXYZ(1,0,0,0)
nod2= nodes.newNodeIDXYZ(2,2,0,0)
nod3= nodes.newNodeIDXYZ(3,2,1,1)
p= nod3.getInitialPos3d
vPosN3= xc.Vector([p.x,p.y,p.z])
nod4= nodes.newNodeIDXYZ(4,0,1,1)
p= nod4.getInitialPos3d
vPosN4= xc.Vector([p.x,p.y,p.z])


movN3= rotacion*vPosN3-vPosN3
movN4= rotacion*vPosN4-vPosN4
# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)
elements= preprocessor.getElementLoader
elements.defaultMaterial= "memb1"
elem= elements.newElement("CorotShellMITC4",xc.ID([1,2,3,4]))

nod3.setTrialDisp(xc.Vector([movN3[0],movN3[1],movN3[2],0,0,0]))
nod4.setTrialDisp(xc.Vector([movN4[0],movN4[1],movN4[2],0,0,0]))

crd_transf= elem.getCoordTransf
crd_transf.update()
R0= crd_transf.getR0
R= crd_transf.getR
movLocalN1= crd_transf.getBasicTrialDisp(0)
movLocalN2= crd_transf.getBasicTrialDisp(1)
movLocalN3= crd_transf.getBasicTrialDisp(2)
movLocalN4= crd_transf.getBasicTrialDisp(3)




ratio1= (movLocalN1-movLocalN2).Norm()
ratio2= (movLocalN3-movLocalN4).Norm()

''' 
print "vPosN3= ",vPosN3
print "movN3= ",movN3
print "vPosN4= ",vPosN4
print "movN4= ",movN4
print "matriz R0= ",R0
print "matriz R= ",R
print "displacement of node 1 in local coordinate system: ",movLocalN1
print "displacement of node 2 in local coordinate system: ",movLocalN2
print "displacement of node 3 in local coordinate system: ",movLocalN3
print "displacement of node 4 in local coordinate system: ",movLocalN4
print "ratio1: ",ratio1
print "ratio2: ",ratio2
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-15)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

