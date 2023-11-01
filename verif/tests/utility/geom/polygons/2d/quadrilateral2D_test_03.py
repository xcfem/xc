# -*- coding: utf-8 -*-
''' Interpolate reinforcement directions in an irregular slab (2D version).'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import os
import geom

#                           p1
#  p2  +--------------------+--------------------+ p0
#      |                    |                    |
#    e |                  e |                  e |
#    d |                  d |                  d |
#    g |                  g |                  g |
#    e |                  e |                  e |
#      |                    |                    |
#    1 |                  2 |                  3 |
#      |                    |                    |
#      |                    |                    |
#      +--------------------+--------------------+
#     p3                    p4                   p5
#
# slabCorners= [p0, p1, p2, p3, p4, p5]

slabCorners= [geom.Pos2d(5901.36865234375e-3, 36112.4765625e-3),
              geom.Pos2d(-2754.543701171875e-3, 36112.4765625e-3),
              geom.Pos2d(-12437.34765625e-3, 31208.30078125e-3),
              geom.Pos2d(-8689.416015625e-3, 27909.431640625e-3),
              geom.Pos2d(-1746.2445068359375e-3, 31571.00390625e-3),
              geom.Pos2d(7165.8857421875e-3, 31553.732421875e-3)]
# Get the vertices.
[p0, p1, p2, p3, p4, p5]= slabCorners

# Compute the directions of the main reinforcement for each vertex.
edge1Dir= (p2-p3).normalized()
edge2Dir= (p1-p4).normalized()
edge3Dir= (p0-p5).normalized()

# Create the quadrilateral surfaces and set the reinforcement directions.
quadA= geom.Quadrilateral2d(p1, p2, p3, p4)
quadADirections= [edge2Dir, edge1Dir, edge1Dir, edge2Dir]
quadB= geom.Quadrilateral2d(p0, p1, p4, p5)
quadBDirections= [edge3Dir, edge2Dir, edge2Dir, edge3Dir]

# Compute the centroid of the surfaces.
centroidA= quadA.midpoint
centroidB= quadB.midpoint

# Compute the directions at the centroids.
NiA= quadA.Ni(centroidA) # values of the shape functions.
directionAtCentroidA= geom.Vector2d(0,0)
for ni, dir_i in zip(NiA, quadADirections):
    directionAtCentroidA+= ni*dir_i
NiB= quadB.Ni(centroidB) # values of the shape functions.
directionAtCentroidB= geom.Vector2d(0,0)
for ni, dir_i in zip(NiB, quadBDirections):
    directionAtCentroidB+= ni*dir_i

# Check results.
refDirectionAtCentroidA= 0.5*(edge1Dir+edge2Dir)
refDirectionAtCentroidB= 0.5*(edge2Dir+edge3Dir)

errA= (directionAtCentroidA-refDirectionAtCentroidA).getModulus()
errB= (directionAtCentroidB-refDirectionAtCentroidB).getModulus()

'''
print(centroidA, centroidB)
print(NiA, NiB)
print(directionAtCentroidA, directionAtCentroidB)
print(refDirectionAtCentroidA, refDirectionAtCentroidB)
print(errA, errB)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(errA)<1e-10 and abs(errB)<1e-10:
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
#         retval.append((p.x, p.y, 0.0))
#     p= vertices[0] # close contour.
#     retval.append((p.x, p.y, 0.0))
#     return retval

# # Write notional lane contour.
# points= getPoints(slabCorners)
# msp.add_polyline3d(points)
# # Write quadA
# points= getPoints(quadA.getVertices())
# print(points)
# msp.add_polyline3d(points)
# # points= getPoints(followingNotionalLaneVertices)
# # msp.add_polyline3d(points)
# doc.saveas(Path(fname).stem+".dxf")

