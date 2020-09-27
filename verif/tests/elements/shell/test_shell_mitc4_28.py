# -*- coding: utf-8 -*-
from __future__ import print_function

'''Verification of resetLoadCase method.'''

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

E= 2.1e6 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.

q= -2

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.newNodeIDXYZ(1,0,0,0)
nodes.newNodeIDXYZ(2,1,0,0)
nodes.newNodeIDXYZ(3,1,1,0)
nodes.newNodeIDXYZ(4,0,1,0)


# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)


elements= preprocessor.getElementHandler
elements.defaultMaterial= memb1.name
elem= elements.newElement("ShellMITC4",xc.ID([1,2,3,4]))


# Constraints
constraints= preprocessor.getBoundaryCondHandler

modelSpace.fixNode000_FFF(1)
modelSpace.fixNode000_FFF(2)
modelSpace.fixNode000_FFF(3)
modelSpace.fixNode000_FFF(4)


# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("shell_uniform_load")
eleLoad.elementTags= xc.ID([0]) 
eleLoad.transComponent= q 
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= False)

preprocessor.resetLoadCase()
nodes.calculateNodalReactions(True,1e-7)

RN1= nodes.getNode(1).getReaction[2] 
RN2= nodes.getNode(2).getReaction[2] 
RN3= nodes.getNode(3).getReaction[2] 
RN4= nodes.getNode(4).getReaction[2] 

ratio1= (abs((RN1)))
ratio2= (abs((RN2)))
ratio3= (abs((RN3)))
ratio4= (abs((RN4)))



'''
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-12) & (ratio4 < 1e-12):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

