# -*- coding: utf-8 -*-
'''Use Gmsh to mesh the whole model. Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
# from postprocess import output_handler

A= 1.0
B= 2.0

pos1= geom.Pos3d(0, 0, 0)
pos2= geom.Pos3d(A, 0, 0)
pos3= geom.Pos3d(A, 0, B)
pos4= geom.Pos3d(0, 0, B)
pos5= geom.Pos3d(0, A, B)
pos6= geom.Pos3d(0, A, 0)

# Hole
pos11= geom.Pos3d(0.25, 0, 0.25)
pos12= geom.Pos3d(0.25, 0, 0.75)
pos13= geom.Pos3d(0.75, 0, 0.75)
pos14= geom.Pos3d(0.75, 0, 0.25)

## Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

### Define k-points.
points= modelSpace.getPointHandler()

pt1= points.newPntFromPos3d(pos1)
pt2= points.newPntFromPos3d(pos2)
pt3= points.newPntFromPos3d(pos3)
pt4= points.newPntFromPos3d(pos4)
pt5= points.newPntFromPos3d(pos5)
pt6= points.newPntFromPos3d(pos6)
pt11= points.newPntFromPos3d(pos11)
pt12= points.newPntFromPos3d(pos12)
pt13= points.newPntFromPos3d(pos13)
pt14= points.newPntFromPos3d(pos14)

### Define polygonal surface
surfaces= modelSpace.getSurfaceHandler()
hole= surfaces.newPolygonalFacePts([pt11.tag, pt12.tag, pt13.tag, pt14.tag])
s1= surfaces.newQuadSurfacePts(pt1.tag, pt2.tag, pt3.tag, pt4.tag)
s2= surfaces.newQuadSurfacePts(pt1.tag, pt4.tag, pt5.tag, pt6.tag)

### Define material
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E=2.1e9,nu=0.3,rho= 7850,h= 0.015)

### Define template element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

### Generate mesh.
nDiv= 6
s1.setNDiv(nDiv)
hole.setNDiv(int(nDiv/2))
s1.addHole(hole)
s2.setNDiv(nDiv)
meshSet= modelSpace.defSet('meshSet')
meshSet.surfaces.append(hole)
meshSet.surfaces.append(s1)
meshSet.surfaces.append(s2)
meshSet.useGmsh= True
#meshSet.setVerbosityLevel(10) # Avoid warning about set element tag
meshSet.genMesh(xc.meshDir.I)


nNodes= len(meshSet.nodes)
nElements= len(meshSet.elements)

nNodesRef= [163] # Different Gmsh versions give different results
nElementsRef= [142]

nNodesOk= False
if nNodes in nNodesRef:
   nNodesOk= True
nElementsOk= False
if nElements in nElementsRef:
   nElementsOk= True
   
'''
print('number of nodes: ', nNodes)
print('number of elements: ', nElements)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if nNodesOk & nElementsOk :
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

       
#Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayBlocks()#setToDisplay= )
# oh.displayFEMesh()#setsToDisplay=[])
# oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX')
# oh.displayDispRot(itemToDisp='uY')
# oh.displayDispRot(itemToDisp='uZ')
