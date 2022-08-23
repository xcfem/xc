# Home made test
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

#             8
#  7 +--------+--------+ 9
#    |        |        |
#    |   3    |   4    |
#    |        |        |
#  4 +--------+--------+ 6
#    |       5|        |
#    |   1    |   2    |
#    |        |        |
#  1 +--------+--------+ 3
#             2
#

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(1.0,0.0,0.0))
pt3= points.newPoint(geom.Pos3d(2.0,0.0,0.0))
pt4= points.newPoint(geom.Pos3d(0.0,1.0,0.0))
pt5= points.newPoint(geom.Pos3d(1.0,1.0,0.0))
pt6= points.newPoint(geom.Pos3d(2.0,1.0,0.0))
pt7= points.newPoint(geom.Pos3d(0.0,2.0,0.0))
pt8= points.newPoint(geom.Pos3d(1.0,2.0,0.0))
pt9= points.newPoint(geom.Pos3d(2.0,2.0,0.0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt5.tag,pt4.tag)
s2= surfaces.newQuadSurfacePts(pt2.tag,pt5.tag,pt6.tag,pt3.tag)
s3= surfaces.newQuadSurfacePts(pt4.tag,pt7.tag,pt8.tag,pt5.tag)
s4= surfaces.newQuadSurfacePts(pt5.tag,pt6.tag,pt9.tag,pt8.tag)

xcTotalSet= modelSpace.getTotalSet()

#xcTotalSet.surfaces.reverse()

orientationVector= geom.Vector3d(0.0,0.0,1.0)
xcTotalSet.surfaces.setOrientation(orientationVector)

err= 0.0
for s in xcTotalSet.surfaces:
    d= s.getKVector.dot(orientationVector)
    err+= (d-1.0)**2

err= math.sqrt(err)

# print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# from postprocess import output_handler

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
