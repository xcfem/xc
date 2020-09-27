# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

# Mome made test.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math


E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
l= 10 # Bar length
b= 0.1
A= b*b #Área in square inches.
Iz= 1/12.0*b**4 # Cross section moment of inertia (m4)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# nodes.defaultTag= 1 # First node number.
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(l,0,0)

# Materials definition
trussScc= typical_materials.defElasticSection1d(preprocessor, "trussScc",A,E, linearRho= 10.0*A)


# Element definition.
elements= preprocessor.getElementHandler
elements.dimElem= 3 # Three-dimensional space.
elements.defaultMaterial= trussScc.name
truss= elements.newElement("TrussSection",xc.ID([n1.tag,n2.tag]))

constraints= preprocessor.getBoundaryCondHandler
# Zero movement for node 1.
spc1= constraints.newSPConstraint(n1.tag,0,0.0)
spc2= constraints.newSPConstraint(n1.tag,1,0.0)
spc3= constraints.newSPConstraint(n1.tag,2,0.0)
spc4= constraints.newSPConstraint(n1.tag,3,0.0)
spc5= constraints.newSPConstraint(n1.tag,4,0.0)
spc6= constraints.newSPConstraint(n1.tag,5,0.0)
# Zero movement for node 2.
spc7= constraints.newSPConstraint(n2.tag,0,0.0)
spc8= constraints.newSPConstraint(n2.tag,1,0.0)
spc9= constraints.newSPConstraint(n2.tag,2,0.0)
spc10= constraints.newSPConstraint(n2.tag,3,0.0)
spc11= constraints.newSPConstraint(n2.tag,4,0.0)
spc12= constraints.newSPConstraint(n2.tag,5,0.0)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
accel= xc.Vector([0,9.81,0])
truss.createInertiaLoad(accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

R= n2.getReaction[1]
R_ref= 0.5*truss.linearRho*l*9.81

ratio1= abs(R-R_ref)/(-R_ref)

'''
print('R= ', R)
print('R_ref= ', R_ref)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 :
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

