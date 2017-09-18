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

v1= xc.Vector([0,math.sqrt(2)/2,math.sqrt(2)/2])
v2= xc.Vector([0,-math.sqrt(2)/2,math.sqrt(2)/2])
C= xc.Vector([1,0.5,0.5])

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
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
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

