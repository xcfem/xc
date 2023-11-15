# -*- coding: utf-8 -*-
''' Inertia load on elastic beam elements. Material created with RCCircularSection. Equilibrium based home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_column_RC_section
#from postprocess import output_handler
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Create nodes.
h= 10.0
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(0,0,h)

# Create elements.
## Coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([1,0,1]))
modelSpace.setDefaultCoordTransf(lin)

## Material
foundationConcrete= EHE_materials.HA35
reinfSteel= EHE_materials.B500S
stiffnessReductionFactor= 1
pileDiameter= 1.25
pilesRCSection= def_column_RC_section.RCCircularSection(name= 'pilesRCSection', sectionDescr= 'piles section', concrType= foundationConcrete, reinfSteelType= reinfSteel, Rext= pileDiameter/2.0)
pilesMaterial= pilesRCSection.defElasticShearSection3d(preprocessor, reductionFactor= stiffnessReductionFactor)
modelSpace.setDefaultMaterial(pilesMaterial)

## Element definition.
elements= preprocessor.getElementHandler
column= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))

## Whole model mass data.
xcTotalSet= modelSpace.getTotalSet()
massZ= xcTotalSet.getTotalMassComponent(2)
massRefZ= h*2500*math.pi*(1.25/2.0)**2
ratio1= abs(massZ-massRefZ)/massRefZ

# Constraints.
# Zero movement for node 1.
modelSpace.fixNode('000_FF0', n1.tag)
# Partial constraint for node 2.
modelSpace.fixNode('00F_FFF', n2.tag)


# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
gravity= 9.81
accel= xc.Vector([0,0,gravity])
xcTotalSet= modelSpace.getTotalSet()
modelSpace.createSelfWeightLoad(xcTotalSet, accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

R= n1.getReaction[2]
crossSectionArea= column.sectionProperties.A
refCrossSectionArea= math.pi*(pileDiameter/2.0)**2
R_ref= refCrossSectionArea*2500*h*gravity

ratio2= abs(R-R_ref)/R_ref

'''
print('cross section area, A= ', crossSectionArea, 'm2')
print('mass: ', massZ, 'kg')
print('reference mass: ', massRefZ, 'kg')
print('ratio1= ', ratio1)
print('R= ', R)
print('R_ref= ', R_ref)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-8 and abs(ratio2)<1e-8:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)
# #oh.displayFEMesh()
# #oh.displayLocalAxes()
# oh.displayReactions()
