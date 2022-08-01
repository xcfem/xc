# -*- coding: utf-8 -*-
'''Displacement control integrator trivial test.'''

from __future__ import print_function
from __future__ import division

import math
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
lp0.newNodalLoad(n2.tag,xc.Vector([1,0,0]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Constraints
modelSpace.fixNode000(n1.tag)
modelSpace.fixNodeF00(n2.tag)

# Solution
dispIncrement= -.005
numSteps= 10
solProc= predefined_solutions.SimpleNewtonRaphsonDisplacementControl(prb= feProblem, node= n2, dof= 0, increment= dispIncrement, numSteps= numSteps)
solProc.setup()
integrator= solProc.integrator

targetDisplacements= [0.0, 0.005, 0.01, 0.015, 0.02, 0.0275, 0.035, 0.0425, 0.05, 0.0625, 0.07500000000000001, 0.08750000000000001, 0.1, 0.14270833325, 0.1854166665, 0.22812499975, 0.270833333, 0.22812499975, 0.1854166665, 0.14270833325, 0.1, 0.08750000000000001, 0.07500000000000001, 0.0625, 0.05, 0.0425, 0.035, 0.0275, 0.02, 0.015, 0.01, 0.005, 0.0, -0.005, -0.01, -0.015, -0.02, -0.0275, -0.035, -0.0425, -0.05, -0.0625, -0.07500000000000001, -0.08750000000000001, -0.1, -0.14270833325, -0.1854166665, -0.22812499975, -0.270833333, -0.22812499975, -0.1854166665, -0.14270833325, -0.1, -0.08750000000000001, -0.07500000000000001, -0.0625, -0.05, -0.0425, -0.035, -0.0275, -0.02, -0.015, -0.01, -0.005, 0.0, 0.005, 0.01, 0.015, 0.02, 0.0275, 0.035, 0.0425, 0.05, 0.0625, 0.07500000000000001, 0.08750000000000001, 0.1, 0.14270833325, 0.1854166665, 0.22812499975, 0.270833333]
displacements= list()

d0= 0.0
for d in targetDisplacements:
    incU= (d-d0)
    integrator.increment= incU/numSteps
    analOk= solProc.solve()
    if(analOk!=0):
        print("Can't solve.")
        exit(1)
    deltax= n2.getDisp[0]
    displacements.append(deltax)
    d0= deltax

# Compute error.
err= 0
for d, td in zip(displacements, targetDisplacements):
    err+= (td-d)**2
err= math.sqrt(err)

'''
print("err= ",err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(err)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # FIGURES & REPORTS
# import matplotlib.pyplot as plt
# plt.title(fname)
# plt.xlabel('index')
# plt.ylabel('displacement')
# xi= range(0, len(targetDisplacements))
# plt.plot(xi, displacements, color= 'blue')
# plt.plot(xi, targetDisplacements, 'ro')
# plt.show()
