# -*- coding: utf-8 -*-
'''Check reinforcement for in-plane stress conditions using Annex F of Eurocode 2.

Test based on the results from the article: [Deep beams reinforcement, national and Eurocode 2 design provisions](https://www.researchgate.net/publication/332229044_Deep_Beams_Reinforcement_National_and_Eurocode_2_Design_Provisions)
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess.config import default_config
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Problem geometry

l= 5 # beam span.
h= 2.5 # beam depth.
p1= modelSpace.newKPoint(0,0,0)
p2= modelSpace.newKPoint(l,0,0)
p3= modelSpace.newKPoint(2*l,0,0)
p4= modelSpace.newKPoint(2*l,h,0)
p5= modelSpace.newKPoint(l,h,0)
p6= modelSpace.newKPoint(0,h,0)

elementSize= 0.5
qs1= modelSpace.newQuadSurface(p1, p2, p5, p6)
qs1.setElemSizeIJ(elementSize,elementSize)
qs2= modelSpace.newQuadSurface(p2, p3, p4, p5)
qs2.setElemSizeIJ(elementSize,elementSize)

deepBeamSet= modelSpace.defSet('deepBeamSet', surfaces= [qs1, qs2])
deepBeamSet.fillDownwards()

# Define mesh.
## Materials definition
concrete= EC2_materials.C25
xc_concrete= concrete.defElasticIsotropicPlaneStress(preprocessor)
steel= EC2_materials.S500B

modelSpace.setDefaultMaterial(xc_concrete)
quad= modelSpace.newSeedElement('FourNodeQuad')
quad.thickness= 0.2
## Generate mesh.
deepBeamSet.genMesh(xc.meshDir.I)
## Define constraints.
#modelSpace.fixNodeF0(p1.getNode().tag)
modelSpace.fixNode00(p1.getNode().tag)
modelSpace.fixNode00(p2.getNode().tag)
modelSpace.fixNode00(p3.getNode().tag)
#modelSpace.fixNodeF0(p3.getNode().tag)

# Define loads
pressure= 240e3
## Get the lines on the top of the beam.
beamTopLines= list()
for ln in deepBeamSet.lines:
    
    centroid= ln.getCentroid()
    if(centroid.y>h/2.0):
        beamTopLines.append(ln)
## Get the nodes on the top of the beam.
beamTopNodes= dict()
for ln in beamTopLines:
    for n in ln.nodes:
        pos= n.getInitialPos3d
        beamTopNodes[pos.x]= n

## Sort on x.
beamTopNodes= dict(sorted(beamTopNodes.items()))
## Create top surface loads.
elements= preprocessor.getElementHandler
xi= list(beamTopNodes.keys())
surfaceLoads= list()
surfaceLoadTags= list()
x0= xi[0]
for x1 in xi[1:]:
    n0= beamTopNodes[x0]
    n1= beamTopNodes[x1]
    newSurfaceLoad= elements.newElement("QuadSurfaceLoad",xc.ID([n0.tag, n1.tag]))
    surfaceLoads.append(newSurfaceLoad)
    surfaceLoadTags.append(newSurfaceLoad.tag)
    x0= x1
## Create load pattern.
lpA= modelSpace.newLoadPattern('A')
eleLoad= lpA.newElementalLoad("surface_load")
eleLoad.elementTags= xc.ID(surfaceLoadTags)
eleLoad.pressure= -pressure # applied pressure loading normal to the surface, outward is positive, inward is negative.

modelSpace.addLoadCaseToDomain(lpA.name)
result= modelSpace.analyze(calculateNodalReactions= True)

# Get reactions.
RX= 0.0
RY= 0.0
for p in [p1, p2, p3]:
    n= p.getNode()
    R= n.getReaction
    RX+= R[0]
    RY+= R[1]    
RYRef= pressure*2*l
ratio1= abs(RY-RYRef)/RYRef

# Define element strengths.
sigma_c_avail= -concrete.getShearStrengthReductionFactor()*concrete.fcd()
fi8Area= math.pi*(8e-3/2.0)**2
fi10Area= math.pi*(10e-3/2.0)**2
## Geometric reinforcement ratios
steel_rho_x= 8*fi8Area/quad.thickness
steel_rho_y= steel_rho_x
## Available strenghts.
ftdx= steel_rho_x*steel.fyd()
ftdy= steel_rho_y*steel.fyd()
## Bottom reinforcement
bottom_steel_rho_x= steel_rho_x+2*fi10Area/quad.thickness
bottom_ftdx= bottom_steel_rho_x*steel.fyd()
for e in deepBeamSet.elements:
    e.setProp('ftd2_avail',ftdy)
    e.setProp('sigma_c_avail', sigma_c_avail)
    centroid= e.getPosCentroid(True)
    if(centroid.y<0.5): # element on the beam bottom
        e.setProp('ftd1_avail',bottom_ftdx)
    else:
        e.setProp('ftd1_avail',ftdx)
    
# Check limit state.
## Load combinations
combContainer= combs.CombContainer()
### Ultimate limit state.
combContainer.ULS.perm.add('combULS01','1.0*A')
### Dump combination definition into XC.
combContainer.dumpCombinations(preprocessor)
## Compute internal forces.
### Setup working directory.
cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.envConfig= cfg
### Set combinations to compute.
loadCombinations= preprocessor.getLoadHandler.getLoadCombinations
### Limit states to calculate stresses for.
limitStates= [EC2_limit_state_checking.ec2InPlaneStressResistance]
### Compute internal forces for each combination
for ls in limitStates:
    ls.analyzeLoadCombinations(combContainer, deepBeamSet)
### Check material resistance.
#### Limit state to check.
limitState= limitStates[0]

#### Build controller.
controller= limitState.getController()
#### Perform checking.
average= limitState.check(setCalc= deepBeamSet, appendToResFile='N', listFile='N', calcMeanCF='Y', controller= controller)
ratio2= abs(average[0]-0.18752881267174293)/0.18752881267174293

'''
print(average[0], ratio2)
print(sigma_c_avail)
print(steel_rho_x, ftdx)
print(steel_rho_y, ftdy)
print(bottom_steel_rho_x, bottom_ftdx)
'''

import os
fname= os.path.basename(__file__)
if (ratio1<0.01) & (ratio2<0.01):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# oh.displayFEMesh(setsToDisplay= [deepBeamSet])
# # oh.displayLocalAxes()
# oh.displayReactions(setToDisplay= deepBeamSet)
# # oh.displayLoads(setToDisplay= deepBeamSet)
# oh.displayDispRot(setToDisplay= deepBeamSet, itemToDisp='uX', defFScale= 100.0)
# oh.displayDispRot(setToDisplay= deepBeamSet, itemToDisp='uY', defFScale= 100.0)
# oh.displayStresses(setToDisplay= deepBeamSet, itemToDisp='sigma_11', defFScale= 100.0)
# oh.displayStresses(setToDisplay= deepBeamSet, itemToDisp='sigma_22', defFScale= 100.0)
# oh.displayStresses(setToDisplay= deepBeamSet, itemToDisp='sigma_12', defFScale= 100.0)
# oh.displayField(limitStateLabel= limitState.label, section= None, argument= 'CF', component= None, setToDisplay= deepBeamSet, fileName= None)
# oh.displayField(limitStateLabel= limitState.label, section= None, argument= 'ftd1_cf', component= None, setToDisplay= deepBeamSet, fileName= None)
# oh.displayField(limitStateLabel= limitState.label, section= None, argument= 'ftd2_cf', component= None, setToDisplay= deepBeamSet, fileName= None)
# oh.displayField(limitStateLabel= limitState.label, section= None, argument= 'sigma_c_cf', component= None, setToDisplay= deepBeamSet, fileName= None)

cfg.cleandirs()  # Clean after yourself.
