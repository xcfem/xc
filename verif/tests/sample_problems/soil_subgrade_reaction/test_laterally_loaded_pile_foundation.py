# -*- coding: utf-8 -*-
'''Laterally-loaded pile foundation.

Static pushover of a single pile, modeled as a beam on a nonlinear Winkler 
foundation. Lateral soil response is described by p-y springs. Vertical soil 
response described by t-z and q-z springs.

Based on the example 14.4.1 of OpenSees (see https://openseespydoc.readthedocs.io/en/stable/src/pile.html) by Pavan Chigullapally from University of Auckland, Auckland.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

silent= True

import math
import xc
from model import predefined_spaces
from model.sets import sets_mng 
from materials import soil_structure_interaction as ssi
from materials import typical_materials
from materials.ec2 import EC2_materials
from geotechnics import frictional_soil as fs
from geotechnics.foundations import pile
from geotechnics.foundations import pytzqz_pile
from solution import predefined_solutions
from actions.quake import ground_motion_utils as gmu
from scipy.constants import g

# Pile geometry.
columnLength = 1.0 # length of pile head (above ground surface) (m)
pileLength = 20.0 # length of embedded pile (below ground surface) (m)
pileDiameter = 1.0 # pile diameter
eleSize= 0.25 # pile element length 

## Soil data.
zGround= pileLength
soil= fs.FrictionalSoil(phi= math.radians(36.0), rho= 17e3/g, gammaMPhi= 1.0)
# select pult definition method for p-y curves
# API --> 1
# Brinch Hansen --> 2
soil.puSwitch= 1
# variation in coefficent of subgrade reaction with depth for p-y curves
# API linear variation (default)   --> 1
# modified API parabolic variation --> 2
soil.kSwitch= 1
# Viscous damping term (dashpot).
soil.dashpotC= 0.0
# Cofficient to compute the  drag resistance within a fully-mobilized gap as
# Cd*pult.
soil.Cd= 0.0
# Soil shear modulus at pile tip.
soil.G= 150000000.0 
# Soil suction at pile tip during tension/uplift.
soil.suction= 0.0

soils=[[-100.0, soil]] # Properties of the soil layers:
soilLayers= pytzqz_pile.SoilLayers(soilProfile= soils, groundLevel= zGround)

# Materials
concrete= EC2_materials.C20

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) 
## Problem geometry
### K-points.
pileTipPt= modelSpace.newKPoint(0.0, 0.0, 0.0) # pile tip.
pileHead= modelSpace.newKPoint(0.0, 0.0, pileLength)
columnHead= modelSpace.newKPoint(0.0, 0.0, columnLength+pileLength) # pile head.
# Lines.
pileLine= modelSpace.newLine(pileTipPt, pileHead)
columnLine= modelSpace.newLine(pileHead, columnHead)
pileLine.setElemSize(eleSize)

pileBC= pytzqz_pile.PyTzQzCircularPile(pileSet= pileLine, diameter= pileDiameter, E= concrete.Ecm(), pileType='endBearing', soilLayers= soilLayers)

## Create mesh.
### Finite element material
xcSection= pileBC.defElasticShearSection3d(preprocessor)
modelSpace.setDefaultMaterial(xcSection)

### Geometric transformations
xzVector= xc.Vector([0, -1, 0])
lin= modelSpace.newLinearCrdTransf("lin", xzVector= xzVector)
modelSpace.setDefaultCoordTransf(lin)
### Set element type.
modelSpace.newSeedElement('ElasticBeam3d')
### Generate mesh.
pileLine.genMesh(xc.meshDir.I)
columnLine.genMesh(xc.meshDir.I)

pileSet= modelSpace.defSet('pileSet', lines= [pileLine, columnLine])
pileSet.fillDownwards()

## Create springs along the pile shaft.
pileBC.generateSpringsPile()
### Put the springs in a set.
springElementSet= modelSpace.defSet('springElementSet', elements= pileBC.springs)
springElementSet.fillDownwards()

# Create recorders.
domain= modelSpace.getDomain()

## Record displacements at pile nodes
displacements= dict()
pileNodeTags= pileSet.getNodeTags()
for tag in pileNodeTags:
    displacements[tag]= list()
dispRecorder= domain.newRecorder("node_prop_recorder", None)
dispRecorder.setNodes(xc.ID(pileNodeTags))
dispRecorder.callbackRecord= "displacements[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime, self.getDisp.getList()))"
## Record reaction force in the p-y springs
reactions= dict()
springNodes= pileBC.getSpringFixedNodes()
springNodeTags= [n.tag for n in springNodes]
for tag in springNodeTags:
    reactions[tag]= list()
reacRecorder= domain.newRecorder("node_prop_recorder", None)
reacRecorder.setNodes(xc.ID(springNodeTags))
reacRecorder.callbackRecord= "self.getDomain.calculateNodalReactions(False, 1e-2); reactions[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime, self.getReaction.getList()))"
# Record element forces in pile elements
internal_forces= dict()
pileElementTags= list(pileLine.getElementTags())
for tag in pileElementTags:
    internal_forces[tag]= list()
iForcesRecorder= domain.newRecorder("element_prop_recorder",None)
iForcesRecorder.setElements(xc.ID(pileElementTags))
iForcesRecorder.callbackRecord= "internal_forces[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime, self.getNodeResistingForceIncInertia(1).getList()))"

# record information at specified increments
timeStep = 0.5

# Create loads.
domain.currentTime= 10.0 

## Apply point load at the uppermost pile node in the x-direction
factors = [0.0, 0.0, 1.0, 1.0]
time = [0.0, 10.0, 20.0, 10000.0]
ts1= modelSpace.newTimeSeries('ts1', "path_time_ts", setCurrent= True)
ts1.path= xc.Vector(factors)
ts1.time= xc.Vector(time)
lp1= modelSpace.newLoadPattern(name= 'lp1')
columnHeadNode= columnHead.getNode()
lp1.newNodalLoad(columnHeadNode.tag, xc.Vector([3500.0e3, 0.0, 0.0, 0.0, 0.0, 0.0]))
modelSpace.addLoadCaseToDomain(lp1.name)

# Solution procedure.
solProc= predefined_solutions.TransformationNewtonRaphsonBandGen(feProblem, convergenceTestTol= 1e-7, convTestType= 'norm_disp_incr_conv_test', maxNumIter= 100, printFlag= 0)
solProc.setup()
solProc.setDeltaLambda(.05)

solProc.getAnalysis().analyze(201)

# Check results.
uXExtrema= gmu.MinMaxTracker()
for key in displacements:
    nodeDisplacements= displacements[key]
    for (t, u) in nodeDisplacements:
        uXExtrema.update(u[0], t, key)

fXExtrema= gmu.MinMaxTracker()
for key in reactions:
    nodeReactions= reactions[key]
    for (t, R) in nodeReactions:
        fXExtrema.update(R[0], t, key)
        
uXMinRef= -0.00286142601800859
tUXMinRef= 20.05
uXMaxRef= .3928310360864577
tUXMaxRef= 20.05
ratio1= abs(uXExtrema.min-uXMinRef)/uXMinRef # Minimum displacement.
ratio2= abs(uXExtrema.t_min-tUXMinRef)/tUXMinRef
ratio3= abs(uXExtrema.max-uXMaxRef)/uXMaxRef
ratio4= abs(uXExtrema.t_max-tUXMaxRef)/tUXMaxRef

fXMinRef= -622.4289515574546e3
tFXMinRef= 20.05
fXMaxRef= 450.68899505561825e3
tFXMaxRef= 20.05
ratio5= abs(fXExtrema.min-fXMinRef)/fXMinRef
ratio6= abs(fXExtrema.t_min-tFXMinRef)/tFXMinRef
ratio7= abs(fXExtrema.max-fXMaxRef)/fXMaxRef
ratio8= abs(fXExtrema.t_max-tFXMaxRef)/tFXMaxRef

if(not silent):
    # print('Extreme node displacements are: ', uXExtrema.getDict())
    print('Check displacements.')
    print('  Minimum displacement: ', uXExtrema.min*1e3, 'mm')
    print('  Expected minimum displacement: ', uXMinRef*1e3, 'mm')
    print('  Minimum displacement; ratio1= ', ratio1)
    print('  Time when minimum displacement occurs: ', uXExtrema.t_min, 's')
    print('  Expected time when minimum displacement occurs: ', tUXMinRef, 's')
    print('  Time of minimum displacement; ratio2= ', ratio2)
    print('\n  Maximum displacement: ', uXExtrema.max*1e3, 'mm')
    print('  Expected maximum displacement: ', uXMaxRef*1e3, 'mm')
    print('  Maximum displacement; ratio3= ', ratio3)
    print('  Time when maximum displacement occurs: ', uXExtrema.t_max, 's')
    print('  Expected time when maximum displacement occurs: ', tUXMaxRef, 's')
    print('  Time of minimum displacement; ratio4= ', ratio4)
    # print('Extreme node reactions are: ', fXExtrema.getDict())    
    print('\nCheck reactions.')
    print('  Minimum reaction: ', fXExtrema.min/1e3, 'kN')
    print('  Expected minimum reaction: ', fXMinRef/1e3, 'kN')
    print('  Expected minimum reaction; ratio5= ', ratio5)
    print('  Time when minimum reaction occurs: ', fXExtrema.t_min, 's')
    print('  Expected time when minimum reaction occurs: ', tFXMinRef, 's')
    print('  Time of minimum reaction; ratio6= ', ratio6)
    print('\n  Maximum reaction: ', fXExtrema.max/1e3, 'kN')
    print('  Expected maximum reaction: ', fXMaxRef/1e3, 'kN')
    print('  Expected maximum reaction; ratio7= ', ratio7)
    print('  Time when maximum reaction occurs: ', fXExtrema.t_max, 's')
    print('  Expected time when maximum reaction occurs: ', tFXMaxRef, 's')
    print('  Time of maximum reaction; ratio8= ', ratio8)
    
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-12) and (abs(ratio2)<1e-12) and (abs(ratio3)<1e-12) and (abs(ratio4)<1e-12) and (abs(ratio5)<1e-12) and (abs(ratio6)<1e-12) and (abs(ratio7)<1e-12) and (abs(ratio8)<1e-12)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Graphic stuff.    
if(not silent):
    ## Compute spring stiffness for each node.
    for n in pileLine.nodes:
        elements= n.getConnectedElements(springElementSet)
        for e in elements:
            materials= e.getMaterials()
            kx= materials[0].getTangent()
            n.setProp('k_x', kx)
            ky= materials[1].getTangent()
            n.setProp('k_y', ky)
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
    oh.displayNodeValueDiagram(itemToDisp= 'k_y', caption= 'Spring stiffness y direction', defaultDirection= 'Y', defaultValue= 0.0)
