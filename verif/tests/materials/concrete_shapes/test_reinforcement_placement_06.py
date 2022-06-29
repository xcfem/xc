# -*- coding: utf-8 -*-
''' Trivial test to check that reinforcing bars are modelled as intended
    (bottom reinforcement at the bottom and so on...).

    This test is based in the previous one but changing the orientation
    of the finite element model.
'''
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

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Define materials
## Materials.
concrete= EC2_materials.C20
steel= EC2_materials.S500C
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
pt2= points.newPoint(geom.Pos3d(0.0,b,0.0))
pt3= points.newPoint(geom.Pos3d(0.0,b,span))
pt4= points.newPoint(geom.Pos3d(0.0,0.0,span))
## Surface.
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)
s.nDivI= 3
s.nDivJ= 10

# Generate mesh.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= rcSection.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))
s.genMesh(xc.meshDir.I)

# Constraints.
fixedNodes= list()
for n in s.nodes:
    pos= n.getInitialPos3d
    if(abs(pos.z)<1e-3):
        fixedNodes.append(n)
    if(abs(pos.z-span)<1e-3):
        fixedNodes.append(n)
for n in fixedNodes:
    modelSpace.fixNode000_FFF(n.tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['load']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## load pattern.
load= xc.Vector([-20e3,0.0,0.0])
cLC= loadCaseManager.setCurrentLoadCase('load')
for e in s.elements:
    e.vector3dUniformLoadGlobal(load)

## Load combinations
combContainer= combs.CombContainer()
### ULS combination.
combContainer.ULS.perm.add('combULS01','1.6*load')
xcTotalSet= preprocessor.getSets.getSet('total')
cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.envConfig= cfg
### Save internal forces.
lsd.normalStressesResistance.saveAll(combContainer,xcTotalSet) 

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
mainBarArea= math.pi*(mainBarDiameter/2.0)**2 # Area of the reinforcement bar.
rowA= def_simple_RC_section.ReinfRow(rebarsDiam= mainBarDiameter, areaRebar= mainBarArea, rebarsSpacing= spacing, width= rcSection.b, nominalCover= cover, nominalLatCover= lateralCover)

## Second row.
rowB= def_simple_RC_section.ReinfRow(rebarsDiam= mainBarDiameter, areaRebar= mainBarArea, rebarsSpacing= spacing, width= rcSection.b-spacing, nominalCover= cover, nominalLatCover= lateralCover+spacing/2.0)

## Third row.
smallBarDiameter= 4e-3
smallBarArea= math.pi*(smallBarDiameter/2.0)**2 # Area of the reinforcement bar.
rowC= def_simple_RC_section.ReinfRow(rebarsDiam= smallBarDiameter, areaRebar= smallBarArea, rebarsSpacing= spacing, width= rcSection.b, nominalCover= cover, nominalLatCover= lateralCover+spacing/2.0)

## Store element reinforcement.
for e in s.elements:
    e.setProp("baseSection", rcSection)
    e.setProp("reinforcementUpVector", geom.Vector3d(1,0,0)) # X+
    e.setProp("reinforcementIVector", geom.Vector3d(0,0,1)) # Z+
    e.setProp("positiveReinforcementI", def_simple_RC_section.LongReinfLayers([rowA]))
    e.setProp("negativeReinforcementI", def_simple_RC_section.LongReinfLayers([rowC]))
    e.setProp("positiveReinforcementII", def_simple_RC_section.LongReinfLayers([rowC]))
    e.setProp("negativeReinforcementII", def_simple_RC_section.LongReinfLayers([rowC]))
    z= e.getPosCentroid(True).z
    if(z>1.5 and z<3.5):
        positiveReinforcementI= e.getProp("positiveReinforcementI")
        positiveReinforcementI.append(rowB)
        e.setProp("positiveReinforcementI", positiveReinforcementI)
        
#### Define sections.

# Define spatial distribution of reinforced concrete sections.
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
rcSections= reinfConcreteSectionDistribution.assignFromElementProperties(elemSet= xcTotalSet.getElements)
# for rcs in rcSections:
#     print(rcs.name, rcs.elements, rcs.positvRebarRows.getAs()*1e4, rcs.negatvRebarRows.getAs()*1e4)

#Checking normal stresses.
outCfg= lsd.VerifOutVars(listFile='N',calcMeanCF='Y')
outCfg.controller= EC2_limit_state_checking.BiaxialBendingNormalStressController('ULS_normalStress')
outCfg.controller.verbose= False # Don't display log messages.
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messagess about computation of the interaction diagram.
feProblem.errFileName= "/tmp/erase.err" # Ignore warning messagess about maximum error in computation of the interaction diagram.
meanFCs= reinfConcreteSectionDistribution.internalForcesVerification3D(lsd.normalStressesResistance,"d",outCfg)
feProblem.errFileName= "cerr" # From now on display errors if any.
feProblem.logFileName= "clog" # From now on display warnings if any.

ratio1= abs(meanFCs[0]-0.49950728265667205)/0.49950728265667205
ratio2= abs(meanFCs[1]-0.5151243799585318)/0.5151243799585318

'''
print('meanFCs= ', meanFCs)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
'''

import os
cfg.cleandirs()  # Clean after yourself.
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-4) and (ratio2<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# #########################################################
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# ## Uncomment to display the mesh
# #oh.displayFEMesh()

# ## Uncomment to display the element axes
# oh.displayLocalAxes()

# ## Uncomment to display the loads
# # oh.displayLoads()

# ## Uncomment to display the vertical displacement
# #oh.displayDispRot(itemToDisp='uY')
# #oh.displayNodeValueDiagram(itemToDisp='uX')

# ## Uncomment to display the reactions
# #oh.displayReactions()

# ## Uncomment to display the internal force
# #oh.displayIntForcDiag('Mz')
# #oh.displayIntForcDiag('Vy')
