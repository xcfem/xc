# -*- coding: utf-8 -*-
'''Bridge loads. Notional lane computation.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"

import geom
from actions.roadway_traffic import EC1_load_models

p1= geom.Pos3d(0.0, 0.0, 0.0)
p2= geom.Pos3d(2.0, 0.0, 0.0)
p3= geom.Pos3d(0.0, 13.5, 0.0)
p4= geom.Pos3d(2.0, 13.5, 0.0)

border1= geom.Segment3d(p1, p2)
border2= geom.Segment3d(p3, p4)

carriagewayWidth= EC1_load_models.getCarriagewayWidth(firstBorder= border1, lastBorder= border2)
notionalLanesWidths= EC1_load_models.getNotionalLanesWidths(carriagewayWidth)
notionalLanes= EC1_load_models.NotionalLanes(firstBorder= border1, lastBorder= border2)

# Compute following and previous notional lanes.
previousNotionalLanes= notionalLanes.getContiguousNotionalLanes(length= -1)
followingNotionalLanes= notionalLanes.getContiguousNotionalLanes(length= 1)

# Compute some derived results.
previousNotionalLanesAreas= previousNotionalLanes.getAreas()
followingNotionalLanesAreas= followingNotionalLanes.getAreas()

# Check results
err= 0.0
refValues= [3.0, 3.0, 3.0, 3.0, 1.5]
for ap,af,ar in zip(previousNotionalLanesAreas, followingNotionalLanesAreas, refValues):
    err+= (ap-ar)**2
    err+= (af-ar)**2

'''
print(previousNotionalLanesAreas)
print(followingNotionalLanesAreas)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
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
# def getPointsNL(notionalLane):
#     return getPoints(notionalLane.contour.getVertexList())
# def getContoursNLs(notionalLanes):
#     retval= list()
#     for nl in notionalLanes.lanes:
#         retval.append(getPointsNL(nl))
#     return retval

# # Write notional lanes contour.
# contours= getContoursNLs(notionalLanes)
# for points in contours:
#     msp.add_polyline3d(points)
# # Write previous and following notional lanes contours.
# contours= getContoursNLs(previousNotionalLanes)
# for points in contours:
#     msp.add_polyline3d(points)
# contours= getContoursNLs(followingNotionalLanes)
# for points in contours:
#     msp.add_polyline3d(points)

# doc.saveas(Path(fname).stem+".dxf")
