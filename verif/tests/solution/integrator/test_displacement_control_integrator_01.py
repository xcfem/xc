# -*- coding: utf-8 -*-
'''Displacement control integrator trivial test.'''

from __future__ import print_function
from __future__ import division

import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Define materials
k= typical_materials.defElasticMaterial(preprocessor, "k",E= 1e3)

# Define mesh.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(0,0)

# Define element.
elements= preprocessor.getElementHandler
elements.defaultMaterial= k.name
elements.dimElem= 2 # Dimension of element space
spring= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))

# Define load.
## Load definition.
ts= modelSpace.newTimeSeries(name= "ts", tsType= "linear_ts")
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([10,0,0]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Constraints
modelSpace.fixNode000(n1.tag)
modelSpace.fixNodeF00(n2.tag)

# Solution
dispIncrement= .005
maxU= 0.075
numSteps= int(maxU/dispIncrement)
solProc= predefined_solutions.SimpleNewtonRaphsonDisplacementControl(prb= feProblem, node= n2, dof= 0, increment= dispIncrement, numSteps= numSteps)

solProc.solve(calculateNodalReactions= True)

deltax= n2.getDisp[0]
ratio1= abs(deltax-maxU)/maxU
R= n1.getReaction[0]
F= k.E*maxU
ratio2= abs(F+R)/F

'''
print("dx= ",deltax)
print("ratio1= ",ratio1)
print("R= ",R)
print("F= ",F)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
