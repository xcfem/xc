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
#from postprocess import output_handler

A= 10.0

pos1= geom.Pos3d(0,0,0)
pos2= geom.Pos3d(A,0,0)
pos3= geom.Pos3d(A,A,0)
pos4= geom.Pos3d(0,A,0)

nDiv= 10

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

### Define polygonal surface
surfaces= modelSpace.getSurfaceHandler()
face= surfaces.newQuadSurfacePts(pt1.tag, pt2.tag, pt3.tag, pt4.tag)

### Define material
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E=2.1e9,nu=0.3,rho= 7850,h= 0.015)

### Define template element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

### Generate mesh.
face.setNDiv(nDiv)
xcTotalSet= modelSpace.getTotalSet()
xcTotalSet.useGmsh= True
xcTotalSet.genMesh(xc.meshDir.I)

nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)

nNodesRef= [140] # Different Gmsh versions give different results
nElementsRef= [119]

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
