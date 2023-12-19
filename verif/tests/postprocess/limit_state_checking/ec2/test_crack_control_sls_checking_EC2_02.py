# -*- coding: utf-8 -*-
''' Test for checking the crack control SLS of a circular reinforced 
    concrete column.

    The results seem reasonable but no tests have been found with which to compare them.
'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import geom
import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking  
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from actions import load_cases
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess.config import default_config
from postprocess import RC_material_distribution
from postprocess import element_section_map

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Define problem geometry
columnHeight= 10.0
## K-points.
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(0.0,0.0,columnHeight))

## Line.
l= modelSpace.newLine(pt1, pt2)
l.setElemSize(0.5)

# Define mesh.

## Define materials
concrete= EC2_materials.C40
steel= EC2_materials.S500B

diameter= 350e-3 # Cross-section diameter [m]
cover= 0.025 # Cover [m]
section= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType= concrete, reinfSteelType= steel)
section.nDivIJ= 10
# Longitudinal reinforcement
rebarDiam= 16e-3
rebarArea= EC2_materials.Fi16
numOfRebars= 12


mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(diameter-2*cover), nominalCover= cover)])
## Put the reinforcement in the section.
section.mainReinf= mainReinf
dummySection= section.defElasticShearSection3d(preprocessor) # Elastic shear section for linear 3D elements.

# Generate mesh.
## Set default material.
modelSpace.setDefaultMaterial(dummySection)
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([1,0,0])) # Set default coordinate transformation
modelSpace.setDefaultCoordTransf(lin) # Set default element type.
modelSpace.newSeedElement("ElasticBeam3d")
l.genMesh(xc.meshDir.I) # Generate mesh.

## Constraints.
modelSpace.fixNode('000_000',pt1.getNode().tag) # Fully supported at bottom.



# Actions
## Load definition
Mx= 85.945e3
My= -Mx
Fz= -100e3 # Axial force.

loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['load']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)
## Load pattern.
cLC= loadCaseManager.setCurrentLoadCase('load')
loadVector= xc.Vector([0,0,Fz,Mx,My,0])
cLC.newNodalLoad(pt2.getNode().tag,loadVector)


# Load combinations
combContainer= combs.CombContainer()
## ULS combination.
combContainer.SLS.freq.add('combSLS01','1.0*load')

# Compute and store load combination results.
cfg= default_config.get_temporary_env_config() # Store results in temporary files.
lsd.LimitStateData.envConfig= cfg
## Save internal forces.
xcTotalSet= modelSpace.getTotalSet()
## Limit state to check.
limitState= lsd.freqLoadsCrackControl # Crack control under frequent loads.
## Save internal forces.
limitState.saveAll(combContainer,xcTotalSet)

# Define spatial distribution of reinforced concrete sections.
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sectContainer= reinfConcreteSectionDistribution.sectionDefinition # sectContainer container
## Store the section in the data structure that the "phantom model" uses
columnRCSects= element_section_map.RCMemberSection('test', [section, section])
sectContainer.append(columnRCSects)
reinfConcreteSectionDistribution.assign(elemSet= xcTotalSet.elements, setRCSects= columnRCSects)

# Check cracking
## Build controller.
controller= EC2_limit_state_checking.CrackController(limitState.label)
controller.verbose= True #False # Don't display log messages.
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
meanCFs= limitState.check(setCalc= None, crossSections= reinfConcreteSectionDistribution,listFile='N',calcMeanCF='Y', controller= controller)

#(FEcheckedModel, meanCFs)= reinfConcreteSectionDistribution.runChecking(lsd.freqLoadsCrackControl, matDiagType="k", threeDim= True, outputCfg= outCfg)

ratio1= abs(meanCFs[0]-1.0000530846623346)/1.0000530846623346
ratio2= abs(meanCFs[1]-1.0000530846614253)/1.0000530846614253

# print(meanCFs)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-4) & (ratio2<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# #########################################################
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayFEMesh()
# #Load properties to display:
# from postprocess.control_vars import *
# modelSpace.readControlVars(inputFileName= cfg.projectDirTree.getVerifCrackFreqFile())
# arguments= ['wk', 'CF', 'N', 'My', 'Mz'] # Possible arguments: 'CF','wk', 's_rmax', 'N', 'My'
# for arg in arguments:
#     oh.displayBeamResult(attributeName= limitState.label, itemToDisp= arg, setToDisplay= xcTotalSet, beamSetDispRes= xcTotalSet, fileName=None, defFScale=0.0)

cfg.cleandirs()  # Clean after yourself.
