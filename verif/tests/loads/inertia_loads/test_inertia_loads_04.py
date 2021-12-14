# -*- coding: utf-8 -*-
''' Inertia load on 2D corotational truss elements. 
    Equilibrium based home made test.'''

from __future__ import division
from __future__ import print_function

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
l= 10 # Bar length
b= 0.1
A= b*b #Área in square inches.
rho= 10.0 # Material density.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l,0)

# Materials definition
trussScc= typical_materials.defElasticSection1d(preprocessor, "trussScc",A,E, linearRho= rho*A)
refLinearRho= rho*A


# Element definition.
elements= preprocessor.getElementHandler
elements.dimElem= 2 # Bidimensional space.
elements.defaultMaterial= trussScc.name
truss= elements.newElement("CorotTrussSection",xc.ID([n1.tag,n2.tag]))

## Whole model mass data.
xcTotalSet= modelSpace.getTotalSet()
massZ= xcTotalSet.getTotalMassComponent(1)
massRefZ= refLinearRho*l
ratio0= abs(massZ-massRefZ)/massRefZ

# Constraints
constraints= preprocessor.getBoundaryCondHandler
# Zero movement for node 1.
modelSpace.fixNode000(n1.tag)
# Zero movement for node 2.
modelSpace.fixNode000(n2.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
accel= xc.Vector([0,9.81])
truss.createInertiaLoad(accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

R= n2.getReaction[1]
R_ref= 0.5*truss.linearRho*l*9.81

ratio1= abs(R-R_ref)/(-R_ref)

'''
print('mass: ', massZ, 'kg')
print('reference mass: ', massRefZ, 'kg')
print('ratio0= ', ratio0)
print('R= ', R)
print('R_ref= ', R_ref)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio0)<1e-12 and abs(ratio1)<1e-12 :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

