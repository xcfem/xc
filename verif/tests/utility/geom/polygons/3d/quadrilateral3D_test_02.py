# -*- coding: utf-8 -*-
''' Quadrilateral2D shape functions test.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

#                           p1
#  p2  +--------------------+
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      +--------------------+
#     p3                    p4
#
# quadCorners= [p1, p2, p3, p4]

quadCorners= [geom.Pos3d(0,-2754.543701171875e-3, 36112.4765625e-3),
              geom.Pos3d(0,-12437.34765625e-3, 31208.30078125e-3),
              geom.Pos3d(0,-8689.416015625e-3, 27909.431640625e-3),
              geom.Pos3d(0,-1746.2445068359375e-3, 31571.00390625e-3)]
# Get the vertices.
[p1, p2, p3, p4]= quadCorners

# Create the quadrilateral surface.
quad= geom.Quadrilateral3d(p1, p2, p3, p4)

# Compute the shape functions at the vertices.
shapeFunctionsValues= list()
for p in quadCorners:
    shapeFunctionsValues.append(quad.Ni(p))
refShapeFunctionsValues= [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]


# Compute the shape functions at the midpoint.
midpointNiValues= quad.Ni(quad.midpoint) # values of the shape functions.
refMidpointNiValues= 4*[1/4.0]

# Check the results.
err= 0
for values,refValues in zip(shapeFunctionsValues, refShapeFunctionsValues):
    for ni, refNi in zip(values, refValues):
        err+= (ni-refNi)**2

for ni, refNi in zip(midpointNiValues, refMidpointNiValues):
    err+= (ni-refNi)**2
    
err= math.sqrt(err)

# print(err)

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
# def get_points(vertices):
#     retval= list()
#     for p in vertices:
#         retval.append((p.x, p.y, 0.0))
#     p= vertices[0] # close contour.
#     retval.append((p.x, p.y, 0.0))
#     return retval

# # Write contour.
# points= get_points(quadCorners)
# msp.add_polyline3d(points)
# doc.saveas(Path(fname).stem+".dxf")

