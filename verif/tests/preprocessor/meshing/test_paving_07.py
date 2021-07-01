# -*- coding: utf-8 -*-
'''Gusset plate mesh generation using paving algorithm. Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
import matplotlib.pyplot as plt
from materials.astm_aisc import ASTM_materials
from model import predefined_spaces
from materials import typical_materials
#from postprocess import output_handler

# Test paving routine inside XC modeler.

## Problem type
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

### Define k-points.
points= modelSpace.getPointHandler()

pt94= points.newPntIDPos3d(94,geom.Pos3d(-1.82,-1.82,0.0)); pt94.setProp("labels",['baseplate', 'joint_1800'])
pt95= points.newPntIDPos3d(95,geom.Pos3d(-2.27,-1.82,0.0)); pt95.setProp("labels",['baseplate', 'joint_1800'])
pt96= points.newPntIDPos3d(96,geom.Pos3d(-2.27,-2.27,0.0)); pt96.setProp("labels",['baseplate', 'joint_1800'])
pt97= points.newPntIDPos3d(97,geom.Pos3d(-1.82,-2.27,0.0)); pt97.setProp("labels",['baseplate', 'joint_1800'])
pt98= points.newPntIDPos3d(98,geom.Pos3d(-2.09338125,-1.975,0.0)); pt98.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt99= points.newPntIDPos3d(99,geom.Pos3d(-2.09971323527,-1.95971323527,0.0)); pt99.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt100= points.newPntIDPos3d(100,geom.Pos3d(-2.115,-1.95338125,0.0)); pt100.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt101= points.newPntIDPos3d(101,geom.Pos3d(-2.13028676473,-1.95971323527,0.0)); pt101.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt102= points.newPntIDPos3d(102,geom.Pos3d(-2.13661875,-1.975,0.0)); pt102.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt103= points.newPntIDPos3d(103,geom.Pos3d(-2.13028676473,-1.99028676473,0.0)); pt103.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt104= points.newPntIDPos3d(104,geom.Pos3d(-2.115,-1.99661875,0.0)); pt104.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt105= points.newPntIDPos3d(105,geom.Pos3d(-2.09971323527,-1.99028676473,0.0)); pt105.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt106= points.newPntIDPos3d(106,geom.Pos3d(-1.95338125,-1.975,0.0)); pt106.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt107= points.newPntIDPos3d(107,geom.Pos3d(-1.95971323527,-1.95971323527,0.0)); pt107.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt108= points.newPntIDPos3d(108,geom.Pos3d(-1.975,-1.95338125,0.0)); pt108.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt109= points.newPntIDPos3d(109,geom.Pos3d(-1.99028676473,-1.95971323527,0.0)); pt109.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt110= points.newPntIDPos3d(110,geom.Pos3d(-1.99661875,-1.975,0.0)); pt110.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt111= points.newPntIDPos3d(111,geom.Pos3d(-1.99028676473,-1.99028676473,0.0)); pt111.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt112= points.newPntIDPos3d(112,geom.Pos3d(-1.975,-1.99661875,0.0)); pt112.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt113= points.newPntIDPos3d(113,geom.Pos3d(-1.95971323527,-1.99028676473,0.0)); pt113.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt114= points.newPntIDPos3d(114,geom.Pos3d(-1.95338125,-2.115,0.0)); pt114.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt115= points.newPntIDPos3d(115,geom.Pos3d(-1.95971323527,-2.09971323527,0.0)); pt115.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt116= points.newPntIDPos3d(116,geom.Pos3d(-1.975,-2.09338125,0.0)); pt116.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt117= points.newPntIDPos3d(117,geom.Pos3d(-1.99028676473,-2.09971323527,0.0)); pt117.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt118= points.newPntIDPos3d(118,geom.Pos3d(-1.99661875,-2.115,0.0)); pt118.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt119= points.newPntIDPos3d(119,geom.Pos3d(-1.99028676473,-2.13028676473,0.0)); pt119.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt120= points.newPntIDPos3d(120,geom.Pos3d(-1.975,-2.13661875,0.0)); pt120.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt121= points.newPntIDPos3d(121,geom.Pos3d(-1.95971323527,-2.13028676473,0.0)); pt121.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt122= points.newPntIDPos3d(122,geom.Pos3d(-2.09338125,-2.115,0.0)); pt122.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt123= points.newPntIDPos3d(123,geom.Pos3d(-2.09971323527,-2.09971323527,0.0)); pt123.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt124= points.newPntIDPos3d(124,geom.Pos3d(-2.115,-2.09338125,0.0)); pt124.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt125= points.newPntIDPos3d(125,geom.Pos3d(-2.13028676473,-2.09971323527,0.0)); pt125.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt126= points.newPntIDPos3d(126,geom.Pos3d(-2.13661875,-2.115,0.0)); pt126.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt127= points.newPntIDPos3d(127,geom.Pos3d(-2.13028676473,-2.13028676473,0.0)); pt127.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt128= points.newPntIDPos3d(128,geom.Pos3d(-2.115,-2.13661875,0.0)); pt128.setProp("labels",['holes', 'baseplate', 'joint_1800'])
pt129= points.newPntIDPos3d(129,geom.Pos3d(-2.09971323527,-2.13028676473,0.0)); pt129.setProp("labels",['holes', 'baseplate', 'joint_1800'])

### Define polygonal surfaces
surfaces= modelSpace.getSurfaceHandler()
f15= surfaces.newPolygonalFacePts([94, 95, 96, 97]); f15.setProp("labels",['baseplate', 'joint_1800']); f15.setProp("thickness",0.03); f15.setProp('matId','A36')
f16= surfaces.newPolygonalFacePts([98, 99, 100, 101, 102, 103, 104, 105]); f16.setProp("labels",['holes', 'baseplate', 'joint_1800']); f16.setProp("thickness",0.0)
f17= surfaces.newPolygonalFacePts([106, 107, 108, 109, 110, 111, 112, 113]); f17.setProp("labels",['holes', 'baseplate', 'joint_1800']); f17.setProp("thickness",0.0)
f18= surfaces.newPolygonalFacePts([114, 115, 116, 117, 118, 119, 120, 121]); f18.setProp("labels",['holes', 'baseplate', 'joint_1800']); f18.setProp("thickness",0.0)
f19= surfaces.newPolygonalFacePts([122, 123, 124, 125, 126, 127, 128, 129]); f19.setProp("labels",['holes', 'baseplate', 'joint_1800']); f19.setProp("thickness",0.0)

holes= [f16, f17, f18, f19]
holeDiameter= 2.0*math.sqrt(holes[0].getArea()/math.pi)

### Define material
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E=2.1e9,nu=0.3,rho= 7850,h= 0.015)

### Define template element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

### Generate mesh.
minElemSize= 1e9
for h in holes:
    h.setNDiv(1)
    minElemSize= min(minElemSize,h.getMinElemSize())
    f15.addHole(h)
f15.setElemSize(2.0*minElemSize, True)
maxElemSize= f15.getMaxElemSize()

f15.genMesh(xc.meshDir.I,True)

xcTotalSet= modelSpace.getTotalSet()
nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)
ratio1= (nNodes-420)
ratio2= (nElements-310)

'''
print("minElemSize= ",minElemSize)
print("maxElemSize= ",maxElemSize)
print(nNodes)
print(nElements)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0) & (ratio2==0) :
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

# Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)

#oh.displayBlocks()#setToDisplay= )
# oh.displayFEMesh()#setsToDisplay=[])
#oh.displayLocalAxes()
