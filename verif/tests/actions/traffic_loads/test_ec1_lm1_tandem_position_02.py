# -*- coding: utf-8 -*-
'''Compute positions of the LM1 tandem load OUTSIDE the notional lane 
   contour.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
from actions.roadway_traffic import load_model_base as lmb
from actions.roadway_traffic import EC1_load_models as EC1lm

# Define notional lane
notionalLaneContourPoints= [geom.Pos3d(0,2,0), geom.Pos3d(0,5,0), geom.Pos3d(10,5,0), geom.Pos3d(10,2,0)]
contour= geom.Polygon3d(notionalLaneContourPoints)
notionalLane= lmb.NotionalLane(name= 'test', contour= contour)

# Compute wheel load positions.
## Compute notional lane axis.
axis= notionalLane.getAxis()
## Get the wheel loads of a 300 kN/axle tandem.
loadModel= EC1lm.tandem300LM1 # Tandem 300 kN/axle
lmbdArcLength= -0.5 # parameter (0.0->start of the axis, 1.0->end of the axis),
                    # so the point is OUTSIDE the lane contour.
                    # 
loadFactor= 1.0 # factor to apply to the loads.
contour.logFileName= "/tmp/erase.log" # Don't print error messages.
wheelLoads= axis.getWheelLoads(loadModel= loadModel, lmbdArcLength= lmbdArcLength, loadFactor= loadFactor)
contour.logileName= "clog" # Display errors if any.

# Check results.
## Reference values.
refPositions= [geom.Pos3d(-4.4,2.5,0), geom.Pos3d(-5.6,2.5,0), geom.Pos3d(-4.4,4.5,0), geom.Pos3d(-5.6,4.5,0)]
## Compute error.
err= 0.0
for wl,refp in zip(wheelLoads, refPositions):
    err+= (wl.position-refp).getModulus()**2
    
# print('axis: ', axis)
# print('err= ', err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
    
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
# # Write polyline joining wheel load centers.
# pointList= list()
# for wl in wheelLoads:
#     pointList.append(wl.position)
# points= getPoints(pointList)
# msp.add_polyline3d(points)
# doc.saveas(Path(fname).stem+".dxf")
