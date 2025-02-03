# -*- coding: utf-8 -*-
''' Trivial check of quadrilateral surfaces. Check default constructor'''

from __future__ import division
from __future__ import print_function

import math
import geom

#
#     4 +---------+ 7     
#      /|        /|       
#     / |       / |       
#  5 +---------+6 |       
#    |  |      |  |       
#    |0 +------|--+ 3     
#    | /       | /
#    |/        |/
#  1 +---------+ 2
#

# Define hexahedron with default constructor.
h= geom.Hexahedron3d()
testPoint= geom.Pos3d(.5, .5, .5)

# Check results.
## Check vertices positions. 
vertices= h.getVertices()

# Check that the vertices are returned in the right sequence.
refVertices= [geom.Pos3d(0,0,0), geom.Pos3d(1,0,0), geom.Pos3d(1,1,0), geom.Pos3d(0,1,0), geom.Pos3d(0,0,1), geom.Pos3d(1,0,1), geom.Pos3d(1,1,1), geom.Pos3d(0,1,1)]
posError= 0.0
for i, v in enumerate(vertices):
    refV= refVertices[i]
    posError+= refV.dist2(v)
posError= math.sqrt(posError)

## Check area.
a= h.area
ratio0= (a-6.0)/6.0
## Check volume.
v= h.volume
ratio1= abs(v-1)

pseudoDist= h.pseudoDist(testPoint)
ratio2= abs(pseudoDist+0.5)/0.5
inside= h.In(testPoint, 1e-3)
outside= not h.In(testPoint+geom.Vector3d(1,1,1), 1e-3)

'''
print('XC to CGAL error: ', posError)
print('area: ', a)
print('volume: ', v)
print('pseudo distance: ', pseudoDist)
print('inside: ', inside)
print('outside: ', outside)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((posError<1e-12) and (ratio0<1e-8) and (ratio1<1e-8) and (ratio2<1e-8) and inside and outside):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # Graphic output
# from postprocess.xcVtk import vtk_geom
# from postprocess.xcVtk import vtk_graphic_base
# from vtk.vtkCommonColor import vtkNamedColors

# colors= vtkNamedColors()

# vtkCubeActor= vtk_geom.get_vtk_cube_actor(h, color= colors.GetColor3d('Tomato'))
# vtk_graphic_base.setup_render_window(windowName= 'PolyLine', actors= [vtkCubeActor], backgroundColor= colors.GetColor3d('DarkOliveGreen'), xSize= 800, ySize= 600)
