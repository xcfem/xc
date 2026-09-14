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
modelSpace= predefined_spaces.SolidMechanics2D(nodeHandler)
m= 1e3
nodeMassMatrix= xc.Matrix([[m, 0], [0, m]])

nodes= list()
refCenterOfMass= geom.Pos3d(0, 0, 0)
for x in range(0, 10):
    refCenterOfMass+= geom.Vector3d(x, 0, 0)
    newNode= modelSpace.newNode(x, 0)
    newNode.mass= nodeMassMatrix
    nodes.append(newNode)
factor= 1/len(nodes)
refCenterOfMass.x*= factor
refCenterOfMass.y*= factor
refCenterOfMass.z*= factor

# Check results.
xcTotalSet= modelSpace.getTotalSet()
totalMass= xcTotalSet.totalMass
refTotalMass= len(nodes)*nodeMassMatrix
ratio1= (totalMass-refTotalMass).Norm()/refTotalMass.Norm()
centerOfMass= xcTotalSet.getCenterOfMass(True)
ratio2= (centerOfMass-refCenterOfMass).getModulus()/refCenterOfMass.x

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
