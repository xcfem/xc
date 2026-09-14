# -*- coding: utf-8 -*-
'''Compute the center of mass of a set.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
import geom
from model import predefined_spaces

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics1D(nodeHandler)
m= 1e3
nodeMassMatrix= xc.Matrix([[m]])

nodes= list()
refCenterOfMass= 0.0
for x in range(0, 10):
    refCenterOfMass+= x
    newNode= modelSpace.newNode(x)
    newNode.mass= nodeMassMatrix
    nodes.append(newNode)
refCenterOfMass/=len(nodes)

# Check results.
xcTotalSet= modelSpace.getTotalSet()
totalMass= xcTotalSet.totalMass(0,0)
refTotalMass= len(nodes)*m
ratio1= abs(totalMass-refTotalMass)/refTotalMass
centerOfMass= xcTotalSet.getCenterOfMass(True).x
ratio2= abs(centerOfMass-refCenterOfMass)/refCenterOfMass

# print("totalMass= ", totalMass)
# print("ratio1= ", ratio1)
# print("centerOfMass= ", centerOfMass)
# print("refCenterOfMass= ", refCenterOfMass)
# print("ratio2= ", ratio2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-15) and (ratio2<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
