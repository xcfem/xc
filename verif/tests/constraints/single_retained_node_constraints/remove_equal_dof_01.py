# -*- coding: utf-8 -*-
''' Verification of removeSPConstraint method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Geometry
L= 1.5 # Bar length (m)
A= 0.01**2 # Cross section area (m2)

# Load
F= 1.5e3 # Load magnitude en N

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messages
n1= nodes.newNodeXY(0,0.0)
n2= nodes.newNodeXY(L,0.0)
n3= nodes.newNodeXY(L,0.0)

# Materials definition
E= 210e9 # Young modulus of the steel.
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

# Elements definition
elements= preprocessor.getElementHandler
elements.dimElem= 2 #Bars defined in a two dimensional space.
elements.defaultMaterial= elast.name
truss1= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
truss1.sectionArea= A
truss2= elements.newElement("Truss",xc.ID([n1.tag,n3.tag]))
truss2.sectionArea= A


# Constraints
modelSpace.fixNode('00', n1.tag) # no displacement.
modelSpace.fixNode('F0', n2.tag) # no uY displacement.
modelSpace.fixNode('F0', n3.tag) # no uY displacement.
eqDOFc= modelSpace.newEqualDOF(n2.tag, n3.tag, dofs= xc.ID([0]))

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([F,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)


nodes= preprocessor.getNodeHandler 
delta0_n2= n2.getDisp[0] # Node 2 uX displacement
delta0_n2_ref= F/E/(2*A)*L
ratio1= abs(delta0_n2-delta0_n2_ref)/delta0_n2_ref
delta0_n3= n3.getDisp[0] # Node 3 uX displacement
ratio2= abs(delta0_n3-delta0_n2_ref)/delta0_n2_ref

# Remove the equal dof constraint.
modelSpace.removeMFreedom_Constraint(eqDOFc.tag)

# Solve again.
result= modelSpace.analyze(calculateNodalReactions= True)
 
delta1_n2= n2.getDisp[0]  # Node 2 uX displacement
delta1_n2_ref= 2*delta0_n2_ref
ratio3= abs(delta1_n2-delta1_n2_ref)/delta1_n2_ref
delta1_n3= n3.getDisp[0]  # Node 2 uX displacement
ratio4= abs(delta1_n3)

'''
print(delta0_n2*1e3, delta0_n2_ref*1e3, ratio1)
print(delta0_n2*1e3, delta0_n2_ref*1e3, ratio2)
print(delta1_n2*1e3, delta1_n2_ref*1e3, ratio3)
print(delta1_n3*1e3, 0, ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((abs(ratio1)<1e-7) & (abs(ratio2)<1e-7) & (abs(ratio3)<1e-7) & (abs(ratio4)<1e-10)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
