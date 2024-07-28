# -*- coding: utf-8 -*-
'''Mesh refinement around loaded nodes in two phases. Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# feProblem.setVerbosityLevel(0)
CooMaxX= 2
CooMaxZ= 1
E= 20e9 # Elastic modulus
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu))
thickness= 0.2 # Cross section depth expressed in inches.
ptLoad= 400e3 # Punctual load.

import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from model.mesh import mesh_refinement

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Define materials
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

# Define materials
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat",E,nu,0.0,thickness)

modelSpace.setDefaultMaterial(shellMat)
modelSpace.newSeedElement("ShellMITC4")



## Define points.
#
#    3              4              5
#     +-------------+-------------+
#     |             |             |
#     |   quad 1    |   quad 2    |
#     |             |             |
#     |             |             |
#     +-------------+-------------+
#    0              1              2
#
points= preprocessor.getMultiBlockTopology.getPoints
p0= modelSpace.newKPoint(0.0, 0.0, 0.0)
p1= modelSpace.newKPoint(CooMaxX/2.0, 0.0, 0.0)
p2= modelSpace.newKPoint(CooMaxX, 0.0, 0.0)
p3= modelSpace.newKPoint(0.0, 0.0, CooMaxZ)
p4= modelSpace.newKPoint(CooMaxX/2.0, 0.0, CooMaxZ)
p5= modelSpace.newKPoint(CooMaxX, 0.0, CooMaxZ)

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfacePts(p0.tag, p1.tag, p4.tag, p3.tag)
s2= surfaces.newQuadSurfacePts(p1.tag, p2.tag, p5.tag, p4.tag)
xcTotalSet= modelSpace.getTotalSet()

xcTotalSet.genMesh(xc.meshDir.I)

# refine mesh around loaded/constrained points
refinementPoints= [p0, p2, p4]
radius= 0.75*CooMaxX/s1.nDivI
for p in refinementPoints:
    pos= p.getPos
    for e in xcTotalSet.elements:
        centroid= e.getPosCentroid(True)
        dist= pos.dist(centroid)
        if(dist<radius):
            e.setProp("subdivisionLevel", 2)

maxNodeSubdivisionLevel= mesh_refinement.refine_mesh(modelSpace= modelSpace, xcSet= xcTotalSet)

radius= 0.1*radius
for p in refinementPoints:
    pos= p.getPos
    for e in xcTotalSet.elements:
        centroid= e.getPosCentroid(True)
        dist= pos.dist(centroid)
        if(dist<radius):
            e.setProp("subdivisionLevel", 1)
            
maxNodeSubdivisionLevel= mesh_refinement.refine_mesh(modelSpace= modelSpace, xcSet= xcTotalSet)

# Constraints
modelSpace.fixNode('000_0F0',p0.getNode().tag)
modelSpace.fixNode('000_0F0',p2.getNode().tag)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
loadedNode= p4.getNode()
lp0.newNodalLoad(loadedNode.tag,xc.Vector([0,0,-ptLoad,0,0,0])) # Concentrated load
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.simple_static_linear(feProblem)
analOk= analysis.analyze(1)

UZ= loadedNode.getDisp[2]
nElements= len(xcTotalSet.elements)


UZTeor= -.8190066134271789e-3
ratio1= (abs((UZ-UZTeor)/UZTeor))
ratio2= (abs((nElements-648)/648))

'''
print("Central node: ", loadedNode.tag)
print("Central node coordinates: ", loadedNode.getCoo)
print("Central node displacements: ", loadedNode.getDisp)
print("UZ= ",UZ)
print("ratio1: ",ratio1)
print("Number of elements: ",nElements)
print("ratio2: ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-2) & (abs(ratio2)<1e-9):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# # oh.displayLocalAxes()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 0.1)
# oh.displayDispRot(itemToDisp='uZ', defFScale= 10)
