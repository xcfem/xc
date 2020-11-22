# -*- coding: utf-8 -*-
''' Inertia load on truss and elastic beam elements. 
    Equilibrium based home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
#from postprocess import output_handler


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
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# nodes.defaultTag= 1 # First node number.
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(l,0,0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
elast.E= E
elast.rho= 10.0
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,0,1]))
# Materials
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iz; sectionProperties.J= Iz
sectionProperties.rho= elast.rho
section= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "section",sectionProperties)


# Element definition.
elements= preprocessor.getElementHandler
elements.dimElem= 3 # Three-dimensional space.
elements.defaultMaterial= elast.name
truss= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
truss.sectionArea= A
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
beam= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))

# Constraints.
# Zero movement for node 1.
modelSpace.fixNode000_0FF(n1.tag)
# Partial constraint for node 2.
modelSpace.fixNode000_FFF(n2.tag)


# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
gravity= 9.81
accel= xc.Vector([0,0,gravity])
xcTotalSet= modelSpace.getTotalSet()
xcTotalSet.createInertiaLoads(accel)
#truss.createInertiaLoad(accel)
#beam.createInertiaLoad(accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

reac1= n1.getReaction
reac2= n2.getReaction
R= n2.getReaction[2]
R_ref= 0.0
R_ref+= 0.5*truss.sectionArea*truss.getMaterial().rho*l*gravity
R_ref+= 0.5*beam.sectionProperties.A*truss.getMaterial().rho*l*gravity

ratio1= abs(R-R_ref)/R_ref

'''
print('reac1= ', reac1)
print('reac2= ', reac2)
print('R= ', R)
print('R_ref= ', R_ref)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 :
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)
# #oh.displayFEMesh()
# #oh.displayLocalAxes()
# oh.displayReactions()
