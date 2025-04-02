# -*- coding: utf-8 -*-
'''Check representation of inertia loads on quad elements.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import xc
from model import predefined_spaces
from materials import typical_materials

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)
squareSide= 1
refArea= squareSide**2
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(squareSide,0)
n3= nodes.newNodeXY(squareSide,squareSide)
n4= nodes.newNodeXY(0,squareSide)

# Materials definition
E= 2.1e6 # Steel Young's modulus [kg/cm2].
nu= 0.3 # Poisson's ratio.
h= 0.2 # thickness.
rho= 5.0 # specific mass [kg/m3].
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)
volRho= elast2d.rho


elements= preprocessor.getElementHandler
elements.defaultMaterial= elast2d.name
elem= elements.newElement("FourNodeQuad",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))
elem.physicalProperties.thickness= h
arealRho= elast2d.rho*elem.physicalProperties.thickness

## Whole model mass data.
xcTotalSet= modelSpace.getTotalSet()
massY= xcTotalSet.getTotalMassComponent(1)

gravity= 9.81 # m/s2

# Constraints.
constrainedNodes= [n1, n2, n3, n4]
for n in constrainedNodes:
    modelSpace.fixNode00(n.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
accel= xc.Vector([0,gravity])
elem.createInertiaLoad(accel)
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

