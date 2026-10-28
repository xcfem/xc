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
from solution import predefined_solutions

silent= True

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

# Materials
concrete= EC2_materials.C30

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes) 
## Problem geometry
pTop= modelSpace.newKPoint(0, 0)
pBottom= modelSpace.newKPoint(0,-lPile)
pileLine= modelSpace.newLine(pTop, pBottom)
eSize= 0.24 # length of elements
pileLine.setElemSize(eSize)

pileBC= pytzqz_pile.PyTzQzCircularPile(pileSet= pileLine, diameter= pileDiameter, E= concrete.Ecm(), pileType='endBearing', soilLayers= soilLayers)

## Create mesh.
### Finite element material
xcSection= pileBC.defElasticShearSection2d(preprocessor)
modelSpace.setDefaultMaterial(xcSection)

### Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)
### Set element type.
modelSpace.newSeedElement('ElasticBeam2d')
### Generate mesh.
pileLine.genMesh(xc.meshDir.I)

### Constraints.
pileBC.generateSpringsPile()

# Just check the springs are created.
numberOfShaftSprings= len(pileBC.springs)
numberOfNodes= len(pileLine.nodes)
testOK= (numberOfShaftSprings==numberOfNodes)

# Loads
Fv= -5500e3
lp0= modelSpace.newLoadPattern(name= 'lp0', setCurrent= True)
pileTopNode= pTop.getNode()
lp0.newNodalLoad(pileTopNode.tag, xc.Vector([0.0, Fv, 0.0]))
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure.
solProc= predefined_solutions.TransformationNewtonRaphsonBandGen(feProblem, convergenceTestTol= 1e-7, convTestType= 'norm_disp_incr_conv_test', maxNumIter= 10, printFlag= 0)
solProc.setup()
solProc.getAnalysis().analyze(1)

# Check results.
uY= pileTopNode.getDisp[1]
ratio1= abs(uY+2.1541611566793932e-3)/2.1541611566793932
testOK&= (ratio1<1e-5)

if(not silent):
    print('Number of springs along the shaft: ', numberOfShaftSprings)
    print('Number of nodes along the shaft: ', numberOfNodes)
    print('vertical displacement: ', uZ*1e3, 'mm')

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
if(not silent):
## Compute spring stiffness for each node.
    springElementSet= modelSpace.defSet('springElementSet', elements= pileBC.springs)
    springElementSet.fillDownwards()
    for n in pileLine.nodes:
        elements= n.getConnectedElements(springElementSet)
        for e in elements:
            materials= e.getMaterials()
            kx= materials[0].getTangent()
            n.setProp('k_x', kx)
    from postprocess import output_handler
    oh= output_handler.OutputHandler(modelSpace)
    # oh.displayBlocks()
    # oh.displayFEMesh()
    # oh.displayLocalAxes()
    oh.displayLoads()
    oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
    oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
    oh.displayReactions()
    oh.displayNodeValueDiagram(itemToDisp= 'k_x', caption= 'Spring stiffness x direction', defaultDirection= 'X', defaultValue= 0.0)
