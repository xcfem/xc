# -*- coding: utf-8 -*-
'''Base plate mesh generation using paving algorithm. Home cooked test.'''

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
from materials.astm_aisc import ASTM_materials
#from postprocess import output_handler

def getHoleAsPolygonalSurface(bolt_fastener: ASTM_materials.BoltFastener,
                            surfaces):
    ''' Create the polygonal surface that represents the hole.

    :param bolt_fastener: ASTM bolt fastener material to create the hole surface for.
    :param surfaces: XC surface handler.
    '''
    plg= bolt_fastener.getHoleAsPolygon()
    vertices= plg.getVertexList()
    newPnts= list()
    for v in vertices:
        newPnts.append(points.newPntFromPos3d(v))
    pntTags= list()
    for p in newPnts:
        pntTags.append(p.tag)
    pFace= surfaces.newPolygonalFacePts(pntTags)
    return pFace


p1= geom.Pos3d(-1.82,-1.82,0)
p2= geom.Pos3d(-2.27,-1.82,0)
p3= geom.Pos3d(-2.27,-2.27,0)
p4= geom.Pos3d(-1.82,-2.27,0)
    

bolts= list()

bolts.append(ASTM_materials.BoltFastener(diameter= .0452, pos3d= geom.Pos3d(-2.115,-2.115,0.0)))
bolts.append(ASTM_materials.BoltFastener(diameter= .0452, pos3d= geom.Pos3d(-2.115,-1.975,0.0)))
bolts.append(ASTM_materials.BoltFastener(diameter= .0452, pos3d= geom.Pos3d(-1.975,-1.975,0.0)))
bolts.append(ASTM_materials.BoltFastener(diameter= .0452, pos3d= geom.Pos3d(-1.975,-2.115,0.0)))


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


### Define polygonal surfaces
surfaces= modelSpace.getSurfaceHandler()
polyFace= surfaces.newPolygonalFacePts([pt1.tag, pt2.tag, pt3.tag, pt4.tag])
pFaceHoles= list()
for b in bolts:
    pFace= getHoleAsPolygonalSurface(b,surfaces)
    pFaceHoles.append(pFace)
    
### Define material
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E=2.1e9,nu=0.3,rho= 7850,h= 0.015)

### Define template element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

### Generate mesh.
polyFace.setNDiv(10)
for h in pFaceHoles:
    h.setNDiv(1)
    polyFace.addHole(h)
polyFace.genMesh(xc.meshDir.I, False)

xcTotalSet= modelSpace.getTotalSet()
nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)

nNodesRef= [348, 354] # Different Gmsh versions give different results
nElementsRef= [315, 321]

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

