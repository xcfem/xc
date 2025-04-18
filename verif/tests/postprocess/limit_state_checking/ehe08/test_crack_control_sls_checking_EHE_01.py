# -*- coding: utf-8 -*-
''' Verify crack control limit state checking.

    The results seem reasonable but no tests had been found with which 
    to compare them.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import geom
import xc
from model import predefined_spaces
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking  
from materials.sections.fiber_section import def_simple_RC_section
from actions import load_cases
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess.config import default_config
from postprocess import RC_material_distribution

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Define materials
## Materials.
concrete= EHE_materials.HA30
steel= EHE_materials.B500S
## Geometry
b= 1.42
h= 0.20
## RC section.
rcSection= def_simple_RC_section.RCRectangularSection(name='BeamSection', width= 1.0, depth= h, concrType= concrete, reinfSteelType= steel)
dummySection= rcSection.defElasticMembranePlateSection(preprocessor) # Elastic membrane plate section.
# Problem geometry.
span= 5

## K-points.
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(span,0.0,0.0))
pt3= points.newPoint(geom.Pos3d(span,b,0.0))
pt4= points.newPoint(geom.Pos3d(0.0,b,0.0))
## Surface.
s= modelSpace.newQuadSurface(pt1, pt2, pt3, pt4)
s.nDivI= 10
s.nDivJ= 3
# Generate mesh.
modelSpace.setDefaultMaterial(rcSection) # Set default material.
modelSpace.newSeedElement("ShellMITC4") # Set default element type
s.genMesh(xc.meshDir.I) # Generate mesh.

# Constraints.
fixedNodes= list()
for n in s.nodes:
    pos= n.getInitialPos3d
    if(abs(pos.x)<1e-3):
        fixedNodes.append(n)
    if(abs(pos.x-span)<1e-3):
        fixedNodes.append(n)
for n in fixedNodes:
    modelSpace.fixNode000_FFF(n.tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['load']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)
## load pattern.
load= xc.Vector([0.0,0.0,-40e3])  # No "in-plane" loads (see example 06 in the same folder).
cLC= loadCaseManager.setCurrentLoadCase('load')
for e in s.elements:
    e.vector3dUniformLoadGlobal(load)

## Load combinations
combContainer= combs.CombContainer()
### ULS combination.
combContainer.SLS.freq.add('combSLS01','1.0*load')

# Compute and store load combination results.
cfg= default_config.get_temporary_env_config() # Store results in temporary files.
lsd.LimitStateData.envConfig= cfg
xcTotalSet= modelSpace.getTotalSet()
## Limit state to check.
limitState= EHE_limit_state_checking.freqLoadsCrackControl # Crack control under frequent loads.
## Save internal forces.
limitState.analyzeLoadCombinations(combContainer,xcTotalSet)

# Define reinforcement.
# Reinforcement row scheme:
#
#    |  o    o    o    o    o    o    o    o    o    o  |
#    <->           <-->                               <-> 
#    lateral      spacing                           lateral
#     cover                                          cover
#

# Geometry of the reinforcement.
nBarsA= 7 # number of bars.
cover= 0.035 # concrete cover.
lateralCover= cover # concrete cover for the bars at the extremities of the row.
spacing= (rcSection.b-2.0*lateralCover)/(nBarsA-1)

## First row.
mainBarDiameter= 25e-3 # Diameter of the reinforcement bar.
rowA= def_simple_RC_section.ReinfRow(rebarsDiam= mainBarDiameter, rebarsSpacing= spacing, width= rcSection.b, nominalCover= cover, nominalLatCover= lateralCover)

## Third row.
smallBarDiameter= 4e-3
rowC= def_simple_RC_section.ReinfRow(rebarsDiam= smallBarDiameter, rebarsSpacing= spacing, width= rcSection.b, nominalCover= cover, nominalLatCover= lateralCover+spacing/2.0)
for e in s.elements:
    e.setProp("baseSection", rcSection)
    e.setProp("reinforcementUpVector", geom.Vector3d(0,0,1)) # Z+
    e.setProp("reinforcementIVector", geom.Vector3d(1,0,0)) # X+
    e.setProp("bottomReinforcementI", def_simple_RC_section.LongReinfLayers([rowA]))
    e.setProp("topReinforcementI", def_simple_RC_section.LongReinfLayers([rowC]))
    e.setProp("bottomReinforcementII", def_simple_RC_section.LongReinfLayers([rowC]))
    e.setProp("topReinforcementII", def_simple_RC_section.LongReinfLayers([rowC]))
    
# Define spatial distribution of reinforced concrete sections.
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
reinfConcreteSectionDistribution.assignFromElementProperties(elemSet= xcTotalSet.getElements)
#reinfConcreteSectionDistribution.report()
# Checking cracking
## Build controller.
controller= limitState.getController()
controller.verbose= False #False # Don't display log messages.
## Perform checking.
meanCFs= limitState.check(setCalc= None, crossSections= reinfConcreteSectionDistribution, listFile='N',calcMeanCF='Y', controller= controller, threeDim= False)

# Check results.
# The value obtained for the 87th element of the phantom model
# has been tested against the results obtained for the same data
# with this calculation tool: https://calculocivil.com/es/ehe08/fisura/calculo
# see the readme.md file for more details.
ratio1= abs(meanCFs[0]-0.8347613436179578)/0.8347613436179578
ratio2= abs(meanCFs[1]-0.24227419443071926)/0.24227419443071926

# print(meanCFs, ratio1, ratio2)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-4) & (ratio2<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Report sections.
# reinfConcreteSectionDistribution.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)

# #########################################################
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayFEMesh()
# # Load control variables to display:
# limitState.readControlVars(modelSpace= modelSpace)
# # Display them.
# arguments= ['wk', 'CF', 'N', 'My'] # Possible arguments: 'CF','wk', 'N', 'My'
# for arg in arguments:
#     oh.displayFieldDirs1and2(limitStateLabel= limitState.label, argument= arg, setToDisplay= xcTotalSet, component=None, fileName=None, defFScale=0.0,rgMinMax= None)

cfg.cleandirs()  # Clean after yourself.
