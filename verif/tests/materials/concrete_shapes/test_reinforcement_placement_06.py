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
elem= seedElemHandler.newElement("ShellMITC4")
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
### Limit state to check.
limitState= lsd.normalStressesResistance
### Save internal forces.
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

## Second row.
rowB= def_simple_RC_section.ReinfRow(rebarsDiam= mainBarDiameter, rebarsSpacing= spacing, width= rcSection.b-spacing, nominalCover= cover, nominalLatCover= lateralCover+spacing/2.0)

## Third row.
smallBarDiameter= 4e-3
rowC= def_simple_RC_section.ReinfRow(rebarsDiam= smallBarDiameter, rebarsSpacing= spacing, width= rcSection.b, nominalCover= cover, nominalLatCover= lateralCover+spacing/2.0)

## Define reinforcement directions.
reinforcementUpVector= geom.Vector3d(1,0,0) # X+ this vector defines the meaning
                                            # of top reinforcement ot bottom
                                            # reinforcement.
reinforcementIVector= geom.Vector3d(0,0,1) # Z+ this vector defines the meaning
                                           # of reinforcement I (parallel to
                                           # this vector) and
                                           # reinforcement II (normal to this
                                           # vector)

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
    e.setProp("reinforcementUpVector", reinforcementUpVector) # X+
    e.setProp("reinforcementIVector", reinforcementIVector) # Z+
    e.setProp("bottomReinforcementI", def_simple_RC_section.LongReinfLayers([rowA]))
    e.setProp("topReinforcementI", def_simple_RC_section.LongReinfLayers([rowC]))
    e.setProp("bottomReinforcementII", def_simple_RC_section.LongReinfLayers([rowC]))
    e.setProp("topReinforcementII", def_simple_RC_section.LongReinfLayers([rowC]))
    z= e.getPosCentroid(True).z
    if(z>1.5 and z<3.5):
        bottomReinforcementI= e.getProp("bottomReinforcementI")
        bottomReinforcementI.append(rowB)
        e.setProp("bottomReinforcementI", bottomReinforcementI)
        
#### Define sections.

# Define spatial distribution of reinforced concrete sections.
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
reinfConcreteSectionDistribution.assignFromElementProperties(elemSet= xcTotalSet.getElements)

#Checking normal stresses.
## Build the controller.
controller= EC2_limit_state_checking.BiaxialBendingNormalStressController('ULS_normalStress')
controller.verbose= False # Don't display log messages.
## Perform the checking.
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messagess about computation of the interaction diagram.
feProblem.errFileName= "/tmp/erase.err" # Ignore warning messagess about maximum error in computation of the interaction diagram.
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
meanCFs= limitState.check(setCalc= None, crossSections= reinfConcreteSectionDistribution, controller= controller, listFile='N',calcMeanCF='Y', threeDim= True)
feProblem.errFileName= "cerr" # From now on display errors if any.
feProblem.logFileName= "clog" # From now on display warnings if any.

ratio1= abs(meanCFs[0]-0.49950728265667205)/0.49950728265667205
ratio2= abs(meanCFs[1]-0.5151243799585318)/0.5151243799585318

'''
print('meanCFs= ', meanCFs)
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
