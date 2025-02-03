# -*- coding: utf-8 -*-
''' Point cloud oriented bounding box trivial test (2D).'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

xPoints= [geom.Pos3d(-1, -1, 0), geom.Pos3d(1,1,0), geom.Pos3d(-1,1,0), geom.Pos3d(0,0,1)]

xPointCloud= geom.PointCloud3d(xPoints)
# Get the reference system corresponding to the principal axes so
# the data used to verify the results are still valid (those data
# come from the test: point_cloud_3d_test_02.py).
ref= xPointCloud.getPrincipalAxes().getAxis()

# Use the get_oriented_bounding_box version that receives alse the
# desired reference system.
boundingBox= geom.get_oriented_bounding_box(xPointCloud, ref)
boundingBoxVertices= boundingBox.getVertices()
## Same reference data than in the test: point_cloud_3d_test_02.py
a= 0.757464; b= 1.24254; c= 0.136197
d= 1.8638; e= 0.485071; f= 0.106339
g= 2.10634; h= 0.378732
referenceVertices= [geom.Pos3d(-a, -b, c), geom.Pos3d(b, a, c), geom.Pos3d(c, d, -e), geom.Pos3d(-d, -c, -e), geom.Pos3d(-1, -1, 1), geom.Pos3d(1, 1, 1), geom.Pos3d(-f, g, h), geom.Pos3d(-g, f, h)]

err= 0.0
for v, vRef in zip(boundingBoxVertices, referenceVertices):
    err+= v.dist2(vRef)
err= math.sqrt(err)

# print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-4):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # Graphic output
# from postprocess.xcVtk import vtk_geom
# from postprocess.xcVtk import vtk_graphic_base
# from vtk.vtkCommonColor import vtkNamedColors

# colors= vtkNamedColors()

# vtkCubeActor= vtk_geom.get_vtk_cube_actor(boundingBox, color= colors.GetColor3d('Tomato'))
# vtkPointActor= vtk_geom.get_vtk_points_actor(points= xPointCloud, color= colors.GetColor3d('Peacock'))
# vtk_graphic_base.setup_render_window(windowName= 'PolyLine', actors= [vtkCubeActor, vtkPointActor], backgroundColor= colors.GetColor3d('DarkOliveGreen'), xSize= 800, ySize= 600)
