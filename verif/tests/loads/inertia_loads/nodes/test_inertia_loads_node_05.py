# -*- coding: utf-8 -*-
''' Test inertia load on nodes. 2D structural mechanics.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials
from scipy.constants import g
from misc_utils import log_messages as lmsg

gravity= g

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Node.
n= modelSpace.newNode(0, 0, 0)
mass= 10.0
templateMatrix= xc.Matrix([[1,0,0,0,0,0],
                           [0,1,0,0,0,0],
                           [0,0,1,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0]])
n.mass= mass*templateMatrix

# Constraints.
modelSpace.fixNode('000_000', n.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
accel= xc.Vector([0, 0, gravity])
n.createInertiaLoad(accel)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)

# Check results.
R= n.getReaction[2]
R_ref= mass*gravity
ratio1= abs(R-R_ref)/R_ref

xcTotalSet= modelSpace.getTotalSet()
totalMassZ= xcTotalSet.getTotalMassComponent(2)
totalWeightZ= totalMassZ*gravity
ratio2= abs(R-totalWeightZ)/R_ref

'''
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-15 and abs(ratio2)<1e-15:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

