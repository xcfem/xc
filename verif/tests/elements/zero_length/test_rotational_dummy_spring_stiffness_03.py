# -*- coding: utf-8 -*-
'''Verification of the define_dummy_springs function in the strut_and_tie_utils
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

## Define nodes.
nodeTriplets= list()
cornerNodes= list()
for a in range(0,11):
    offset= 5*a
    n0= modelSpace.newNodeXY(1+offset, 1)
    n1= modelSpace.newNodeXY(1+offset, 0)
    n2= modelSpace.newNodeXY(0+offset, 1)
    nodeTriplets.append((n0, n1, n2))
    cornerNodes.append(n0)

# Materials definition
A= .1*.1
elast= typical_materials.defElasticMaterial(preprocessor, "elast", E= 2.1e9)
# Element definition.
elements= preprocessor.getElementHandler
elements.dimElem= 2 # Bidimensional space.
elements.defaultMaterial= elast.name
elementDuets= list()
for (n0, n1, n2) in nodeTriplets:
    truss1= elements.newElement("Truss",xc.ID([n1.tag,n0.tag]))
    truss1.sectionArea= A
    truss2= elements.newElement("Truss",xc.ID([n2.tag,n0.tag]))
    truss2.sectionArea= A
    elementDuets.append((truss1, truss2))

# Constraints.
for (n0, n1, n2) in nodeTriplets:
    modelSpace.fixNode('000', n1.tag)
    modelSpace.fixNode('000', n2.tag)

# Create dummy springs.
springsAndNodes= strut_and_tie_utils.define_dummy_springs(modelSpace, cornerNodes)

# Load definition.
F= 1e4
M= F/10.0
lp0= modelSpace.newLoadPattern(name= '0')
for n in cornerNodes:
    lp0.newNodalLoad(n.tag,xc.Vector([-F,-F, M]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)

# Check results.
## Check displacements.
deltaX= list()
deltaY= list()
rot= list()
for n in cornerNodes:
    deltaX.append(n.getDisp[0])
    deltaY.append(n.getDisp[1])
    rot.append(n.getDisp[2])
    
refDeltaX= -F/(elast.E*A)
refDeltaY= -F/(elast.E*A)
refRot= M/strut_and_tie_utils.dummy_spring_stiffness

ratio1= 0.0
ratio2= 0.0
ratio3= 0.0
for dX, dY, r in zip(deltaX, deltaY, rot):
    ratio1= max(ratio1, abs(abs(dX-refDeltaX)/refDeltaX))
    ratio2= max(ratio2, abs(abs(dY-refDeltaY)/refDeltaY))
    ratio3= max(ratio3, abs(abs(r-refRot)/refRot))

## Check reactions.
springAttachmentNodes= list()
for (zl, n10) in springsAndNodes:
    springAttachmentNodes.append(n10)
    
errR10= 0.0
for n in springAttachmentNodes:
    r10= n.getReaction
    errR10= max(errR10, math.sqrt(r10[0]**2+r10[1]**2+(r10[2]+M)**2))
    
errR1= 0.0
errR2= 0.0
for (n0, n1, n2) in nodeTriplets:
    r1= n1.getReaction
    errR1= max(errR1, math.sqrt(r1[0]**2+(r1[1]-F)**2+r1[2]**2))
    r2= n2.getReaction
    errR2= max(errR2, math.sqrt((r2[0]-F)**2+r2[1]**2+r2[2]**2))

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
