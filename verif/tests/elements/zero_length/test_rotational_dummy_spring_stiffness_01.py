# -*- coding: utf-8 -*-
'''Verification of the create_dummy_spring function in the strut_and_tie_utils
   module.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from materials import typical_materials
from model import predefined_spaces
from model.mesh import strut_and_tie_utils

# Define FE problem. 
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

n0= modelSpace.newNodeXY(1,1)
n1= modelSpace.newNodeXY(1,0)
n2= modelSpace.newNodeXY(0,1)

# Materials definition
A= .1*.1
elast= typical_materials.defElasticMaterial(preprocessor, "elast", E= 2.1e9)
# Element definition.
elements= preprocessor.getElementHandler
elements.dimElem= 2 # Bidimensional space.
elements.defaultMaterial= elast.name
truss1= elements.newElement("Truss",xc.ID([n1.tag,n0.tag]))
truss1.sectionArea= A
truss2= elements.newElement("Truss",xc.ID([n2.tag,n0.tag]))
truss2.sectionArea= A

# Constraints.
modelSpace.fixNode('000', n1.tag)
modelSpace.fixNode('000', n2.tag)

# Create dummy spring.
zl, n10= strut_and_tie_utils.create_dummy_spring(modelSpace, n0)

# Load definition.
F= 1e4
M= F/10.0
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n0.tag,xc.Vector([-F,-F, M]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)
    
deltaX= n0.getDisp[0]
refDeltaX= -F/(elast.E*A)
deltaY= n0.getDisp[1]
refDeltaY= -F/(elast.E*A)
rot= n0.getDisp[2]
refRot= M/strut_and_tie_utils.dummy_spring_stiffness

R10= n10.getReaction
errR10= math.sqrt(R10[0]**2+R10[1]**2+(R10[2]+M)**2)
R1= n1.getReaction
errR1= math.sqrt(R1[0]**2+(R1[1]-F)**2+R1[2]**2)
R2= n2.getReaction
errR2= math.sqrt((R2[0]-F)**2+R2[1]**2+R2[2]**2)



ratio1= abs(abs(deltaX-refDeltaX)/refDeltaX)
ratio2= abs(abs(deltaY-refDeltaY)/refDeltaY)
ratio3= abs(abs(rot-refRot)/refRot)

'''
print('deltaX= ', deltaX*1e3, 'mm', ratio1)
print('deltaY= ', deltaY*1e3, 'mm', ratio2)
print('rot= ', rot*1e3, 'mrad', ratio3)
print('R10= ', R10*1e-3, 'kN', errR10)
print('R1= ', R1*1e-3, 'kN', errR1)
print('R2= ', R2*1e-3, 'kN', errR2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6) and (ratio2<1e-6) and (ratio3<1e-6) and (errR10<1e-8) and (errR1<1e-8) and (errR2<1e-8): 
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# oh.displayLoads()
# oh.displayReactions()
