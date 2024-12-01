# -*- coding: utf-8 -*-
''' Verify normal stresses limit state checking.'''

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
from misc_utils import log_messages as lmsg

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
span= 3.75
b= 5.0*span # simply supported conditions.

## K-points.
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(span,0.0,0.0))
pt3= points.newPoint(geom.Pos3d(span,b,0.0))
pt4= points.newPoint(geom.Pos3d(0.0,b,0.0))

## Surface.
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt4.tag,pt3.tag,pt2.tag,pt1.tag)
s.setElemSizeIJ(0.4, 0.4)

# Generate mesh.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= rcSection.name
elem= seedElemHandler.newElement("ShellMITC4")
s.genMesh(xc.meshDir.I)

# Constraints.
fixedNodesA= list()
fixedNodesB= list()
for n in s.nodes:
    pos= n.getInitialPos3d
    if(abs(pos.x)<1e-3):
        fixedNodesA.append(n)
    if(abs(pos.x-span)<1e-3):
        fixedNodesB.append(n)
for n in fixedNodesA:
    modelSpace.fixNode000_FFF(n.tag)
for n in fixedNodesB:
    modelSpace.fixNodeF00_FFF(n.tag)
    
# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['G1', 'G2', 'Q']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)
## load patterns.
### Self weight.
accel= xc.Vector([0,0,g])
cLC= loadCaseManager.setCurrentLoadCase('G1')
for e in s.elements:
    e.createInertiaLoad(accel)
    
### Permanent load.
g2Load= xc.Vector([0.0,0.0,-1e3])
cLC= loadCaseManager.setCurrentLoadCase('G2')
for e in s.elements:
    e.vector3dUniformLoadGlobal(g2Load)

### Variable load.
qLoad= xc.Vector([0.0,0.0,-3e3])
cLC= loadCaseManager.setCurrentLoadCase('Q')
for e in s.elements:
    e.vector3dUniformLoadGlobal(qLoad)
    
## Load combinations
combContainer= combs.CombContainer()
### ULS combination.
combContainer.ULS.perm.add('ULS01','1.35*G1+1.35*G2+1.5*Q')

# Define reinforcement.
# Reinforcement row scheme:
#
#    |  o    o    o    o    o    o    o    o    o    o  |
#    <->           <-->                               <-> 
#    lateral      spacing                           lateral
#     cover                                          cover
#

# Geometry of the reinforcement.
nBarsA= 4 # number of bars.
cover= 0.030 # concrete cover.
lateralCover= 0.125 # concrete cover for the bars at the extremities of the row.
spacingI= (rcSection.b-2.0*lateralCover)/(nBarsA-1)


## Bottom reinforcement I.
mainBarDiameter= 12e-3 # Diameter of the reinforcement bar.
bottomReinfI= def_simple_RC_section.ReinfRow(rebarsDiam= mainBarDiameter, rebarsSpacing= spacingI, width= rcSection.b, nominalCover= cover, nominalLatCover= lateralCover)

## BottomReinforcement II.
spacingII= 0.45
lateralCoverII= cover
bottomReinfII= def_simple_RC_section.ReinfRow(rebarsDiam= mainBarDiameter, rebarsSpacing= spacingII, width= rcSection.b, nominalCover= cover, nominalLatCover= lateralCoverII)

## Top reinforcement.
smallBarDiameter= 4e-3
topReinf= def_simple_RC_section.ReinfRow(rebarsDiam= smallBarDiameter, rebarsSpacing= spacingII, width= rcSection.b, nominalCover= cover, nominalLatCover= lateralCoverII)

## Store element reinforcement. Assign to each element the properties
# that will be used to define its reinforcement on each direction:
#
# - baseSection: RCSectionBase derived object containing the geometry
#                and the material properties of the reinforcec concrete
#                section.
# - reinforcementUpVector: reinforcement "up" direction which defines
#                          the position of the positive reinforcement
#                          (bottom) and the negative reinforcement
#                          (up).
# - reinforcementIVector: (for slabs) direction corresponding to 
#                         the first RC section
# - bottomReinforcement: LongReinfLayers objects defining the 
#                        reinforcement at the bottom of the section.
# - topReinforcement: LongReinfLayers objects defining the 
#                     reinforcement at the top of the section.
# - shearReinforcement: ShearReinforcement objects defining the 
#                       reinforcement at the bottom of the section.
for e in s.elements:
    e.setProp("baseSection", rcSection)
    e.setProp("reinforcementUpVector", geom.Vector3d(0,0,1)) # Z+
    e.setProp("reinforcementIVector", geom.Vector3d(1,0,0)) # X+
    e.setProp("bottomReinforcementI", def_simple_RC_section.LongReinfLayers([bottomReinfI]))
    e.setProp("topReinforcementI", def_simple_RC_section.LongReinfLayers([topReinf]))
    e.setProp("bottomReinforcementII", def_simple_RC_section.LongReinfLayers([bottomReinfII]))
    e.setProp("topReinforcementII", def_simple_RC_section.LongReinfLayers([topReinf]))
    
#### Define spatial distribution of reinforced concrete sections.
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
xcTotalSet= modelSpace.getTotalSet()
reinfConcreteSectionDistribution.assignFromElementProperties(elemSet= xcTotalSet.getElements)

## Limit state to check.
limitState= EC2_limit_state_checking.normalStressesResistance

cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.envConfig= cfg
# Compute internal forces.
limitState.analyzeLoadCombinations(combContainer,xcTotalSet)

# Check normal stresses.
## Elements to check.
setCalc= xcTotalSet
## Build controller.
controller= limitState.getController(biaxialBending= True)

## Perform checking.
## variables that control the output of the checking:
### setCalc: set of elements to be checked.
### crossSections: cross sections for each element.
### controller: object that controls the limit state checking.
### appendToResFile:  'Yes','Y','y',.., if results are appended to 
###                   existing file of results (defaults to 'N')
### listFile: 'Yes','Y','y',.., if latex listing file of results 
###           is desired to be generated (defaults to 'N')
### calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
###               meant to be calculated (defaults to 'N')
### threeDim: true if it's 3D (Fx,Fy,Fz,Mx,My,Mz) 
###           false if it's 2D (Fx,Fy,Mz).
limitState.check(setCalc= setCalc, crossSections= reinfConcreteSectionDistribution,appendToResFile='N',listFile='N',calcMeanCF='N', controller= controller, threeDim= False)

## Check results.
### Read the control variables.
limitState.readControlVars(modelSpace= modelSpace)
### Compute the maximum efficiency.
maxCF= -1e3
for e in xcTotalSet.elements:
    controlVarsSect1= e.getProp(limitState.label+'Sect1')
    maxCF= max(maxCF,controlVarsSect1.CF)
    controlVarsSect2= e.getProp(limitState.label+'Sect2')
    maxCF= max(maxCF,controlVarsSect2.CF)

ratio1= abs(maxCF-0.9135590413756935)/0.9135590413756935

'''
print(maxCF)
print(ratio1)
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Report sections.
# reinfConcreteSectionDistribution.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)

# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# # oh.displayFEMesh()
# oh.displayLocalAxes()
# # oh.displayReactions()
# # for load in loadCaseNames:
# #     modelSpace.addLoadCaseToDomain(load)
# #     oh.displayLoads()
# #     modelSpace.removeLoadCaseFromDomain(load)
# # # oh.displayReactions()
# # # oh.displayDispRot(itemToDisp='uX')
# # # oh.displayDispRot(itemToDisp='uY')
# # # oh.displayDispRot(itemToDisp='uZ')
# # limitState.readControlVars(modelSpace= modelSpace)
# #  oh.displayFieldDirs1and2(limitStateLabel= limitState.label, argument= 'CF', setToDisplay= xcTotalSet, component=None, fileName=None, defFScale=0.0,rgMinMax= None)

cfg.cleandirs() # Clean after yourself.
