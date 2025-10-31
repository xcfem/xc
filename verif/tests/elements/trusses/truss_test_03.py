# -*- coding: utf-8 -*-
''' Test from Ansys manual.

Reference:  Strength of Material, Part I, Elementary Theory and Problems, pg. 10, problem 2
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials
import math


E= 30e6 # Young modulus (psi)
l= 15*12 # Bar length (15 feet) expressed in inches.
theta= math.radians(30) # angle between bars
F= 5000 # Force magnitude (pounds).
A= 0.5 # Area in square inches.
a= 2*l*math.cos(theta) # Distance between end nodes.
b= l*math.sin(theta) # Distance between end nodes.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(a/2,-b)
n3= nodes.newNodeXY(a,0)

# Materials definition
sectionProperties= xc.CrossSectionProperties1d()
sectionProperties.A= A; sectionProperties.E= E;
section= typical_materials.defElasticSectionFromMechProp1d(preprocessor, "section", sectionProperties)


# Element definition.
elements= preprocessor.getElementHandler
elements.dimElem= 2 # Two-dimensional space.
elements.defaultMaterial= section.name
truss1= elements.newElement("TrussSection",xc.ID([n1.tag,n2.tag]))
truss2= elements.newElement("TrussSection",xc.ID([n2.tag,n3.tag]))

constraints= preprocessor.getBoundaryCondHandler
# Zero movement for node 1.
spc1= constraints.newSPConstraint(n1.tag,0,0.0)
spc2= constraints.newSPConstraint(n1.tag,1,0.0)
# Zero movement for node 3.
spc3= constraints.newSPConstraint(n3.tag,0,0.0)
spc4= constraints.newSPConstraint(n3.tag,1,0.0)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([0,-F]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= False)


delta= n2.getDisp[1]
stress= truss1.getMaterial().getStress()

ratio1= delta/(-0.12)
ratio2= stress/10000

'''
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1-1)<1e-5 and abs(ratio2-1)<1e-5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayDispRot(itemToDisp='uY')

