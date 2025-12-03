# -*- coding: utf-8 -*-
''' Check removal of inertia loads on ElasticBeam2d elements when using
    the createInertiaLoads method of an XC set.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials

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

n1= modelSpace.newNode(0,0)
n2= modelSpace.newNode(l/2.0,0)
n3= modelSpace.newNode(l,0)

rho= 7850.0
n2Vol= 0.01
n2Mass= n2Vol*rho
n2.mass= xc.Matrix([[n2Mass,0,0],[0,n2Mass,0],[0,0,0]]) # node mass matrix.

# Materials definition
lin= modelSpace.newLinearCrdTransf("lin")
# Materials
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz;
sectionProperties.rho= rho # Material density
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties)

# Element definition.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(section)
elemA= modelSpace.newElement("ElasticBeam2d", [n1.tag,n2.tag])
elemB= modelSpace.newElement("ElasticBeam2d", [n2.tag,n3.tag])

# Constraints.
modelSpace.fixNode('00F', n1.tag)
modelSpace.fixNode('00F', n3.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
accel= xc.Vector([0,gravity])
xcTotalSet= modelSpace.getTotalSet()
inertiaLoads= xcTotalSet.createInertiaLoads(accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solve.
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)

# Check results.
R0= n1.getReaction[1]+n3.getReaction[1]
beamMassRefZ= rho*A*l+n2Mass
R_ref= beamMassRefZ*gravity
ratio0= abs(R0-R_ref)/(-R_ref)

# Remove inertia loads.
## Get nodal and elemental loads.
nodalLoadTags= list()
elementalLoadTags= list()
for load in inertiaLoads:
    typo= str(type(load))
    lTag= load.tag
    if('NodalLoad' in typo):
        nodalLoadTags.append(lTag)
    else:
        elementalLoadTags.append(lTag)
inertiaLoads.clear() # Avoid calling an object that will not exist anymore.

## Remove inertia loads.
### Nodal loads.
for nLTag in nodalLoadTags:
    lp0.removeNodalLoad(nLTag)
### Elemental loads.
for eLTag in elementalLoadTags:
    lp0.removeElementalLoad(eLTag)
    
# Solve again.
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)

# Check results.
R1= n1.getReaction[1]+n3.getReaction[1]
ratio1= abs(R1)

'''
print('n2Mass= ', n2Mass)
print('n2F= ', n2Mass*gravity)
print('R0= ', R0/1e3)
print('R_ref= ', R_ref/1e3)
print(ratio0)
print('R1= ', R1/1e3)
print(ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio0)<1e-10 and abs(ratio1)<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic output
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayLoads()
# oh.displayReactions()
