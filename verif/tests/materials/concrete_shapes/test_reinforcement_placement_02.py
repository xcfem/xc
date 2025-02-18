# -*- coding: utf-8 -*-
''' Trivial test to check that reinforcing bars are modelled as intended
    (bottom reinforcement at the bottom and so on...).

    This test is very similar to the previous one but uses a different
    technique to define the reinforcement.
'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es"

import os
from materials.sections.fiber_section import def_simple_RC_section
from materials.ec2 import EC2_materials
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from misc_utils import log_messages as lmsg

# Reinforcement row scheme:
#
#    |  o    o    o    o    o    o    o    o    o    o  |
#    <->           <-->                               <-> 
#    lateral      spacing                           lateral
#     cover                                          cover
#

# Geometry of the reinforcement.
spacing= 0.15 # spacing of reinforcement.
nBarsA= 10 # number of bars.
cover= 0.035 # concrete cover.
lateralCover= cover # concrete cover for the bars at the extremities of the row.
width= (nBarsA-1)*spacing+2.0*lateralCover

## First row.
barDiameter= 25e-3 # Diameter of the reinforcement bar.
### Reinforcement row.
rowA= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, rebarsSpacing= spacing, width= width, nominalCover= cover, nominalLatCover= lateralCover)

## Second row.
### Reinforcement row.
rowB= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, rebarsSpacing= spacing, width= width-spacing, nominalCover= cover, nominalLatCover= lateralCover+spacing/2.0)

## Concrete geometry.
## Materials.
concrete= EC2_materials.C20
steel= EC2_materials.S500C
## Geometry
b= width
h= 0.20
## RC section.
rcSection= def_simple_RC_section.RCRectangularSection(name='BeamSection', width= b, depth= h, concrType= concrete, reinfSteelType= steel)

# Define finite element problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
## Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

dummySection= rcSection.defElasticShearSection2d(preprocessor) # Elastic cross-section.

## Create mesh.
### Create nodes (backwards along the x axis to check the placement routine).
span= 5
numDiv= 10
beamNodes= list()
for i in range(0,numDiv+1):
    x= span-i/numDiv*span
    beamNodes.append(nodes.newNodeXY(x,0.0))

### Create elements.
#### Geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin")
elemHandler= preprocessor.getElementHandler
elemHandler.defaultTransformation= lin.name # Coordinate transformation for the new elements
elemHandler.defaultMaterial= dummySection.name

#### Create elements.
beamElements= list()
n0= beamNodes[0]
for n1 in beamNodes[1:]:
    beamElements.append(elemHandler.newElement("ForceBeamColumn2d",xc.ID([n0.tag,n1.tag])))
    n0= n1
    
## Constraints
nA= beamNodes[0]
nB= beamNodes[-1]
nC= beamNodes[5]
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode00F(nA.tag) # First node pinned.
modelSpace.fixNodeF0F(nB.tag) # Last node pinned.

## Define reinforcement directions.
reinforcementUpVector= geom.Vector3d(0,1,0) # Y+. This vector defines the
                                            # meaning of "top reinforcement"
                                            # or "bottom reinforcement".
## Store element reinforcement properties. Assign to each element the properties
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
#                       shear reinforcement of the section.
for e in beamElements:
    e.setProp("baseSection", rcSection)
    e.setProp("reinforcementUpVector", reinforcementUpVector) # Y+
    e.setProp("bottomReinforcement", def_simple_RC_section.LongReinfLayers([rowA]))
    e.setProp("topReinforcement", def_simple_RC_section.LongReinfLayers())
    x= e.getPosCentroid(True).x
    if(x>1.5 and x<3.5):
        bottomReinforcement= e.getProp("bottomReinforcement")
        bottomReinforcement.append(rowB)
        e.setProp("bottomReinforcement", bottomReinforcement)

#### Define sections.
rcSections= def_simple_RC_section.compute_element_rc_sections(beamElements)

##### Create corresponding XC materials.
for s in rcSections:
    s.defRCSection2d(preprocessor, matDiagType= 'k') # Create XC material.

#### Assign material to elements.
for s in rcSections:
    for tple in s.elements:
        # Each tuple has (element tag, section number).
        eTag= tple[0] # Only one section, no need to go forward.
        elem= elemHandler.getElement(eTag)
        elem.setMaterial(s.name)
        
## Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
q= -20e3
#q= 20e3
loadVector= xc.Vector([0.0, q])
for e in beamElements:
    e.vector2dUniformLoadGlobal(loadVector)
### We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem, maxNumIter= 20, convergenceTestTol= 1e-6)
result= analysis.analyze(1)
if(result!=0):
    fname= os.path.basename(__file__)
    lmsg.error(fname+' ERROR. Can\'t solve.')
    exit(1)

# If the program reaches this point the reinforcement is placed in the right
# side of the section (try to inverse the applied load and you will see that
# the solver crashes (there is no reinforcement to resist the inverse load).

# Get reactions.
nodes.calculateNodalReactions(True,1e-7) 
vDisp= xc.Vector([nC.getDisp[0],nC.getDisp[1]])
vReacA= xc.Vector([nA.getReaction[0],nA.getReaction[1]])
vReacB= xc.Vector([nB.getReaction[0],nB.getReaction[1]])


# Check results
## Check that node C is at mid-span.
halfSpan= span/2.0
ratio1= abs(nC.getCoo[0]-halfSpan)/(halfSpan)
## Check horizontal reactions.
ratio2= abs(vReacA[0]+vReacB[0])
## Check vertical reactions.
ratio3= abs(vReacA[1]+vReacB[1]+q*span)
## Check deflection.
ratio4= abs(vDisp[1]+14.187242585506766e-3)/14.187242585506766e-3

'''
print('span l= ', span, ' m')
print('width= ', width, ' m')
print('uniform load: q= ', q/1e3, 'kN/m')
print('C node position: x= ', nC.getCoo[0], 'm')
print('ratio1= ', ratio1)
print('vertical reactions: ', vReacA, vReacB)
print('ratio2= ', ratio2)
print('ratio3= ', ratio3)
print('deflection = ', vDisp[1]*1e3, 'mm')
print('ratio4= ', ratio4)
'''

fname= os.path.basename(__file__)
if (ratio1<1e-6) and (ratio2<1e-6) and (ratio3<1e-6) and (ratio4<1e-6):
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
# #oh.displayLoads()

# ## Uncomment to display the vertical displacement
# #oh.displayDispRot(itemToDisp='uY')
# #oh.displayNodeValueDiagram(itemToDisp='uX')

# ## Uncomment to display the reactions
# #oh.displayReactions()

# ## Uncomment to display the internal force
# #oh.displayIntForcDiag('Mz')
# #oh.displayIntForcDiag('Vy')

