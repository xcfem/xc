# -*- coding: utf-8 -*-
'''Test getContiguousLaneSegment method.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from actions.roadway_traffic import load_model_base as lmb

# Define notional lane
notionalLaneContourPoints= [geom.Pos3d(0,0,0), geom.Pos3d(0,1,0), geom.Pos3d(2,1,0), geom.Pos3d(2,0,0)]
notionalLane= lmb.NotionalLane(name= 'test', contour= geom.Polygon3d(notionalLaneContourPoints))
notionalLaneVDir= notionalLane.getVDir()

# Compute following and previous segments.
previousNotionalLane= notionalLane.getContiguousLaneSegment(length= -1)
followingNotionalLane= notionalLane.getContiguousLaneSegment(length= 1)

# Compute some derived quantities.
previousNotionalLaneVertices= previousNotionalLane.contour.getVertexList()
previousNotionalLaneVDir= previousNotionalLane.getVDir()
followingNotionalLaneVertices= followingNotionalLane.contour.getVertexList()
followingNotionalLaneVDir= followingNotionalLane.getVDir()

# Check results.
err1= (notionalLaneVDir-previousNotionalLaneVDir).getModulus()
err2= (notionalLaneVDir-followingNotionalLaneVDir).getModulus()
previousNotionalLaneVerticesRef= [geom.Pos3d(-1,0,0), geom.Pos3d(-1,1,0), geom.Pos3d(0,1,0), geom.Pos3d(0,0,0)]
err3= 0
for v, vr in zip(previousNotionalLaneVertices, previousNotionalLaneVerticesRef):
    err3+= v.dist2(vr) # |v-vr|^2
err3= math.sqrt(err3)

followingNotionalLaneVerticesRef= [geom.Pos3d(2,0,0), geom.Pos3d(2,1,0), geom.Pos3d(3,1,0), geom.Pos3d(3,0,0)]
err4= 0
for v, vr in zip(followingNotionalLaneVertices, followingNotionalLaneVerticesRef):
    err4+= v.dist2(vr) # |v-vr|^2
err4= math.sqrt(err4)

'''
print('previous notional lane direction vector: ', previousNotionalLaneVDir)
print(err1)
print('notional lane direction vector: ', notionalLaneVDir)
print('following notional lane direction vector: ', followingNotionalLaneVDir)
print(err2)
print('previous notional lane vertices: ', previousNotionalLaneVertices)
print(err3)
print('following notional lane vertices: ', followingNotionalLaneVertices)
print(err4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((err1<1e-11) and (err2<1e-11) and (err3<1e-11) and (err4<1e-11)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Export to DXF
# import ezdxf
# from pathlib import Path
# doc= ezdxf.new("R2000")
# msp= doc.modelspace()
# def getPoints(vertices):
#     retval= list()
#     for p in vertices:
#         retval.append((p.x, p.y, p.z))
#     p= vertices[0] # close contour.
#     retval.append((p.x, p.y, p.z))
#     return retval

# # Write notional lane contour.
# points= getPoints(notionalLaneContourPoints)
# msp.add_polyline3d(points)
# # Write previous and following notional lanes contours.
# points= getPoints(previousNotionalLaneVertices)
# msp.add_polyline3d(points)
# points= getPoints(followingNotionalLaneVertices)
# msp.add_polyline3d(points)
# doc.saveas(Path(fname).stem+".dxf")
