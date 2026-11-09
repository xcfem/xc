# -*- coding: utf-8 -*-
'''Check the Pile class when using sampled data instead of layers to represent
   the properties of soil.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import math
import json
import geom
import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from geotechnics.foundations import soil_sampled_data
from geotechnics.foundations import sampled_data_pile

feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) # Defines the

# Problem geometry.
pileLengths= {'P-4': 23.5, 'P-5': 32.0, 'P-6': 33.5, 'P-7': 39.0, 'P-8': 38.0, 'P-9': 40.0, 'P-10': 40.0, 'P-11': 38.0, 'P-12': 40.0, 'P-13': 40.0, 'P-14': 34.0, 'P-15': 34.0, 'P-16': 34.0, 'P-17': 36.0, 'P-18': 36.0, 'P-19': 36.0, 'P-20': 38.0, 'P-21': 36.0, 'P-22': 39.0, 'P-23': 39.0, 'P-24': 33.0, 'P-25': 33.0, 'P-26': 31.0, 'P-27': 31.0, 'P-28': 31.0+1e-3, 'P-29': 33.0, 'P-30': 33.0, 'P-31': 27.0, 'P-32': 27.0, 'E-2': 24.0}
pileHeadZ= 3.15 # z coordinate of the pile heeads.

## k-points.
headPoints= dict()
tipPoints= dict()
x= 0.0
y= 0.0
for i, pileCode in enumerate(pileLengths):
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
for pileCode in pileLengths:
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

# Create pile objects.
piles= dict()
## Materials
concrete= EC2_materials.C35
for pileCode in pileLengths:
    # Get the soil data for this pile.
    soilData= pgpSet.getSoilSampledData(pileCode)
    # Create the pile.
    line= pileLines[pileCode]
    newPile= sampled_data_pile.CircularPile(E= concrete.getEcm(), diameter= 1.8, soilData= soilData, pileSet= line)
    piles[pileCode]= newPile

# Generate the mesh.
## Set material.
firstPile= piles[next(iter(piles))]
xcMaterial= firstPile.defElasticShearSection3d(preprocessor)
modelSpace.setDefaultMaterial(xcMaterial)
## Set coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
modelSpace.setDefaultCoordTransf(lin)
## Set seed element.
modelSpace.newSeedElement('ElasticBeam3d')
elementSize= 1.0 # m
for pileCode in piles:
    pile= piles[pileCode]
    line= pile.pileSet
    line.setElemSize(elementSize)
    line.genMesh(xc.meshDir.I)


# Create constraints.
soilSprings= dict()
for pileCode in piles:
    pile= piles[pileCode]
    soilSprings[pileCode]= pile.generateSpringsPile(alphaKh_x= 1.0, alphaKh_y= 1.0, alphaKv_z= 1.0)
    # Fix z rotation on pile tip.
    ln= pile.pileSet
    nTip= ln.getP2().getNode()
    modelSpace.fixNode('FFF_FF0',nTip.tag)

# Check values.
## Load reference values.
refFilePath= pth+"/../../../aux/reference_files/"
fname= os.path.basename(__file__)
jsonFileName= refFilePath+'/ref_'+fname.replace('.py', '.json')
with open(jsonFileName, 'r') as f:
     ref_values= json.load(f)

# Convert depth string keys to floats.
tmp= dict()
for key in ref_values:
    if(key[0] in ['P', 'E']):
        values= ref_values[key]
        tmp[key]= dict()
        for depth in values:
            if(len(depth)>5):
                newDepth= round(float(depth), 2)
            else:
                newDepth= float(depth)
            tmp[key][newDepth]= ref_values[key][depth]
                
    else:
        tmp[key]= ref_values[key]
ref_values= tmp

## Check vertical stiffness at pile tip.
kV_ref= ref_values['kV_ref']
kv_err= 0.0
for pileCode in piles:
    pile= piles[pileCode]
    kV= pile.getVerticalStiffnessSinglePile()
    kV_r= kV_ref[pileCode]
    kv_err= max(kv_err, abs(kV-kV_r)/kV_r)
    
## Check horizontal stiffnesses.
kh_err= 0.0
for pileCode in piles:
    pile= piles[pileCode]
    spring_constants= pile.getLinearSpringsConstants3D(alphaKh_x= 1.0, alphaKh_y= 1.0, alphaKv_z= 1.0)
    ref_spring_constants= ref_values[pileCode]
    for nTag in spring_constants:
        n= modelSpace.getNode(nTag)
        nZ= n.getInitialPos3d.z
        nodeDepth= round(pile.getDepth(nZ), 2)
        node_spring_constants= xc.Vector(spring_constants[nTag])
        if(nodeDepth in ref_spring_constants):
            ref_depth_spring_constants= xc.Vector(ref_spring_constants[nodeDepth])
            # print(node_spring_constants, ref_depth_spring_constants, error)
        else:
            for ref_depth in ref_spring_constants:
                if(abs(ref_depth-nodeDepth)<.05):
                    ref_depth_spring_constants= xc.Vector(ref_spring_constants[ref_depth])
            
        error= (node_spring_constants-ref_depth_spring_constants).Norm()/ref_depth_spring_constants.Norm()
        kh_err= max(kh_err, error)
            
# print('kv_err= ', kv_err)
# print('kh_err= ', kh_err)
        
testOK= (abs(kv_err)<1e-4) and (abs(kh_err)<1e-4)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')



# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayFEMesh()
# oh.displayNodeValueDiagram(itemToDisp= 'k_x', caption= 'Spring stiffness x direction', defaultDirection= 'X', defaultValue= 0.0)
# oh.displayNodeValueDiagram(itemToDisp= 'k_y', caption= 'Spring stiffness y direction', defaultDirection= 'Y', defaultValue= 0.0)


