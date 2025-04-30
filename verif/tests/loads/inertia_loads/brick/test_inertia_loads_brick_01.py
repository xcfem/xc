# -*- coding: utf-8 -*-
'''Check inertia load on brick elements.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials
from scipy.constants import g

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Material definition
rho= 2500
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25, rho= rho)
# Mesh
## Nodes.
nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
n0= nodes.newNodeXYZ(0,0,0)
n1= nodes.newNodeXYZ(1,0,0)
n2= nodes.newNodeXYZ(1,1,0)
n3= nodes.newNodeXYZ(0,1,0)
n4= nodes.newNodeXYZ(0,0,1)
n5= nodes.newNodeXYZ(1,0,1)
n6= nodes.newNodeXYZ(1,1,1)
n7= nodes.newNodeXYZ(0,1,1)

## Elements.
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast3d.name
brick= elements.newElement("Brick",xc.ID([n0.tag, n1.tag, n2.tag, n3.tag, n4.tag, n5.tag, n6.tag, n7.tag]))

constraints= preprocessor.getBoundaryCondHandler
# Constrain the displacement of the nodes in the base.
n0.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
n1.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
n2.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
n3.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))

## Whole model mass data.
xcTotalSet= modelSpace.getTotalSet()
massZ= xcTotalSet.getTotalMassComponent(2)
massRefZ= rho
ratio1= abs(massZ-massRefZ)/massRefZ

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
accel= xc.Vector([0,0,g])
xcTotalSet= modelSpace.getTotalSet()
modelSpace.createSelfWeightLoad(xcTotalSet, accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

R= n0.getReaction[2]
R_ref= massZ*g/4.0

ratio2= abs(R-R_ref)/R_ref

# Check "horizontal" and "vertical" reactions.
R0= 0
R1= 0
vReac= list() # "vertical" reactions.
for n in [n0, n1, n2, n3]:
    R= n.getReaction
    R0+= R[0]
    R1+= R[1]
    vReac.append(R[2])
R2= sum(vReac)
weightRefZ= massRefZ*g
ratio3= abs(R2-weightRefZ)/weightRefZ

testOK= abs(ratio1)<1e-8 and abs(ratio2)<1e-8
testOK= testOK and (abs(R0)<1e-6)
testOK= testOK and (abs(R1)<1e-6)
testOK= testOK and (abs(ratio3)<1e-2)

'''
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
print('R0= ', R0)
print('R1= ', R1)
print(R0)
print(R1)
print('R2= ', R2/1e3)
print('weightRefZ= ', weightRefZ/1e3)
print('ratio2= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# #oh.displayFEMesh()
# #oh.displayLocalAxes()
# oh.displayReactions()
