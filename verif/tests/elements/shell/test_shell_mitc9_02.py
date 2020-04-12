# -*- coding: utf-8 -*-
# home made test

from __future__ import division
from __future__ import print_function

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
from solution import predefined_solutions

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

# Constraints.
modelSpace.fixNode000_FFF(n1.tag)
modelSpace.fixNode000_FFF(n2.tag)
modelSpace.fixNode000_FFF(n3.tag)
modelSpace.fixNode000_FFF(n4.tag)
modelSpace.fixNode000_FFF(n5.tag)
modelSpace.fixNode000_FFF(n6.tag)
modelSpace.fixNode000_FFF(n7.tag)
modelSpace.fixNode000_FFF(n8.tag)

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
F= -10*8
lp0.newNodalLoad(n9.tag,xc.Vector([0,0,F,0,0,0]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)
RN1= n1.getReaction[2] 
RN2= n2.getReaction[2] 
RN3= n3.getReaction[2] 
RN4= n4.getReaction[2] 
RN5= n5.getReaction[2] 
RN6= n6.getReaction[2] 
RN7= n7.getReaction[2] 
RN8= n8.getReaction[2]
R= RN1+RN2+RN3+RN4+RN5+RN6+RN7+RN8

ratio0= F+R
# Symmetry
ratio1= (RN1-RN2)**2+(RN1-RN3)**2+(RN1-RN4)**2
ratio2= (RN5-RN6)**2+(RN5-RN7)**2+(RN5-RN8)**2
# Reaction values.
ratio3= RN1-0.019444184*F
ratio4= RN5+0.269444184*F

''' 
print("RN1= ", RN1)
print("RN2= ", RN2)
print("RN3= ", RN3)
print("RN4= ", RN4)
print("RN5= ", RN5)
print("RN6= ", RN6)
print("RN7= ", RN7)
print("RN8= ", RN8)
print("R= ", R)
print("ratio0= ",ratio0)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("ratio34= ",ratio3+ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio0 < 1e-11) & (ratio1 < 1e-15) & (ratio2 < 1e-15) & (ratio3 < 1e-6) & (ratio4 < 1e-6):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

