# -*- coding: utf-8 -*-
''' Trivial test to check the computation of the center of rigidity (CoR).
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from actions import loads

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)


# 1. Create FE model.
## 1.1 Define nodes.
L= 2
n0= modelSpace.newNode(0, 0, -L)
n1= modelSpace.newNode(0, 0, 0)
n2= modelSpace.newNode(-L, 0, 0)
n3= modelSpace.newNode(L, 0, 0)

## 1.2 Define elements.
### Material properties
E= 2.1e6*9.81/1e-4 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

### Cross section properties
A= 7.64e-4 # Cross section area (m2)
Iy= 80.1e-8 # Cross section moment of inertia (m4)
Iz= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)
### Define material.
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
elementRho= 1e3
section= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "section", sectionProperties, overrideRho= elementRho)
### Coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
### Define elements.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(section)
column= modelSpace.newElement("ElasticBeam3d", [n0.tag, n1.tag])
beam1= modelSpace.newElement("ElasticBeam3d", [n1.tag, n2.tag])
beam2= modelSpace.newElement("ElasticBeam3d", [n1.tag, n3.tag])

## 1.3. Define constraints.
modelSpace.fixNode('000_000',n0.tag)
modelSpace.fixNode('000_FFF',n2.tag)
modelSpace.fixNode('000_FFF',n3.tag)

## 1.4 Define sets.
deckSet= modelSpace.defSet("deckSet", elements= [beam1, beam2])
deckSet.fillDownwards()

# 2. Compute center of gravity.
cog= deckSet.getCenterOfMass(True)
cogNearestNode= deckSet.getNearestNode(cog)
loadPointCoord= [cog.x, cog.y, cog.z]
loadedNodes= [cogNearestNode]
F= 1e2
M= F

# 3.Create unit load cases.
# 3.1 Unit force along the global X-direction at the COG.
lVector= xc.Vector([F,0,0,0,0,0]) # Fx, Fy, Fz, Mx, My, Mz
xLoadP= modelSpace.newLoadPattern(name= 'xLoadP', setCurrent= True)
xSlidingVectorLoad= loads.SlidingVectorLoad(name= 'xLoad', nodes= loadedNodes, pntCoord= loadPointCoord, loadVector= lVector)
xSlidingVectorLoad.appendLoadToCurrentLoadPattern()
# 3.2 Unit force along the global Y-direction at the COG.
lVector= xc.Vector([0,F,0,0,0,0]) # Fx, Fy, Fz, Mx, My, Mz
yLoadP= modelSpace.newLoadPattern(name= 'yLoadP', setCurrent= True)
ySlidingVectorLoad= loads.SlidingVectorLoad(name= 'yLoad', nodes= loadedNodes, pntCoord= loadPointCoord, loadVector= lVector)
ySlidingVectorLoad.appendLoadToCurrentLoadPattern()
# 3.2 Unit force along the global Y-direction at the COG.
lVector= xc.Vector([0,0,0,0,0,M]) # Fx, Fy, Fz, Mx, My, Mz
tLoadP= modelSpace.newLoadPattern(name= 'tLoadP', setCurrent= True)
tSlidingVectorLoad= loads.SlidingVectorLoad(name= 'tLoad', nodes= loadedNodes, pntCoord= loadPointCoord, loadVector= lVector)
tSlidingVectorLoad.appendLoadToCurrentLoadPattern()

R= list()
for load in [xLoadP, yLoadP, tLoadP]:
    modelSpace.addLoadCaseToDomain(load.name)
    modelSpace.analyze(1)
    disp= cogNearestNode.getDisp
    r= disp[5]
    R.append(r)
    modelSpace.removeLoadCaseFromDomain(load.name)
Rzy= R[1]
Rzx= R[0]
Rzz= R[2]
xCR= -Rzy/Rzz
yCR= -Rzx/Rzz
cor= cog+geom.Vector3d(xCR, yCR, 0.0)
dist= cor.dist(cog)
    
# print(cog)
# print(cor)
# print(dist)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(dist<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# for load in [xLoadP, yLoadP, tLoadP]:
#     modelSpace.addLoadCaseToDomain(load.name)
#     oh.displayLoads()
#     modelSpace.removeLoadCaseFromDomain(load.name)
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uZ', defFScale= 10.0)
# # oh.displayIntForc(itemToDisp='N1', defFScale= 10.0)
# # oh.displayIntForc(itemToDisp='N2', defFScale= 10.0)
# # oh.displayReactions()
