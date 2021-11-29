# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

# Home made test.
__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

gravity= 9.81
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
l= 10 # Bar length
b= 0.1
A= b*b #Área in square inches.
Iz= 1/12.0*b**4 # Cross section moment of inertia (m4)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l,0)

# Materials definition
rho= 10.0
lin= modelSpace.newLinearCrdTransf("lin")
# Materials
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz;
sectionProperties.rho= rho # Material density
refLinearRho= rho*A # linear density
ratio0= abs(sectionProperties.linearRho-refLinearRho)/refLinearRho
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties)


# Element definition.
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
beam= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))

## Element mass data.
beamMassZ= beam.getTotalMassComponent(1)
beamMassRefZ= refLinearRho*l
ratio1= abs(beamMassZ-beamMassRefZ)/beamMassRefZ


constraints= preprocessor.getBoundaryCondHandler
# Zero movement for node 1.
spc1= constraints.newSPConstraint(n1.tag,0,0.0)
spc2= constraints.newSPConstraint(n1.tag,1,0.0)
spc3= constraints.newSPConstraint(n1.tag,2,0.0)
# Zero movement for node 2.
spc4= constraints.newSPConstraint(n2.tag,0,0.0)
spc5= constraints.newSPConstraint(n2.tag,1,0.0)
spc6= constraints.newSPConstraint(n2.tag,2,0.0)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
accel= xc.Vector([0,gravity])
beam.createInertiaLoad(accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

R= n2.getReaction[1]
R_ref= 0.5*beamMassRefZ*gravity
ratio2= abs(R-R_ref)/(-R_ref)

xcTotalSet= modelSpace.getTotalSet()
totalMassZ= xcTotalSet.getTotalMassComponent(1)
totalWeightZ= totalMassZ*gravity
ratio3= abs(R-0.5*totalWeightZ)/R_ref


'''
print('sectionProperties.rho= ', sectionProperties.rho)
print('sectionProperties.linearRho= ', sectionProperties.linearRho)
print('ratio0= ', ratio0)
print('beam mass: ', beamMassZ, 'kg')
print('reference beam mass: ', beamMassRefZ, 'kg')
print('ratio1= ', ratio1)
print('R= ', R)
print('R_ref= ', R_ref)
print('ratio2= ', ratio2)
print("totalMassZ= ", totalMassZ)
print("totalWeightZ= ", totalWeightZ)
print('ratio3= ', ratio3)
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-12 and abs(ratio1)<1e-12 and abs(ratio2)<1e-12 and abs(ratio3)<1e-12 :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

