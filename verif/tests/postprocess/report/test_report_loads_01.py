# -*- coding: utf-8 -*-
''' Verify crack control limit state checking.'''

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
from materials.sections.fiber_section import def_simple_RC_section
from actions import load_cases
from postprocess.config import default_config
from scipy.constants import g
from misc_utils import log_messages as lmsg
from postprocess.reports import report_generator

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

# Generate load report.
cfg= default_config.get_temporary_env_config()
report= report_generator.ReportGenerator(modelSpace= modelSpace, envConfig= cfg)

loadsAndSets= dict()
xcSet= modelSpace.getTotalSet()
for lcName in loadCaseNames:
    loadsAndSets[lcName]= [xcSet]
fileNames= report.loadsReport(loadCasesAndSets= loadsAndSets)

## Check results.
refFileNames= ['G1total.png', 'G2total.png', 'Qtotal.png']

testOK= (fileNames == refFileNames)

# print(fileNames, testOK)

import os
fname= os.path.basename(__file__)
if testOK:
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

cfg.cleandirs() # Clean after yourself.
