# -*- coding: utf-8 -*-
'''Home cooked test.'''

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
import matplotlib.pyplot as plt
from model import predefined_spaces
from materials import typical_materials
#from postprocess import output_handler

p1= geom.Pos3d(0,0,0)
p2= geom.Pos3d(10,0,0)
p3= geom.Pos3d(10,10,0)
p4= geom.Pos3d(0,10,0)

pH0= geom.Pos3d(4,4,0)
pH1= geom.Pos3d(5,4,0)
pH2= geom.Pos3d(5,5,0)
pH3= geom.Pos3d(4,5,0)

pH4= geom.Pos3d(1,3,0)
pH5= geom.Pos3d(2,2,0)
pH6= geom.Pos3d(3,3,0)
pH7= geom.Pos3d(2,4,0)

## Problem type
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

### Define k-points.
points= modelSpace.getPointHandler()

#### Exterior contour
pt1= points.newPntFromPos3d(p1)
pt2= points.newPntFromPos3d(p2)
pt3= points.newPntFromPos3d(p3)
pt4= points.newPntFromPos3d(p4)

#### Holes
ptH0= points.newPntFromPos3d(pH0)
ptH1= points.newPntFromPos3d(pH1)
ptH2= points.newPntFromPos3d(pH2)
ptH3= points.newPntFromPos3d(pH3)
ptH4= points.newPntFromPos3d(pH4)
ptH5= points.newPntFromPos3d(pH5)
ptH6= points.newPntFromPos3d(pH6)
ptH7= points.newPntFromPos3d(pH7)

### Define polygonal surfaces
surfaces= modelSpace.getSurfaceHandler()
polyFace= surfaces.newPolygonalFacePts([pt1.tag, pt2.tag, pt3.tag, pt4.tag])
holeA= surfaces.newPolygonalFacePts([ptH0.tag, ptH1.tag, ptH2.tag, ptH3.tag])
holeB= surfaces.newPolygonalFacePts([ptH4.tag, ptH5.tag, ptH6.tag, ptH7.tag])

### Define material
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E=2.1e9,nu=0.3,rho= 7850,h= 0.015)

### Define template element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

### Generate mesh.
polyFace.setNDiv(10)
holeA.setNDiv(1)
holeB.setNDiv(1)
polyFace.addHole(holeA)
polyFace.addHole(holeB)
polyFace.genMesh(xc.meshDir.I, False)

xcTotalSet= modelSpace.getTotalSet()
nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)

nNodesRef= [206, 207, 215] # Different Gmsh versions give different results
nElementsRef= [179, 180, 188]

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

# Graphic stuff.
#oh= output_handler.OutputHandler(modelSpace)

#oh.displayBlocks()#setToDisplay= )
#oh.displayFEMesh()#setsToDisplay=[])
#oh.displayLocalAxes()
