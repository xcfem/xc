# -*- coding: utf-8 -*-
''' Reinforcement check for a skew slab with orthogonal reinforcement layout.
    The objective of the test is to show how to use the 'theta' property of the
    elements to express the internals forces in the axes of the reinforcement.
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from scipy.constants import g
from model import predefined_spaces
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from actions import load_cases
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess.config import default_config
from postprocess import element_section_map
from postprocess import RC_material_distribution

# Problem type
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Define materials
## Materials.
concrete= EHE_materials.HA30
steel= EHE_materials.B500S
## Geometry
h= 0.50 # slab depth
## RC section.
rcSection= def_simple_RC_section.RCRectangularSection(name='SlabSection', width= 1.0, depth= h, concrType= concrete, reinfSteelType= steel)
dummySection= rcSection.defElasticMembranePlateSection(preprocessor) # Elastic membrane plate section.
## Reinforcement direction.
slabReinforcementDirection= geom.Vector3d(3.75,-2.1651,0).normalized() # Defines the direcion of the first reinforcement section (section 1).

# Problem geometry.
## K-points.
ptA= modelSpace.newKPoint(0.0,0.0,-1.0)
ptB= modelSpace.newKPoint(5,0.0,-1.0)

pt1= modelSpace.newKPoint(0.0,0.0,0.0)
pt2= modelSpace.newKPoint(5.0,0.0,0.0)
pt3= modelSpace.newKPoint(10, 8.6603,0)
pt4= modelSpace.newKPoint(5, 8.6603,0)

ptC= modelSpace.newKPoint(10,8.6603,-1.0)
ptD= modelSpace.newKPoint(5,8.6603,-1.0)

## Surfaces.
### we use a rough mesh (n= 1) to get a solution quickly
### (the precision of the results is not important in
### this test). Use n=8 to get a finer mesh
n= 1
s0= modelSpace.newQuadSurface(ptA, ptB, pt2, pt1)
s0.nDivI= 5*n
s1= modelSpace.newQuadSurface(pt1, pt2, pt3, pt4)
s1.nDivI= 5*n
s1.nDivJ= 10*n
s2= modelSpace.newQuadSurface(ptD, ptC, pt3, pt4)
s2.nDivI= 5*n

xcTotalSet= modelSpace.getTotalSet()
# Mesh generation.
modelSpace.setDefaultMaterial(dummySection)
modelSpace.newSeedElement("ShellMITC4")

xcTotalSet.genMesh(xc.meshDir.I)

# Constraints.
fixedNodes= list()
for n in s0.nodes+s2.nodes:
    pos= n.getInitialPos3d
    if(abs(pos.z+1.0)<1e-3):
        fixedNodes.append(n)
for n in fixedNodes:
    pos= n.getInitialPos3d
    if(abs(pos.y)<1e-3):
        modelSpace.fixNode('000_FFF', n.tag)
    else:
        modelSpace.fixNode('FF0_FFF', n.tag)
    
# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['load']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## load pattern.
selfWeight= h*2500*g
liveLoad= 20e3
totalLoad= selfWeight+liveLoad
load= xc.Vector([0.0,0.0,-totalLoad])
cLC= loadCaseManager.setCurrentLoadCase('load')
for e in s1.elements:
    e.vector3dUniformLoadGlobal(load)

## Load combinations
combContainer= combs.CombContainer()
### ULS combination.
combContainer.ULS.perm.add('combULS01','1.6*load')

# Reinforced concrete sections on each element.

## Define RC sections.
slabSections= element_section_map.RCSlabBeamSection(name= "slab", sectionDescr= "RC slab.", concrType= concrete, reinfSteelType= steel, depth= h)
slabSections.dir1PositvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 10e-3, rebarsSpacing= 0.20, nominalCover= 0.035)])
slabSections.dir1NegatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 25e-3, rebarsSpacing= 0.20, nominalCover= 0.035)])
slabSections.dir2PositvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 10e-3, rebarsSpacing= 0.20, nominalCover= 0.035+20e-3)])
slabSections.dir2NegatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 25e-3, rebarsSpacing= 0.20, nominalCover= 0.035+20e-3)])

# Display sections geometry
# slabSections.createSections()
# slabSections.plot(preprocessor= preprocessor, matDiagType= 'k')


reinfConcreteSections= RC_material_distribution.RCMaterialDistribution()
sections= reinfConcreteSections.sectionDefinition # Get the sections container.
sections.append(slabSections) # Append the sections to the container.
for e in s1.elements:
    eTag= e.tag # element identifier.
    reinfConcreteSections.sectionDistribution[eTag]= slabSections.getSectionNames()
    reinfConcreteSections.sectionDistribution.elementDimension[eTag]= e.getDimension
    # Compute skew angle of the reinforcement.
    iVector= e.getIVector3d(True)
    reinforcementSkewAngle= iVector.getAngle(slabReinforcementDirection)
    e.setProp('theta', reinforcementSkewAngle)

# Check limit state.
cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.envConfig= cfg

## Set limit state to check.
limitState= lsd.normalStressesResistance # normal stresses.

## Define set of elements to check.
setCalc= modelSpace.defSet(setName= 'setCalc', surfaces= [s1])
setCalc.fillDownwards() # populate with the elements of the face.

# Compute internal forces for each load combination.
limitState.analyzeLoadCombinations(combContainer= combContainer, setCalc= setCalc) 

## Set limit state to check.
limitState= lsd.normalStressesResistance # normal stresses.
## Build controller.
controller= EHE_limit_state_checking.BiaxialBendingNormalStressController(limitState.label)
## Perform checking.
feProblem.errFileName= "/tmp/erase.err" # Ignore warning messagess about maximum error in computation of the interaction diagram.
meanCFs= limitState.check(setCalc= None, crossSections= reinfConcreteSections, listFile='N', calcMeanCF='Y', threeDim= False, controller= controller)
feProblem.errFileName= "cerr" # From now on display errors if any.

# Check results.
meanCF0Teor= 0.7344421404092445
ratio1= abs(meanCFs[0]-meanCF0Teor)/meanCF0Teor
meanCF1Teor= 0.8458379072588967
ratio2= abs(meanCFs[1]-meanCF1Teor)/meanCF1Teor

'''
print(meanCFs)
print(ratio1, ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<0.01) & (ratio2<0.01):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # # oh.displayBlocks()
# # oh.displayFEMesh()
# oh.displayLocalAxes()
# oh.displayReactions()
# # # oh.displayLoads()
# # # oh.displayReactions()
# # # oh.displayDispRot(itemToDisp='uX')
# # # oh.displayDispRot(itemToDisp='uY')
# # # oh.displayDispRot(itemToDisp='uZ')

# # Read check results.
# limitState.readControlVars(modelSpace= modelSpace)
# # Display them.
# oh.displayFieldDirs1and2(limitStateLabel= limitState.label, argument= 'My', setToDisplay= xcTotalSet, component=None, fileName=None, defFScale=0.0,rgMinMax= None)

cfg.cleandirs() # Clean after yourself.
