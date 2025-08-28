# -*- coding: utf-8 -*-
''' SlidingVectorLoad class verification (3D structural mechanics).

    This example illustrates how, in certain cases, the algorithm gives a
    result that is statically correct but mechanically unsatisfactory due to 
    the load concentrations it produces (see reaction in fourth node -n4- 
    below).
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from actions import loads

# Load
f= 2e3 # Load magnitude.

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
leverArm= 1
n1= nodes.newNodeXYZ(-leverArm,0.0,0.0)
n2= nodes.newNodeXYZ(leverArm,0.0,0.0)

n3= nodes.newNodeXYZ(-leverArm,0.0,-leverArm)
n4= nodes.newNodeXYZ(leverArm,0.0,-leverArm)


# Constraints
for n in [n1, n2, n3, n4]:
    modelSpace.fixNode000_000(n.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
## Load nodes n1 and n2
lVectorA= xc.Vector([0,0,-f, 0,0,0]) # Fx, Fy, Fz, Mx, My, Mz
slidingVectorLoadAa= loads.SlidingVectorLoad(name= 'test', nodes= [n1,n2], pntCoord= [0.0,0.0,0.0], loadVector= lVectorA)
slidingVectorLoadAa.appendLoadToCurrentLoadPattern()
lVectorB= xc.Vector([f,0,0, 0,0,0]) # Fx, Fy, Fz, Mx, My, Mz
slidingVectorLoadBa= loads.SlidingVectorLoad(name= 'test', nodes= [n1,n2], pntCoord= [0.0,0.0,0.0], loadVector= lVectorB)
slidingVectorLoadBa.appendLoadToCurrentLoadPattern()
## Load nodes n3 and nh
slidingVectorLoadAb= loads.SlidingVectorLoad(name= 'test', nodes= [n3,n4], pntCoord= [0.0,0.0,0.0], loadVector= lVectorA)
slidingVectorLoadAb.appendLoadToCurrentLoadPattern()
slidingVectorLoadBb= loads.SlidingVectorLoad(name= 'test', nodes= [n3,n4], pntCoord= [0.0,0.0,0.0], loadVector= lVectorB)
slidingVectorLoadBb.appendLoadToCurrentLoadPattern()

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)
## Compute solution
result= modelSpace.analyze(1, calculateNodalReactions= True)
if(result!=0):
    print('Can\'t solve.')
    exit(1)

R1= n1.getReaction
R2= n2.getReaction
R3= n3.getReaction
R4= n4.getReaction

# Check results.
err_a= (R1[0]+R2[0]+f)**2 # equlibrium on x
err_a+= (R1[2]+R2[2]-f)**2 # equlibrium on y
err_a+= (-R1[2]*leverArm+R2[2]*leverArm)**2 # moment equilibrium
err_b= (R3[0]+R4[0]+f)**2 # equlibrium on x
err_b+= (R3[2]+R4[2]-f)**2 # equlibrium on y
err_b+= (R3[0]*leverArm-R3[2]*leverArm+R4[0]*leverArm+R4[2]*leverArm)**2 # moment equilibrium

'''
print("R1= ", R1)
print("R2= ", R2)
print(err_a)
print("R3= ", R3)
print("R4= ", R4)
print(err_b)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(err_a)<1e-10) & (abs(err_b)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# #########################################################
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# ## Uncomment to display the loads
# oh.displayLoads()
