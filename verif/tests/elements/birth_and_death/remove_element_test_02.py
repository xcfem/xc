# -*- coding: utf-8 -*-
''' Trivial test cheking element removal.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials.ec3 import EC3_materials
from materials import typical_materials
from materials.sections.fiber_section import def_simple_RC_section
from solution import predefined_solutions
from misc_utils import log_messages as lmsg
# # Graphic stuff.
# from postprocess import output_handler

#
#  1   beam      2   beam      3
#  +-------------+-------------+
#  ^             | p           ^
#                | r
#                | o to be removed.
#                | p
#                ^ 0
#

#
# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
l= 10.0
h= 5.0
n0= modelSpace.newNode(x=0.0, y= 0.0)
ii= list(range(0,int(l+1)))
xi= list()
for i in reversed(ii):
    x= float(i)
    xi.append(-x)
for i in ii[1:]:
    xi.append(float(i))
iZero= None
for i, x in enumerate(xi):
    if(x==0.0):
        iZero= i

steelBeamNodes= list()
for x in xi:
    steelBeamNodes.append(modelSpace.newNode(x=x, y= h))
    
n1= steelBeamNodes[0]
n2= steelBeamNodes[iZero]
n3= steelBeamNodes[-1]

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
# Materials definition
steel= EC3_materials.S355JR
gammaM0= 1.00
steel.gammaM= gammaM0 
beamSteelShape= EC3_materials.HEShape(steel,'HE_340_B')
beamSteelShape.sectionClass= 1
trussSteelShape= EC3_materials.SHSShape(steel= steel,name='SHS175x175x8')
beamMat= beamSteelShape.defElasticShearSection2d(preprocessor)
trussMat= typical_materials.defElasticMaterial(preprocessor, "trussMat",steel.E)

# Elements definition
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(beamMat)
steelSectionDepth= beamSteelShape.h() # Beam cross-section depth.
steelBeamElements= list()
nFrom= steelBeamNodes[0]
for nTo in steelBeamNodes[1:]:
    beam= modelSpace.newElement("ElasticBeam2d",[nFrom.tag, nTo.tag])
    beam.h= steelSectionDepth
    steelBeamElements.append(beam)
    nFrom= nTo
modelSpace.setDefaultMaterial(trussMat)
modelSpace.setElementDimension(2) # Truss defined in a two-dimensional space.
truss= modelSpace.newElement("Truss",nodeTags= [n0.tag,n2.tag])
truss.sectionArea= trussSteelShape.A()

# Constraint definition.
modelSpace.fixNode('000', n0.tag)
modelSpace.fixNode('00F', n1.tag)
modelSpace.fixNode('00F', n3.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
loadVector= xc.Vector([0.0,-2e3])
for beam in steelBeamElements:
    beam.vector2dUniformLoadGlobal(loadVector)
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
## Solve.
result= analysis.analyze(10)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)


# oh= output_handler.OutputHandler(modelSpace)
# oh.displayIntForcDiag(itemToDisp= 'Mz')
# oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)

# Add concrete elements.
## Reinforced concrete materials.
concrete= EC2_materials.C30
reinfSteel= EC2_materials.S500C
concreteSectionWidth= 1.0
concreteSectionDepth= 0.2
rcSection= def_simple_RC_section.RCRectangularSection(name='test', width= concreteSectionWidth, depth= concreteSectionDepth, concrType= concrete, reinfSteelType= reinfSteel)
xcRCSection= rcSection.defElasticShearSection2d(preprocessor)

# Create new nodes.
offset= (steelSectionDepth+concreteSectionDepth)/2.0
concreteNodes= list()
for x in xi:
    concreteNodes.append(modelSpace.newNode(x=x, y= h+offset))
## Attach them to the steel beam.
attachingElements= list()
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(beamMat)
for nDown, nTop in zip(steelBeamNodes, concreteNodes):
    attach= modelSpace.newElement("ElasticBeam2d",[nDown.tag, nTop.tag])
    attach.h= steelSectionDepth
    attachingElements.append(beam)
## Add concrete elements.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(xcRCSection)
concreteSectionDepth= concreteSectionDepth # Beam cross-section depth.
concreteElements= list()
nFrom= concreteNodes[0]
for nTo in concreteNodes[1:]:
    beam= modelSpace.newElement("ElasticBeam2d",[nFrom.tag, nTo.tag])
    beam.h= concreteSectionDepth
    concreteElements.append(beam)
    nFrom= nTo

# Create sets.
steelBeamSet= modelSpace.defSet('steelBeamSet', elements= steelBeamElements)
steelBeamSet.fillDownwards()
concreteSet= modelSpace.defSet('concreteSet', elements= concreteElements)
concreteSet.fillDownwards()

# Remove prop
trussTag= truss.tag
truss= None # Avoid calls to a non-existent element.
modelSpace.removeElement(trussTag)

# Solve again.
result= analysis.analyze(10)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)
    
testOK= (result==0)

yDisp= n2.getDisp[1]
yDispRef= -0.009442748333251186 # Check that it works as always (nothing more).
ratio= abs(yDisp-yDispRef)/abs(yDispRef)
testOK= testOK and (ratio<1e-4)

'''
print(yDisp)
print(ratio)

oh.displayIntForcDiag(setToDisplay= steelBeamSet,itemToDisp= 'N')
oh.displayIntForcDiag(setToDisplay= steelBeamSet,itemToDisp= 'Mz')
oh.displayIntForcDiag(setToDisplay= concreteSet,itemToDisp= 'N')
oh.displayIntForcDiag(setToDisplay= concreteSet,itemToDisp= 'Mz')
oh.displayDispRot(itemToDisp='uY', defFScale= 100.0)
'''

import os
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# oh.displayFEMesh()
# #oh.displayLocalAxes()
# # oh.displayStrongWeakAxis()
# oh.displayLoads()
# oh.displayReactions(reactionCheckTolerance= 1e-4)
# oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
# #oh.displayIntForcDiag(itemToDisp= 'Qy')
# #oh.displayIntForcDiag(itemToDisp= 'Mz')
# #oh.displayIntForcDiag(itemToDisp= 'T')
