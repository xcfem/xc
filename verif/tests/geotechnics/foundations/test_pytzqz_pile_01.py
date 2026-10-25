# -*- coding: utf-8 -*-
'''Trivial test for PyTzQzPile class.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
import xc
from scipy.constants import g
from model import predefined_spaces
from materials.ec2 import EC2_materials
from geotechnics.foundations import pytzqz_pile
from geotechnics import frictional_soil as fs

# Data
pileDiameter= 1.5  # pile diameter [m]
lPile= 20.0 # Pile length.
## Soil data.
zGround= 0.0
sandySoil= fs.FrictionalSoil(phi= math.radians(30.0), rho= 18e3/g, gammaMPhi= 1.0)
# select pult definition method for p-y curves
# API --> 1
# Brinch Hansen --> 2
sandySoil.puSwitch= 1
# variation in coefficent of subgrade reaction with depth for p-y curves
# API linear variation (default)   --> 1
# modified API parabolic variation --> 2
sandySoil.kSwitch= 1
# Viscous damping term (dashpot).
sandySoil.dashpotC= 0.0
# Cofficient to compute the  drag resistance within a fully-mobilized gap as
# Cd*pult.
sandySoil.Cd= 0.0
# Soil shear modulus.
sandySoil.G= 150000000.0
# Soil suction at pile tip during tension/uplift.
sandySoil.suction= 0.0

soils=[[-100.0, sandySoil]] # Properties of the soil layers:
soilLayers= pytzqz_pile.SoilLayers(soilProfile= soils, groundLevel= zGround)


# Loads
Fh=706.9e3+572.5e3
N=-1532e3-1470.1e3
Fh=706.9e3
N=-1532e3

# Materials
concrete= EC2_materials.C30

# Define FE problem.
FEcase= xc.FEProblem()
preprocessor=FEcase.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) 
## Problem geometry
pTop= modelSpace.newKPoint(0, 0, 0)
pBottom= modelSpace.newKPoint(0, 0,-lPile)
l= modelSpace.newLine(pTop, pBottom)
eSize= 0.24 # length of elements
l.setElemSize(eSize)

pileBC= pytzqz_pile.PyTzQzCircularPile(pileSet= l, diameter= pileDiameter, E= concrete.Ecm(), pileType='endBearing', soilLayers= soilLayers)

## Create mesh.
### Finite element material
xcSection= pileBC.defElasticShearSection3d(preprocessor)
modelSpace.setDefaultMaterial(xcSection)

### Geometric transformations
xzVector= xc.Vector([1, 0, 0])
lin= modelSpace.newLinearCrdTransf("lin", xzVector= xzVector)
modelSpace.setDefaultCoordTransf(lin)
### Set element type.
modelSpace.newSeedElement('ElasticBeam3d')
### Generate mesh.
l.genMesh(xc.meshDir.I)

### Constraints.
pileBC.generateSpringsPile()

# Just check the springs are created.
numberOfShaftSprings= len(pileBC.springs)
numberOfNodes= len(l.nodes)
testOK= (numberOfShaftSprings==numberOfNodes)

'''
print(numberOfShaftSprings)
print(numberOfNodes)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
