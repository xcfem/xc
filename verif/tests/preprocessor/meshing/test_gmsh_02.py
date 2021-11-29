# -*- coding: utf-8 -*-
'''Mesh polygonal face with holes using Gmsh. Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
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

# Test paving routine inside XC modeler.

## Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

### Define k-points.
points= modelSpace.getPointHandler()

#### Exterior contour
pt1= points.newPoint(p1)
pt2= points.newPoint(p2)
pt3= points.newPoint(p3)
pt4= points.newPoint(p4)

#### Hole
ptH0= points.newPoint(pH0)
ptH1= points.newPoint(pH1)
ptH2= points.newPoint(pH2)
ptH3= points.newPoint(pH3)

### Define polygonal surfaces
surfaces= modelSpace.getSurfaceHandler()
polyFace= surfaces.newPolygonalFacePts([pt1.tag, pt2.tag, pt3.tag, pt4.tag])
hole= surfaces.newPolygonalFacePts([ptH0.tag, ptH1.tag, ptH2.tag, ptH3.tag])

### Define material
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E=2.1e9,nu=0.3,rho= 7850,h= 0.015)

### Define template element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

### Generate mesh.
polyFace.setNDiv(10)
hole.setNDiv(1)
polyFace.addHole(hole)
polyFace.genMesh(xc.meshDir.I, False)

xcTotalSet= modelSpace.getTotalSet()
nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)

nNodesRefRange= (190,230) # Different Gmsh versions give different results
nElementsRefRange= (170,200)

nNodesOk= False
if (nNodes>=nNodesRefRange[0]) and (nNodes<=nNodesRefRange[1]):
   nNodesOk= True
nElementsOk= False
if (nElements>=nElementsRefRange[0]) and (nElements<=nElementsRefRange[1]):
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
