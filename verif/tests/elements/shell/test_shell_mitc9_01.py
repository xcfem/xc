# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT  AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.Ortega@ciccp.es"

E= 2.1e6 # Steel Young's modulus [kg/cm2].
nu= 0.3 # Poisson's ratio.
h= 0.1 # thickness.
dens= 1.33 # specific mass [kg/m2].

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

v1= xc.Vector([0,1,0])
v2= xc.Vector([0,0,1])

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0,0) # first corner nodes counter-clockwise
n2= nodes.newNodeXYZ(1,0,0)
n3= nodes.newNodeXYZ(1,1,0)
n4= nodes.newNodeXYZ(0,1,0)
n5= nodes.newNodeXYZ(0.5,0,0) # mid-side nodes counter-clockwise
n6= nodes.newNodeXYZ(1,0.5,0)
n7= nodes.newNodeXYZ(0.5,1,0)
n8= nodes.newNodeXYZ(0,0.5,0)
n9= nodes.newNodeXYZ(0.5,0.5,0) # central node.

# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)

elements= preprocessor.getElementHandler
elements.defaultMaterial= "memb1"
elem= elements.newElement("ShellMITC9",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag,n5.tag,n6.tag,n7.tag,n8.tag,n9.tag]))

ratio0= (elem.getArea(True)-1)
ratio1= (elem.getCoordTransf.getG2Vector-v1).Norm()
ratio2= (elem.getCoordTransf.getG3Vector-v2).Norm()

''' 
print "area= ",elem.getArea(True)
print "v1= ",v1
print "elem.v1= ", elem.getCoordTransf.getG2Vector
print "ratio1= ",ratio1
print "v2= ",v2
print "elem.v2= ", elem.getCoordTransf.getG3Vector
print "ratio2= ",ratio2
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio0 < 1e-12) & (ratio1 < 1e-12) & (ratio2 < 1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

