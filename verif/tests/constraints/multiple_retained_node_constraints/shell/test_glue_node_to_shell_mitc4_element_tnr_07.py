# -*- coding: utf-8 -*-
''' Check newGlueNodeToElement function with transformation Newton-Raphson
solution algorithm.

The problem is linear, so there is no need to use a non-linear solver, BUT the 
multi-row, multi-freedom constraints gave an error with this kind of solver. 
This is a regression test for that error.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 2.1e9 # Young modulus of the steel en N/m2.
nu= 0.3 # Poisson's ratio.
h= 0.04 # Thickness (m).
rho= 7860 # Density kg/m2.

L= 1.0 # Umbrella side (m).
a= .02 # Leg side (m).
A= a*a # Leg cross-section area (m2).
nu= 0.3 # Poisson's ratio.
G= E/(2.0*(1+nu))
Iy= a**4/12 # Inertia of the leg section (m4).
Iz= Iy
J= a**4/7.0 # Cross section torsion constant (m4)

import math
import geom
import xc
from scipy.constants import g
from model import predefined_spaces
from materials import typical_materials
from postprocess.quick_inquiry import nodal_reactions
from solution import predefined_solutions

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Umbrella nodes.
halfSide= L/2.0
n1= modelSpace.newNode(-halfSide, -halfSide, 0)
n2= modelSpace.newNode(-halfSide, halfSide, 0)
n3= modelSpace.newNode(halfSide, halfSide, 0)
n4= modelSpace.newNode(halfSide, -halfSide, 0)

# Leg nodes.
legTopNode= modelSpace.newNode(0, 0, 0)
legBottomNode= modelSpace.newNode(0, 0, -1.5)
    
# Materials definition
## Umbrella.
umbrellaMat= typical_materials.defElasticMembranePlateSection(preprocessor, "umbrellaMat",E,nu,rho,h)
## Legs.
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
legMat= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "scc",sectionProperties)

# Element definition.
## Umbrella.
modelSpace.setDefaultMaterial(umbrellaMat)
umbrellaElements= list()
umbrellaElement= modelSpace.newElement("ShellMITC4", [n1.tag, n2.tag, n3.tag, n4.tag])

## Leg.
### Geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(legMat)
legElement= modelSpace.newElement("ElasticBeam3d",xc.ID([legTopNode.tag, legBottomNode.tag]))

# Glue node.
gluedDOFs= xc.ID([0,1,2,3,4,5])
glue= modelSpace.glueNodeToElement(legTopNode.tag, umbrellaElement.tag, xc.ID(gluedDOFs))

# Constraints.
modelSpace.fixNode('000_000', legBottomNode.tag)
    
# Load definition.
lpG= modelSpace.newLoadPattern(name= 'G', setCurrent= True)
accel= xc.Vector([0,0,g])
umbrellaElement.createInertiaLoad(accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lpG.name)

# Solution
# LP 16/08/2026.
# The problem is linear, so there is no need to use a non-linear solver, BUT
# the multi-row, multi-freedom constraints gave an error with this kind of
# solver. This is a regression test for that error.
analysis= predefined_solutions.transformation_newton_raphson_band_gen(feProblem, maxNumIter= 2, printFlag=0) # => OK 14/08/2026.
result= analysis.analyze(1)
if(result!= 0):
    lmsg.error("Can't solve.")
    exit(1)

nodes.calculateNodalReactions(False,1e-7)

# Check results.
gluedNodeReaction= legTopNode.getReaction
ratio1= gluedNodeReaction.Norm()
Rx= 0; Ry= 0; Rz= 0
R= legBottomNode.getReaction
Rx+= R[0]
Ry+= R[1]
Rz+= R[2]
refRz= rho*L*L*h*g
errRz= abs(Rz-refRz)/refRz

'''
print(ratio1)
print(Rx/1e3)
print(Ry/1e3)
print(Rz/1e3, refRz/1e3, errRz)
'''

testOK= (ratio1<1e-12) and (abs(Rx)<1e-12) and (abs(Ry)<1e-12) and (abs(errRz)<1e-12)
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# # oh.displayDispRot(itemToDisp= 'uZ', defFScale= 100.0)
# # oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions()

