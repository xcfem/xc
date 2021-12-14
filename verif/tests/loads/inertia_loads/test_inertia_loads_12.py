# -*- coding: utf-8 -*-
''' Inertia load on 3D corotational truss elements. 
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
from materials.ec3 import EC3_materials # Steel shapes.
import math


gravity= 9.81 # m/s2

# Material
steelType= EC3_materials.S235JR # steel type for the steel frames

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Geometry.
l= 1.0 # Bar length
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(l,0,0)

# Materials definition
diameter= 12.0e-3
area= math.pi*(diameter/2)**2
trussMat= steelType.defElasticMaterial(preprocessor)


# Element definition.
elements= preprocessor.getElementHandler
elements.dimElem= 3 # Bidimensional space.
elements.defaultMaterial= trussMat.name
truss= elements.newElement("CorotTruss",xc.ID([n1.tag,n2.tag]))
truss.sectionArea= area

## Whole model mass data.
xcTotalSet= modelSpace.getTotalSet()
massZ= xcTotalSet.getTotalMassComponent(2)
massRefZ= truss.linearRho*l
ratio0= abs(massZ-massRefZ)/massRefZ

# Constraints
constraints= preprocessor.getBoundaryCondHandler
# Zero movement for node 1.
modelSpace.fixNode("000_000", n1.tag)
# Zero movement for node 2.
modelSpace.fixNode("000_000", n2.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
accel= xc.Vector([0,0,gravity])
truss.createInertiaLoad(accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

R= n2.getReaction[2]
R_ref= 0.5*truss.linearRho*l*gravity
ratio1= abs(R-R_ref)/(-R_ref)
R_ref2= 0.5*l*area*steelType.rho*gravity
ratio2= abs(R-R_ref2)/(-R_ref2)

'''
print('mass: ', massZ, 'kg')
print('reference mass: ', massRefZ, 'kg')
print('ratio0= ', ratio0)
print('R= ', R)
print('R_ref= ', R_ref)
print('ratio1= ', ratio1)
print('R_ref2= ', R_ref2)
print('weight: ', 2*R/l,' kg/m')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio0)<1e-12) and (abs(ratio1)<1e-12) and (abs(ratio2)<1e-12)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

