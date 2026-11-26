# -*- coding: utf-8 -*-
'''Check the Pile class when using sampled data instead of layers to represent
   the properties of soil. Trivial equilibrium test.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

silent= True

import os
import math
import json
import geom
import xc
import numpy as np
from scipy.constants import g
from model import predefined_spaces
from materials.ec2 import EC2_materials
from geotechnics.foundations import soil_sampled_data
from geotechnics.foundations import sampled_data_pytzqz_pile
from solution import rayleigh_damping as rd
from solution import predefined_solutions
from misc_utils import log_messages as lmsg


feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) # Defines the

# Problem geometry.
pileLengths= {'P-4': 23.5, 'P-5': 32.0, 'P-6': 33.5, 'P-7': 39.0, 'P-8': 38.0, 'P-9': 40.0, 'P-10': 40.0, 'P-11': 38.0, 'P-12': 40.0, 'P-13': 40.0, 'P-14': 34.0, 'P-15': 34.0, 'P-16': 34.0, 'P-17': 36.0, 'P-18': 36.0, 'P-19': 36.0, 'P-20': 38.0, 'P-21': 36.0, 'P-22': 39.0, 'P-23': 39.0, 'P-24': 33.0, 'P-25': 33.0, 'P-26': 31.0, 'P-27': 31.0, 'P-28': 31.0+1e-3, 'P-29': 33.0, 'P-30': 33.0, 'P-31': 27.0, 'P-32': 27.0, 'E-2': 24.0}
pileHeadZ= 3.15 # z coordinate of the pile heads.
selectedPiles= ['P-11']#, 'P-6', 'E-2']

## k-points.
headPoints= dict()
tipPoints= dict()
x= 0.0
y= 0.0
for i, pileCode in enumerate(selectedPiles):
    length= pileLengths[pileCode]
    headPoints[pileCode]= modelSpace.newKPoint(x, y, pileHeadZ)
    tipPoints[pileCode]= modelSpace.newKPoint(x, y, pileHeadZ-length)
    x+= 20.0
    if (i % 2)==0: # even
        y= 50.0
    else: # odd
        y= -50.0
    

## Lines.
pileLines= dict()
for pileCode in selectedPiles:
    headPoint= headPoints[pileCode]
    tipPoint= tipPoints[pileCode]
    pileLines[pileCode]= modelSpace.newLine(headPoint, tipPoint)

# Load soil data.
pgpSet= soil_sampled_data.SoilSampledDataSet()

# Read the soil data.
pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
soilDataFilePath= pth+'/../../../aux/soil_data/pile_geotechnical_parameters.json'
pgpSet.readFromFile(soilDataFilePath)

# Get bearing capacities.
bearingCapacities= dict()
for pileCode in selectedPiles:
    soilData= pgpSet.getSoilSampledData(pileCode)
    depth= pileLengths[pileCode]
    bearingCapacity= soilData.getPileBearingCapacityAtDepth(depth, characteristic= True)
    bearingCapacities[pileCode]= bearingCapacity

# Create pile objects.
piles= dict()
## Materials
concrete= EC2_materials.C35
for pileCode in selectedPiles:
    # Get the soil data for this pile.
    soilData= pgpSet.getSoilSampledData(pileCode)
    # Set the ground level.
    line= pileLines[pileCode]
    groundLevel= max(line.getP1().getPos.z, line.getP2().getPos.z)
    soilData.groundLevel= groundLevel
    
    # Create the pile.
    newPile= sampled_data_pytzqz_pile.PyTzQzCircularPile(E= concrete.getEcm(), diameter= 1.8, soilData= soilData, pileSet= line)
    piles[pileCode]= newPile
    
# Generate the mesh.
## Set material.
samplePile= piles[selectedPiles[0]]
xcMaterial= samplePile.defElasticShearSection3d(preprocessor)
modelSpace.setDefaultMaterial(xcMaterial)
## Set coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
modelSpace.setDefaultCoordTransf(lin)
## Set seed element.
modelSpace.newSeedElement('ElasticBeam3d')
elementSize= 1.0 # m
for pileCode in piles:
    pile= piles[pileCode]
    if(pile):
        line= pile.pileSet
        line.setElemSize(elementSize)
        line.genMesh(xc.meshDir.I)

# Create constraints.
soilSprings= dict()
for pileCode in piles:
    pile= piles[pileCode]
    if(pile):
        soilSprings[pileCode]= pile.generateSpringsPile(fixZRotationOfTipNode= True)
        
pileTopNodes= dict()
for pileCode in piles:
    pile= piles[pileCode]
    if(pile):
        ln= pile.pileSet
        nHead= ln.getP1().getNode()
        pileTopNodes[pileCode]= nHead

# Gravity load.
bearingLoadFraction= 0.98 # Fraction of the bearing load to be supported
                         # by the pile.
gLC= modelSpace.newLoadPattern(name= 'gLC')
modelSpace.setCurrentLoadPattern(gLC.name)
for pileCode in pileTopNodes:
    n= pileTopNodes[pileCode]
    bearingCapacity= bearingCapacities[pileCode]
    gravityLoad= bearingLoadFraction*bearingCapacity
    gLC.newNodalLoad(n.tag, xc.Vector([0,0,-gravityLoad,0,0,0]))
modelSpace.addLoadCaseToDomain(gLC.name) # Add to domain.

## Solve for gravity.
solProc= predefined_solutions.PlainNewtonRaphsonBandGen(feProblem, convergenceTestTol= 1e-4, maxNumIter= 20, printFlag= 0)
solProc.setup()
analysis= solProc.getAnalysis()
result= analysis.analyze(1)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)

modelSpace.calculateNodalReactions()
# Skin friction bearing.
verticalReaction= 0.0
for pileCode in piles:
    pile= piles[pileCode]
    springFixedNodes= pile.getSpringFixedNodes()
    for fixedNode in springFixedNodes:
        R= fixedNode.getReaction
        verticalReaction+= R[2]

# End bearing.
fixedTipQzNodes= pile.getPileTipQzFixedNodes()
for n in fixedTipQzNodes:
    R= n.getReaction
    verticalReaction+= R[2]
    
# Check result.
ratio1= abs(verticalReaction-gravityLoad)/gravityLoad

'''
print('Gravity load Pz= ', gravityLoad/1e3, 'kN')
print('Vertical reaction: Rz= ', verticalReaction/1e3, 'kN')
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-9:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Display results
if(not silent):
    # Graphic stuff.
    from postprocess import output_handler
    oh= output_handler.OutputHandler(modelSpace)
    # oh.displayFEMesh()
    oh.displayLoads()
    oh.displayDispRot(itemToDisp='uZ')
    oh.displayIntForcDiag(itemToDisp='N')
    # oh.displayIntForcDiag(itemToDisp='Vy')
    # oh.displayIntForcDiag(itemToDisp='Mz')
