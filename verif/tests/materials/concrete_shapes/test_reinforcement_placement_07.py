# -*- coding: utf-8 -*-
''' Trivial test to check that shear reinforcing bars are modelled as intended
    (shear reinforcement in direction I is placed perpendicular to bottom reinforcement at the bottom and so on...).

'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
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
from solution import predefined_solutions

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
b= 1.0
h= 0.5
## RC section.
rcSection= def_simple_RC_section.RCRectangularSection(name='BeamSection', width= 1.0, depth= h, concrType= concrete, reinfSteelType= steel)
dummySection= rcSection.defElasticMembranePlateSection(preprocessor) # Elastic membrane plate section.


# Problem geometry.
span= 2.5

## K-points.
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0,0,0))
pt2= points.newPoint(geom.Pos3d(b,0,0))
pt3= points.newPoint(geom.Pos3d(b,span,0))
pt4= points.newPoint(geom.Pos3d(0,span,0))
## Surface.
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)
s.nDivI= 10
s.nDivJ= 20

# Generate mesh.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= rcSection.name
elem= seedElemHandler.newElement("ShellMITC4")
s.genMesh(xc.meshDir.I)

# Constraints.
fixedNodes= list()
for n in s.nodes:
    pos= n.getInitialPos3d
    if(abs(pos.y)<1e-3):
        fixedNodes.append(n)
for n in fixedNodes:
    modelSpace.fixNode000_0FF(n.tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['load']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## load pattern.
load= xc.Vector([0,0,-160e3])
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
limitState= lsd.shearResistance # Shear limit state.
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
smallBarDiameter= 10e-3
rowC= def_simple_RC_section.ReinfRow(rebarsDiam= smallBarDiameter, rebarsSpacing= spacing, width= rcSection.b, nominalCover= cover, nominalLatCover= lateralCover+spacing/2.0)

## Longitudinal shear reinforcement.
shearReinfABarDiameter= 8e-3
shearReinfABarArea=  math.pi*(shearReinfABarDiameter/2.0)**2
shearReinfA= def_simple_RC_section.ShearReinforcement(familyName= "shearReinfA", nShReinfBranches= 3, areaShReinfBranch= shearReinfABarArea, shReinfSpacing= 0.15, angAlphaShReinf= math.pi/2.0)

## Transverse shear reinforcement.
shearReinfBBarDiameter= 6e-3
shearReinfBBarArea=  math.pi*(shearReinfBBarDiameter/2.0)**2
shearReinfB= def_simple_RC_section.ShearReinforcement(familyName= "shearReinfB", nShReinfBranches= 2, areaShReinfBranch= shearReinfBBarArea, shReinfSpacing= 0.15, angAlphaShReinf= math.pi/2.0)

## Define reinforcement directions.
reinforcementUpVector= geom.Vector3d(0,0,1) # Z+ this vector defines the meaning
                                            # of top reinforcement ot bottom
                                            # reinforcement.
reinforcementIVector= geom.Vector3d(0,1,0) # Y+ this vector defines the meaning
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
    e.setProp("reinforcementUpVector", reinforcementUpVector) 
    e.setProp("reinforcementIVector", reinforcementIVector)
    e.setProp("bottomReinforcementI", def_simple_RC_section.LongReinfLayers([rowC]))
    e.setProp("topReinforcementI", def_simple_RC_section.LongReinfLayers([rowA]))
    e.setProp("bottomReinforcementII", def_simple_RC_section.LongReinfLayers([rowC]))
    e.setProp("topReinforcementII", def_simple_RC_section.LongReinfLayers([rowC]))
    y= e.getPosCentroid(True).y
    if(y<2.0):
        topReinforcementI= e.getProp("topReinforcementI")
        topReinforcementI.append(rowB)
        e.setProp("topReinforcementI", topReinforcementI)
        e.setProp('shearReinforcementI',shearReinfA)
        if(y<0.25):
            e.setProp('shearReinforcementII',shearReinfB)
        
#### Define sections.

# Define spatial distribution of reinforced concrete sections.
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
reinfConcreteSectionDistribution.assignFromElementProperties(elemSet= xcTotalSet.getElements)

class CustomSolver(predefined_solutions.PlainNewtonRaphson):

    def __init__(self, prb):
        super(CustomSolver,self).__init__(prb= prb, name= 'test', maxNumIter= 20, printFlag= 1, convergenceTestTol= 1e-3)

# Checking shear.
## Limit state to check.
limitState= EC2_limit_state_checking.shearResistance
## Build the controller.
controller= limitState.getController(solutionProcedureType= CustomSolver)
controller.verbose= False # Don't display log messages.
## Perform checking.
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
meanCFs= limitState.check(setCalc= None, crossSections= reinfConcreteSectionDistribution, listFile='N', calcMeanCF='Y', controller= controller)
feProblem.errFileName= "cerr" # From now on display errors if any.
feProblem.logFileName= "clog" # From now on display warnings if any.

relError= list() # Relative errors.
refMeanCFs= [0.08543032671148505, 0.33687225608980853]
for meanCF, refMeanCF in zip(meanCFs, refMeanCFs):
    relError.append(abs(meanCF-refMeanCF)/refMeanCF)
                    
'''
print('meanCFs= ', meanCFs)
print("relError[0]= ",relError[0])
print("relError[1]= ",relError[1])
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (relError[0]<1e-4) and (relError[1]<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Report sections.
# reinfConcreteSectionDistribution.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)

# #########################################################
# # Graphic stuff.
# from postprocess import output_handler
# from postprocess.control_vars import *
# oh= output_handler.OutputHandler(modelSpace)

# ## Uncomment to display the mesh
# #oh.displayFEMesh()

# ## Uncomment to display the element axes
# oh.displayLocalAxes()

# ## Uncomment to display the loads
# #oh.displayLoads()

# ## Uncomment to display the vertical displacement
# #oh.displayDispRot(itemToDisp='uY')
# #oh.displayNodeValueDiagram(itemToDisp='uX')

# ## Uncomment to display the reactions
# #oh.displayReactions()

# ## Uncomment to display the internal force
# #oh.displayIntForcDiag('Mz')
# #oh.displayIntForcDiag('Vy')

# # Uncomment to display the results for the limit state
# argument= 'CF'
# # Load properties to display:
# limitState.readControlVars(modelSpace= modelSpace)
# # Display results.
# oh.displayFieldDirs1and2(limitStateLabel= limitState.label,argument=argument,setToDisplay=xcTotalSet,component=None,fileName=None,defFScale=0.0,rgMinMax= None)

cfg.cleandirs()  # Clean after yourself.
