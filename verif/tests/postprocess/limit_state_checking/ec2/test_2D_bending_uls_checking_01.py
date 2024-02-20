# -*- coding: utf-8 -*-
'''Limit state controller for 2D bending. Trivial home made test.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2023, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"

import os
import geom
import xc
from materials.sections.fiber_section import def_simple_RC_section
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from solution import predefined_solutions
from model import predefined_spaces
from actions import combinations
from postprocess import RC_material_distribution
from postprocess.config import default_config
from postprocess import limit_state_data as lsd
from misc_utils import log_messages as lmsg

# Reinforcement row scheme:
#
#    |  o    o    o    o    o    o    o    o    o    o  |
#    <->           <-->                               <-> 
#    lateral      spacing                           lateral
#     cover                                          cover
#

# Geometry of the reinforcement.
spacing= 0.1 # spacing of reinforcement.
nBarsA= 3 # number of bars.
cover= 0.035 # concrete cover.
lateralCover= cover # concrete cover for the bars at the extremities of the row.
width= (nBarsA-1)*spacing+2.0*lateralCover

## First row
barDiameter= 25e-3 # Diameter of the reinforcement bar.
### Reinforcement row.
rowA= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, rebarsSpacing= spacing, width= width, nominalCover= cover, nominalLatCover= lateralCover)

## Second row
### Reinforcement row.
barDiameter= 8e-3
rowB= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, rebarsSpacing= spacing, width= width, nominalCover= cover, nominalLatCover= lateralCover)

## Define reinforcement layers.
bottomReinfLayers= def_simple_RC_section.LongReinfLayers([rowA])
topReinfLayers= def_simple_RC_section.LongReinfLayers([rowB])
# End of the reinforcement definition.

# Define reinforced concrete rectangular section.
## Materials.
concrete= EC2_materials.C20
steel= EC2_materials.S500C
## Geometry
b= width
h= 0.70
## RC section.
rcSection= def_simple_RC_section.RCRectangularSection(name='RC section', width= b, depth= h, concrType= concrete, reinfSteelType= steel)
# End of the reinforced concrete section definition.

# Define finite element problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
## Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Materials.
eSection= rcSection.defElasticShearSection2d(preprocessor) # Elastic cross-section.
beamSection= eSection
elementType= "ElasticBeam2d"

## Create mesh.
### Create nodes.
span= 5
numDiv= 10
beamNodes= list()
for i in range(0,numDiv+1):
    x= i/numDiv*span
    beamNodes.append(nodes.newNodeXY(x,0.0))
    
### Create elements.
#### Geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin")
elemHandler= preprocessor.getElementHandler
elemHandler.defaultTransformation= lin.name # Coordinate transformation for the new elements
elemHandler.defaultMaterial= beamSection.name

#### Create elements and its orientation
# Element local "y" axis points upwards and in this circunstamces the reinforced# defined as so the reinforcement previously defined in "positvRebarRows" is
# placed in the section bottom.

beamElements= list()
n0= beamNodes[0]
for n1 in beamNodes[1:]:
    beamElements.append(elemHandler.newElement(elementType,xc.ID([n0.tag,n1.tag])))
    n0= n1
    
## Constraints
nA= beamNodes[0]
nB= beamNodes[-1]
nC= beamNodes[5]
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode00F(nA.tag) # First node pinned.
modelSpace.fixNodeF0F(nB.tag) # Last node pinned.

## Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
q= -55e3
loadVector= xc.Vector([0.0, q])
for e in beamElements:
    e.vector2dUniformLoadGlobal(loadVector)

# Load combinations
combContainer= combinations.CombContainer()
combContainer.ULS.perm.add('allLoads', '1.5*'+lp0.name)
xcTotalSet= preprocessor.getSets.getSet('total')

# Check RC sections for bending.
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
# - bottomReinforcement: LongReinfLayers objects defining the 
#                        reinforcement at the bottom of the section.
# - topReinforcement: LongReinfLayers objects defining the 
#                     reinforcement at the top of the section.
# - shearReinforcement: ShearReinforcement objects defining the 
#                       reinforcement at the bottom of the section.
reinforcementUpVector= geom.Vector3d(0,1,0) # Y+ this vector defines the meaning
                                            # of top reinforcement ot bottom
                                            # reinforcement.
for e in xcTotalSet.elements:
    e.setProp("baseSection", rcSection)
    e.setProp("reinforcementUpVector", reinforcementUpVector) # Z+
    e.setProp("bottomReinforcement", bottomReinfLayers)
    e.setProp("topReinforcement", topReinfLayers)

## Define RC sections for elements.
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
reinfConcreteSectionDistribution.assignFromElementProperties(elemSet= xcTotalSet.getElements)

## Compute internal forces.
cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.envConfig= cfg
lsd.normalStressesResistance.analyzeLoadCombinations(combContainer,xcTotalSet)

## Check normal stresses.
## Limit state to check.
limitState= lsd.normalStressesResistance
## Elements to check.
setCalc= xcTotalSet
## Build controller.
controller= EC2_limit_state_checking.UniaxialBendingNormalStressController(limitState.label)
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
modelSpace.readControlVars(inputFileName= cfg.projectDirTree.getVerifNormStrFile())
maxCF= -1e3
for e in xcTotalSet.elements:
    ULS_normalStressesResistanceSect1= e.getProp('ULS_normalStressesResistanceSect1')
    maxCF= max(maxCF,ULS_normalStressesResistanceSect1.CF)
    ULS_normalStressesResistanceSect2= e.getProp('ULS_normalStressesResistanceSect2')
    maxCF= max(maxCF,ULS_normalStressesResistanceSect2.CF)

ratio1= abs(maxCF-0.9983882615059927)/0.9983882615059927


'''
print(maxCF)
print(ratio1)
'''

fname= os.path.basename(__file__)
if (ratio1<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # #########################################################
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayFEMesh()
# #Load properties to display:
# argument= 'CF' #Possible arguments: 'CF', 'N', 'Mz'
# oh.displayBeamResult(attributeName=lsd.normalStressesResistance.label, itemToDisp= argument, beamSetDispRes= xcTotalSet, setToDisplay= xcTotalSet)

cfg.cleandirs()  # Clean after yourself.
