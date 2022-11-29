# -*- coding: utf-8 -*-
''' Trivial test to check that the load "shadow" contour on the retaining wall
    is properly computed. The results have been tested using the DXF wireframe
    created from the ressults (see commented lines at the end of this file).'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
from geotechnics import horizontal_surcharge as hs

# Position of the concentrated load.
posA= geom.Pos3d(0.0,0.0,10)
posB= geom.Pos3d(0.0,1.0,10)
# Horizontal load.
H= geom.Vector3d(0,-1e3,0)

horizontalSurcharge= hs.HorizontalLinearLoadOnBackfill3D(endPoints= [posA, posB], H= H)

# Plane of the retaining structure.
loadedPlane= geom.Plane3d(geom.Pos3d(0,-1,0), geom.Pos3d(10,-1,0), geom.Pos3d(0,-1,10))

# "Shadow" of the loaded area on the loaded plane.
shadowContour= horizontalSurcharge.getLoadedArea(loadedPlane, phi= math.radians(30))
shadowCentroid= shadowContour.getCenterOfMass()
shadowArea= shadowContour.getArea()


# Check results
ratio1= (shadowCentroid-geom.Pos3d(0.0,-1,7.8189)).getModulus()
ratio2= abs(shadowArea-8.660254037844378)/8.660254037844378

'''
print(shadowContour)
print(shadowCentroid)
print(ratio1)
print(shadowArea)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-5 and abs(ratio2)<1e-5):
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

# # Write loaded contour.
# points= getPoints(horizontalSurcharge.endPoints)
# msp.add_polyline3d(points)
# # Write shados contour.
# points= getPoints(shadowContour.getVertexList())
# msp.add_polyline3d(points)
# doc.saveas(Path(fname).stem+".dxf")



