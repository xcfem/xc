# -*- coding: utf-8 -*-
'''Test meshing of warped surfaces. Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import xc
from model import predefined_spaces
from materials import typical_materials

pos1= geom.Pos3d(0,0,1)
pos2= geom.Pos3d(1,0,0)
pos3= geom.Pos3d(1,1,1)
pos4= geom.Pos3d(0,1,0)

nDiv= 8

## Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

### Define k-points.
points= modelSpace.getPointHandler()

pt1= points.newPoint(pos1)
pt2= points.newPoint(pos2)
pt3= points.newPoint(pos3)
pt4= points.newPoint(pos4)

### Define polygonal surface
surfaces= modelSpace.getSurfaceHandler()
quadFace= surfaces.newQuadSurfacePts(pt1.tag, pt2.tag, pt3.tag, pt4.tag)

### Define material
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E=2.1e9,nu=0.3,rho= 7850,h= 0.015)

### Define template element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

### Generate mesh.
quadFace.nDivI= nDiv
quadFace.nDivJ= nDiv
quadFace.genMesh(xc.meshDir.I)

xcTotalSet= modelSpace.getTotalSet()
nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)

nNodesRef= (nDiv+1)**2
nElementsRef= nDiv**2

nNodesOk= (nNodes==nNodesRef)
nElementsOk= (nElements==nElementsRef)
   
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
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# #oh.displayBlocks()#setToDisplay= )
# oh.displayFEMesh()#setsToDisplay=[])
# oh.displayLocalAxes()
