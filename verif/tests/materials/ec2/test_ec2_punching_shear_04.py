# -*- coding: utf-8 -*-
''' Punching shear. Computation of the beta factor.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT  AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.Ortega@ciccp.es"

import math
import geom
import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.sections import section_properties as sp
from solution import predefined_solutions
from postprocess.quick_inquiry import element_resisting_forces as erf

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

halfSide= 5 # Typical span for buildings.
## Problem geometry.
#
#                      p9
# p8 +----------------+----------------+ p6
#    |                |                |
#    |                |                |
#    |                |                |
#    |   slabs[0]     |   slabs[1]     |
#    |                |                |
#    |                |                |
#    |                |                |
#    +----------------+----------------+
#   p1                p5               p2
#
### column bottom.
p0= modelSpace.newKPoint(0,0,0)
### corners.
p1= modelSpace.newKPoint(-halfSide, -halfSide, 5)
p2= modelSpace.newKPoint(halfSide, -halfSide, 5)
### mid-points
p5= modelSpace.newKPoint(0, -halfSide, 5)
p6= modelSpace.newKPoint(halfSide, 0, 5)
p8= modelSpace.newKPoint(-halfSide, 0, 5)
### center
p9= modelSpace.newKPoint(0, 0, 5)

### column.
lColumn= modelSpace.newLine(p0, p9)
### slab
slabs= list()
slabs.append(modelSpace.newQuadSurface(p1, p5, p9, p8))
slabs.append(modelSpace.newQuadSurface(p5, p2, p6, p9))
for s in slabs:
    s.setElemSizeIJ(.2, .2)
slabArea= len(slabs)*halfSide**2

## Define sets.
slabSet= modelSpace.defSet(setName= 'slabSet', surfaces= slabs)
columnSet= modelSpace.defSet(setName= 'columnSet', lines= [lColumn])

## Define materials.
concrete= EC2_materials.C30
slabDepth= 0.35 # thickness.
slabEffectiveDepth= 0.9*slabDepth 
slabMaterial= concrete.defElasticMembranePlateSection(preprocessor= preprocessor, name= "slabMaterial", thickness= slabDepth)
columnWidth= 0.4
columnDepth= 0.6
columnMaterial= concrete.defElasticShearSection3d(preprocessor= preprocessor, sectionProperties= sp.RectangularSection(name= None, b= columnWidth, h= columnDepth))

## Generate mesh.
### Slab mesh.
modelSpace.setDefaultMaterial(slabMaterial)
modelSpace.newSeedElement('ShellMITC4')
slabSet.genMesh(xc.meshDir.I)
### Column mesh.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0])) # Coord. transf.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(columnMaterial)
modelSpace.newSeedElement('ElasticBeam3d')
lColumn.genMesh(xc.meshDir.I)
columnSet.fillDownwards()

### Constraints.
modelSpace.fixNode(DOFpattern= '000_000', nodeTag= p0.getNode().tag)
#### Simulate the columns in the perimeter.
for p in [p1, p2, p5, p6, p8]:
    modelSpace.fixNode(DOFpattern= 'FF0_FFF', nodeTag= p.getNode().tag)

### Loads.
lp0= modelSpace.newLoadPattern(name= '0', setCurrent= True)
uniformLoad= -20e3
loadVector= xc.Vector([0, 0, uniformLoad])
for e in slabSet.elements:
    e.vector3dUniformLoadGlobal(loadVector)
modelSpace.addLoadCaseToDomain(lp0.name)

# Solve
analysis= predefined_solutions.simple_static_linear(feProblem)
analOk= analysis.analyze(1)

# Compute critical perimeter.
## Get strong and weak axes.
columnElement= lColumn.elements[0]
columnStrongAxis= geom.Vector3d(columnElement.getVDirStrongAxisGlobalCoord(True).getList()) # initialGeometry= True
columnWeakAxis= geom.Vector3d(columnElement.getVDirWeakAxisGlobalCoord(True).getList()) # initialGeometry= True
## Compute column contour
pA= p9.getPos-columnWidth*columnStrongAxis-columnDepth*columnWeakAxis
pB= p9.getPos+columnWidth*columnStrongAxis-columnDepth*columnWeakAxis
pC= p9.getPos+columnWidth*columnStrongAxis+columnDepth*columnWeakAxis
pD= p9.getPos-columnWidth*columnStrongAxis+columnDepth*columnWeakAxis
columnContour= [pA, pB, pC, pD]
## Compute mid-plane of the slab
slabMidPlane= slabSet.nodes.getRegressionPlane(1.0) # 1.0 -> scale factor for the current position: 1.0 (curreentPosition= initialPosition+scaleFactor*nodeDisplacement).
## Project the column contour on the mid-plane of the slab.
projVertices= slabMidPlane.getProjection(columnContour)
projColumnContour= geom.Polygon3d(projVertices)

criticalContour= projColumnContour.getBuffer(2.0*slabEffectiveDepth)

# Compute contour control positions
controlPositions= erf.ContourControlPositions(contour= criticalContour, elemSet= slabSet, tol= 0.005)

criticalPerimeterForces= controlPositions.computeContourForces()

# Compute beta
beta= EC2_limit_state_checking.compute_punching_shear_beta(punchingPos= p9.getPos, criticalPerimeterForces= criticalPerimeterForces, criticalContourArea= criticalContour.getArea())
betaRef= 1.4 # Recommended value for edge column.
betaCheck= 1.511419915062309 # Check code consistency.
errorRef= abs(beta-betaRef)/betaRef
errorCheck= abs(beta-betaCheck)/betaCheck

testOK= (errorRef<0.1) and (errorCheck<1e-5)

'''
print('beta= ', beta)
print('betaRef= ', betaRef)
print('errorRef= ', errorRef)
print('errorCheck= ', errorCheck)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# elementsOnCriticalContour= controlPositions.elementsOnContourBorder
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# oh.displayFEMesh(setsToDisplay= [slabSet, elementsOnCriticalContour])
# # oh.displayLocalAxes()
# # oh.displayStrongWeakAxis(setToDisplay= columnSet)
# # oh.displayLoads()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)

# import matplotlib.pyplot as plt
# from mpl_toolkits.mplot3d import Axes3D
# X= list()
# Y= list()
# Z= list()
# Fx= list()
# Fy= list()
# Fz= list()

# xmin= 6.023e23
# xmax= -xmin
# ymin= xmin
# ymax= xmax
# zmin= xmin
# zmax= xmax
# for nTag in criticalPerimeterForces:
#     values= criticalPerimeterForces[nTag]
#     svs= values['sliding_vector_system']
#     F= svs.getResultant()
#     P= svs.getOrg()
#     X.append(P.x)
#     Y.append(P.y)
#     Z.append(P.z)
#     tributaryLength= values['tributary_length']
#     fx= F.x/1e3/tributaryLength
#     fy= F.y/1e3/tributaryLength
#     fz= F.z/1e3/tributaryLength
#     Fx.append(fx)
#     Fy.append(fy)
#     Fz.append(fz)
#     xmax= max(xmax, P.x)
#     xmin= min(xmin, P.x)
#     xmax= max(xmax, P.x+fx)
#     xmin= min(xmin, P.x+fx)
#     ymax= max(ymax, P.y)
#     ymin= min(ymin, P.y)
#     ymax= max(ymax, P.y+fy)
#     ymin= min(ymin, P.y+fy)
#     zmax= max(zmax, P.z)
#     zmin= min(zmin, P.z)
#     zmax= max(zmax, P.z+fz)
#     zmin= min(zmin, P.z+fz)

# fig = plt.figure()
# ax = fig.add_subplot(111, projection='3d')
# ax.quiver(X, Y, Z, Fx, Fy, Fz, arrow_length_ratio = 0.001)
# ax.set_xlim([xmin, xmax])
# ax.set_ylim([ymin, ymax])
# ax.set_zlim([zmin, zmax])
# plt.show()
