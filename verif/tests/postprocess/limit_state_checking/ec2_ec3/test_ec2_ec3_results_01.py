# -*- coding: utf-8 -*-
'''Check storing and reading limit state results for EC2 and EC3 in the same
   FE model.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"

import os
import geom
import xc
from materials.sections.fiber_section import def_simple_RC_section
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.ec3 import EC3_materials
from materials.ec3 import EC3_limit_state_checking
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
nBarsA= 2 # number of bars (LP 22/03/2025).
cover= 0.035 # concrete cover.
lateralCover= cover # concrete cover for the bars at the extremities of the row.
barDiameter= 25e-3 # Diameter of the reinforcement bar.
width= (nBarsA-1)*spacing+2.0*lateralCover+barDiameter

## First row
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
### Concrete RC section.
rcBeamSection= rcSection.defElasticShearSection2d(preprocessor) # Elastic cross-section.
### Steel beam section.
steel= EC3_materials.S355JR
gammaM0= 1.00
steel.gammaM= gammaM0 
steelShape= EC3_materials.IPEShape(steel,"IPE_200")
steelShape.sectionClass= 1
steelBeamSection= steelShape.defElasticShearSection2d(preprocessor)


# Element type.
elementType= "ElasticBeam2d"

## Create mesh.
### Create RC beam nodes.
rcBeamSpan= 5
rcBeamNDiv= 10
rcBeamNodes= list()
for i in range(0,rcBeamNDiv+1):
    x= i/rcBeamNDiv*rcBeamSpan
    rcBeamNodes.append(nodes.newNodeXY(x,0.0))
    
### Create RC beam elements.
#### Geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)
#### Material.
modelSpace.setDefaultMaterial(rcBeamSection)

#### Create elements and its orientation
rcBeamElements= list()
n0= rcBeamNodes[0]
for n1 in rcBeamNodes[1:]:
    rcBeamElements.append(modelSpace.newElement(elementType,[n0.tag,n1.tag]))
    n0= n1

### Create steel beam mesh.
#### Steel beam geometry
steelBeamSpan= 4.0
##### Points.
p1= modelSpace.newKPoint(0,2.0,0)
p2= modelSpace.newKPoint(steelBeamSpan, 2.0, 0)
##### Lines
l= modelSpace.newLine(p1,p2)
l.nDiv= 6
# Mesh
modelSpace.setDefaultMaterial(steelBeamSection)
modelSpace.newSeedElement("ElasticBeam2d")
steelMesh= l.genMesh(xc.meshDir.I)

    
## Reinforced concrete beam constraints
nA= rcBeamNodes[0]
nB= rcBeamNodes[-1]
nC= rcBeamNodes[5]
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode00F(nA.tag) # First node pinned.
modelSpace.fixNodeF0F(nB.tag) # Last node pinned.

# Steel beam Constraints (cantilever beam)
modelSpace.fixNode('000', p1.getNode().tag)

# Sets.
xcTotalSet= modelSpace.getTotalSet()
rcBeamSet= modelSpace.defSet('rcBeamSet', elements= rcBeamElements)
rcBeamSet.fillDownwards()
steelBeamSet=  modelSpace.defSet('steelBeamSet', lines= [l])
steelBeamSet.fillDownwards()

## Load definition.
### Reinforced concrete beam loads.
rcLp0= modelSpace.newLoadPattern(name= 'rc0')
modelSpace.setCurrentLoadPattern(rcLp0.name)
q= -53.53e3
loadVector= xc.Vector([0.0, q])
for e in rcBeamElements:
    e.vector2dUniformLoadGlobal(loadVector)
    
### Steel beam loads.
#### Permanent action.
qz= 6e3 # Uniform load.
permanentLoad=  0.4*qz
permanentAction= modelSpace.newLoadPattern(name= 'permanentAction', setCurrent= True)
deadLoad= xc.Vector([0.0,-permanentLoad])
for e in steelBeamSet.elements:
    e.vector2dUniformLoadGlobal(deadLoad)
#### Variable action.
variableLoad= qz-permanentLoad
liveLoad= xc.Vector([0.0,-variableLoad])
variableAction=  modelSpace.newLoadPattern(name= 'variableAction', setCurrent= True)
for e in steelBeamSet.elements:
    e.vector2dUniformLoadGlobal(liveLoad)

# Load combinations
## Concrete load combinations.
ec2CombContainer= combinations.CombContainer()
ec2CombContainer.ULS.perm.add('allLoads', '1.5*'+rcLp0.name)
## Steel load combinations.
ec3CombContainer= combinations.CombContainer()
ec3CombContainer.ULS.perm.add('combULS01','1.35*permanentAction+1.50*variableAction')

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
for e in rcBeamSet.elements:
    e.setProp("baseSection", rcSection)
    e.setProp("reinforcementUpVector", reinforcementUpVector) # Z+
    e.setProp("bottomReinforcement", bottomReinfLayers)
    e.setProp("topReinforcement", topReinfLayers)

## Define RC sections for elements.
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
reinfConcreteSectionDistribution.assignFromElementProperties(elemSet= rcBeamSet.getElements)


### Check normal stresses in reinforced concrete elements.
cfg= default_config.get_temporary_env_config(subDirName= 'test_ec2_ec3_check_results')

## Set limit state to check.
ec2LimitState= EC2_limit_state_checking.normalStressesResistance
ec2LimitState.setEnvConfig(cfg)

## Compute internal forces on RC elements.
ec2LimitState.analyzeLoadCombinations(ec2CombContainer, rcBeamSet)

## Reinforced concrete elements to check.
ec2CalcSet= rcBeamSet
## Build controller.
ec2Controller= ec2LimitState.getController(biaxialBending= False)
## Perform checking.
## variables that control the output of the checking:
### setCalc: set of elements to be checked.
### crossSections: cross sections for each element.
### controller: object that controls the limit state checking.
### appendToResFile:  True if results are appended to 
###                   existing file of results (defaults to False)
### listFile: True if latex listing file of results 
###           is desired to be generated (defaults to False)
### calcMeanCF: True if average capacity factor is
###               meant to be calculated (defaults to False)
### threeDim: true if it's 3D (Fx,Fy,Fz,Mx,My,Mz) 
###           false if it's 2D (Fx,Fy,Mz).
ec2LimitState.check(setCalc= ec2CalcSet, crossSections= reinfConcreteSectionDistribution,appendToResFile=False,listFile=False,calcMeanCF=False, controller= ec2Controller, threeDim= False)

## Create EC3 Member objects.
### Cantilever beam support coefficients ky= 2-0 and k1= 0.5
beamSupportCoefs= EC3_limit_state_checking.BeamSupportCoefficients(ky= 2.0, kw= 1.0, k1= 0.5, k2= 1.0)
# Create EC3 members.
ec3Members= list() # EC3 members.
for l in steelBeamSet.getLines:
    member= EC3_limit_state_checking.Member(name= l.name, ec3Shape= steelShape, lstLines= [l], beamSupportCoefs= beamSupportCoefs)
    #member.setControlPoints()
    ec3Members.append(member)
    
## Steel elements to check.
ec3CalcSet= steelBeamSet
for member in ec3Members:
    member.installULSControlRecorder(recorderType="element_prop_recorder", calcSet= ec3CalcSet, silent= True)
ec3CalcSet.fillDownwards()

## Compute internal forces on steel elements
ec3LimitState= EC3_limit_state_checking.normalStressesResistance
ec3LimitState.analyzeLoadCombinations(combContainer= ec3CombContainer, setCalc= steelBeamSet, bucklingMembers= ec3Members)

### Build controller.
ec3Controller= ec3LimitState.getController(biaxialBending= False)
### Perform checking.
bendingAverage= ec3LimitState.check(setCalc= ec3CalcSet, appendToResFile= True, listFile=False, calcMeanCF=True, controller= ec3Controller)

## Check results.
ec2LimitState.readControlVars(modelSpace= modelSpace)

### Check reinforced concrete results.
ec2MaxCF= -1e3
for e in rcBeamSet.elements:
    ULS_normalStressesResistanceSect1= e.getProp('ULS_normalStressesResistanceSect1')
    ec2MaxCF= max(ec2MaxCF,ULS_normalStressesResistanceSect1.CF)
    ULS_normalStressesResistanceSect2= e.getProp('ULS_normalStressesResistanceSect2')
    ec2MaxCF= max(ec2MaxCF,ULS_normalStressesResistanceSect2.CF)

refEc2MaxCF= 0.9983773299214459
ratio1= abs(ec2MaxCF-refEc2MaxCF)/refEc2MaxCF

### Check steel results: get the maximum efficiency.
maxSteelBendingCF= 0.0
for e in steelBeamSet.elements:
    CF1= e.getProp('ULS_normalStressesResistanceSect1').CF
    CF2= e.getProp('ULS_normalStressesResistanceSect2').CF
    maxSteelBendingCF= max(maxSteelBendingCF, CF1, CF2)
ratio2= abs(maxSteelBendingCF-1.783045394470495)/1.783045394470495

'''
print('ec2MaxCF= ', ec2MaxCF)
print('refMaxCf= ', refEc2MaxCF)
print('ratio1= ', ratio1)
print('Steel beam maximum bending capacity factor:', maxSteelBendingCF, ratio2)
'''

fname= os.path.basename(__file__)
if (ratio1<1e-3) and (ratio2<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # #########################################################
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayFEMesh()
# # Display capacity factors on beam.
# argument= 'CF' #Possible arguments: 'CF', False, 'Mz'
# oh.displayBeamResult(attributeName= ec2LimitState.label, itemToDisp= argument, beamSetDispRes= xcTotalSet, setToDisplay= xcTotalSet)

cfg.cleandirs()  # Clean after yourself.
