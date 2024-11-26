# -*- coding: utf-8 -*-
''' Check computation of inertia loads.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from actions import load_cases
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess.config import default_config
from postprocess import RC_material_distribution
from scipy.constants import g

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Define materials
## Materials.
concrete= EC2_materials.C25
steel= EC2_materials.S500B
## Geometry
h= 0.15 # depth
## RC section.
rcSection= def_simple_RC_section.RCRectangularSection(name='SlabSection', width= 1.0, depth= h, concrType= concrete, reinfSteelType= steel)
plateMat= rcSection.defElasticMembranePlateSection(preprocessor) # Elastic membrane plate section.

# Problem geometry.
span= 1.0

## K-points.
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(span,0.0,0.0))
pt3= points.newPoint(geom.Pos3d(span,span,0.0))
pt4= points.newPoint(geom.Pos3d(0.0,span,0.0))

## Surface.
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)
s.setElemSizeIJ(0.2, 0.2)

# Generate mesh.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= rcSection.name
elem= seedElemHandler.newElement("ShellMITC4")
s.genMesh(xc.meshDir.I)

# Constraints.
fixedNodes= list()
for p in [pt1, pt2, pt3, pt4]:
    n= p.getNode()
    fixedNodes.append(n)
for n in fixedNodes:
    modelSpace.fixNode000_FFF(n.tag)
    
# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['G1']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)
## load patterns.
### Self weight.
accel= xc.Vector([0,0,g])
cLC= loadCaseManager.setCurrentLoadCase('G1')
for e in s.elements:
    e.createInertiaLoad(accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(cLC.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

nForce= 2500*g*span*span*h/4.0

zReactions= list()
for n in fixedNodes:
    zReactions.append(n.getReaction[2])

err= 0.0
for r in zReactions:
    err+=(nForce-r)**2
err= math.sqrt(err)

'''
print(zReactions)
print('nForce= ', nForce)
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-6 :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayLoads()
