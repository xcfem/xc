# -*- coding: utf-8 -*-
'''Check representation of inertia loads on brick elements.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
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

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
accel= xc.Vector([0,0,g])
xcTotalSet= modelSpace.getTotalSet()
modelSpace.createSelfWeightLoad(xcTotalSet, accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
# oh.displayLoads()

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayLoads(fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName)
