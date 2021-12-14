# -*- coding: utf-8 -*-
# Home made test.

from __future__ import division
from __future__ import print_function

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

gravity= 1#9.81
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
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
elast.E= E
elast.rho= 10.0
lin= modelSpace.newLinearCrdTransf("lin")
# Materials
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz;
sectionProperties.rho= elast.rho*A # Linear rho
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties)


# Element definition.
elements= preprocessor.getElementHandler
elements.dimElem= 2 # Bidimensional space.
elements.defaultMaterial= elast.name
truss= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
truss.sectionArea= A

trussMassZ= truss.getTotalMassComponent(1)
trussMassRefZ= truss.sectionArea*elast.rho*l
ratio1= abs(trussMassZ-trussMassRefZ)/trussMassRefZ

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
truss.createInertiaLoad(accel)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

R= n2.getReaction[1]
R_ref= 0.5*trussMassRefZ*gravity
ratio2= abs(R-R_ref)/R_ref

xcTotalSet= modelSpace.getTotalSet()
totalMassZ= xcTotalSet.getTotalMassComponent(1)
totalWeightZ= totalMassZ*gravity
ratio3= abs(R-0.5*totalWeightZ)/R_ref


'''
print('truss mass: ', trussMassZ, 'kg')
print('reference truss mass: ', trussMassRefZ, 'kg')
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
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5 and abs(ratio3)<1e-5 :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

