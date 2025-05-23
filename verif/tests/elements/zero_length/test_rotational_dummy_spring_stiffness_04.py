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
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
## Define nodes.
nodeQuadruplets= list()
cornerNodes= list()
for a in range(0,11):
    offset= 5*a
    n0= modelSpace.newNodeXYZ(1+offset,1,1)
    n1= modelSpace.newNodeXYZ(0+offset,1,1)
    n2= modelSpace.newNodeXYZ(1+offset,0,1)
    n3= modelSpace.newNodeXYZ(1+offset,1,0)
    nodeQuadruplets.append((n0, n1, n2, n3))
    cornerNodes.append(n0)

# Define materials.
A= .1*.1
elast= typical_materials.defElasticMaterial(preprocessor, "elast", E= 2.1e9)
# Element definition.
elements= preprocessor.getElementHandler
elements.dimElem= 3 # Bidimensional space.
elements.defaultMaterial= elast.name
elementTriplets= list()
for (n0, n1, n2, n3) in nodeQuadruplets:
    truss1= elements.newElement("Truss",xc.ID([n1.tag,n0.tag]))
    truss1.sectionArea= A
    truss2= elements.newElement("Truss",xc.ID([n2.tag,n0.tag]))
    truss2.sectionArea= A
    truss3= elements.newElement("Truss",xc.ID([n3.tag,n0.tag]))
    truss3.sectionArea= A
    elementTriplets.append((truss1, truss2, truss3))

# Constraints.
for (n0, n1, n2, n3) in nodeQuadruplets:
    modelSpace.fixNode('000_000', n1.tag)
    modelSpace.fixNode('000_000', n2.tag)
    modelSpace.fixNode('000_000', n3.tag)

# Create dummy springs.
springsAndNodes= strut_and_tie_utils.define_dummy_springs(modelSpace, cornerNodes)

# Load definition.
F= 1e4
M= F/10.0
lp0= modelSpace.newLoadPattern(name= '0')
for n in cornerNodes:
    lp0.newNodalLoad(n.tag,xc.Vector([-F,-F, -F, M, 2*M, 3*M]))
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
deltaZ= list()
rotX= list()
rotY= list()
rotZ= list()
for n in cornerNodes:
    deltaX.append(n.getDisp[0])
    deltaY.append(n.getDisp[1])
    deltaZ.append(n.getDisp[2])
    rotX.append(n.getDisp[3])
    rotY.append(n.getDisp[4])
    rotZ.append(n.getDisp[5])

refDeltaX= -F/(elast.E*A)
refDeltaY= -F/(elast.E*A)
refDeltaZ= -F/(elast.E*A)
refRotX= M/strut_and_tie_utils.dummy_spring_stiffness
refRotY= 2*M/strut_and_tie_utils.dummy_spring_stiffness
refRotZ= 3*M/strut_and_tie_utils.dummy_spring_stiffness

ratio1= 0.0
ratio2= 0.0
ratio3= 0.0
ratio4= 0.0
ratio5= 0.0
ratio6= 0.0
for dX, dY, dZ, rx, ry, rz in zip(deltaX, deltaY, deltaZ, rotX, rotY, rotZ):
    ratio1= max(ratio1, abs(abs(dX-refDeltaX)/refDeltaX))
    ratio2= max(ratio2, abs(abs(dY-refDeltaY)/refDeltaY))
    ratio3= max(ratio3, abs(abs(dZ-refDeltaZ)/refDeltaZ))
    ratio4= max(ratio4, abs(abs(rx-refRotX)/refRotX))
    ratio5= max(ratio5, abs(abs(ry-refRotY)/refRotY))
    ratio6= max(ratio6, abs(abs(rz-refRotZ)/refRotZ))
    
## Check reactions.
springAttachmentNodes= list()
for (zl, n10) in springsAndNodes:
    springAttachmentNodes.append(n10)

errR10= 0.0

for n in springAttachmentNodes:
    R10= n10.getReaction
    errR10= max(errR10, math.sqrt(R10[0]**2+R10[1]**2+R10[2]**2+(R10[3]+M)**2+(R10[4]+2*M)**2+(R10[5]+3*M)**2))

errR1= 0.0
errR2= 0.0
errR3= 0.0
for (n0, n1, n2, n3) in nodeQuadruplets:
    R1= n1.getReaction
    errR1= max(errR1, math.sqrt((R1[0]-F)**2+R1[1]**2+R1[2]**2+R1[3]**2+R1[4]**2+R1[5]**2))
    R2= n2.getReaction
    errR2= max(errR2, math.sqrt(R2[0]**2+(R2[1]-F)**2+R2[2]**2+R2[3]**2+R2[4]**2+R2[5]**2))
    R3= n3.getReaction
    errR3= max(errR3, math.sqrt(R3[0]**2+R3[1]**2+(R3[2]-F)**2+R3[3]**2+R3[4]**2+R3[5]**2))


'''
print('deltaX= ', deltaX*1e3, 'mm', ratio1)
print('deltaY= ', deltaY*1e3, 'mm', ratio2)
print('deltaZ= ', deltaZ*1e3, 'mm', ratio3)
print('rotX= ', rotX*1e3, 'mrad', ratio4)
print('rotY= ', rotY*1e3, 'mrad', ratio5)
print('rotZ= ', rotZ*1e3, 'mrad', ratio6)
print('R10= ', R10*1e-3, 'kN', errR10)
print('R1= ', R1*1e-3, 'kN', errR1)
print('R2= ', R2*1e-3, 'kN', errR2)
print('R3= ', R3*1e-3, 'kN', errR3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6) and (ratio2<1e-6) and (ratio3<1e-6) and (ratio4<1e-6) and (ratio5<1e-6) and (ratio6<1e-6) and (errR10<1e-8) and (errR1<1e-8) and (errR2<1e-8) and (errR3<1e-8): 
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# oh.displayLoads()
# oh.displayReactions()
