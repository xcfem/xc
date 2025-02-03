# -*- coding: utf-8 -*-
''' Trivial check of 3D blocks. Check returned hexahedron.'''

from __future__ import division
from __future__ import print_function

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
## Problem type
modelSpace= predefined_spaces.SolidMechanics3D(nodes)

#
#     5 +---------+ 8     0: Bottom face; vertices 1,4,3,2. (0,3,2,1)
#      /|        /|       1: Left-side face; vertices 1,2,6,5. (0,1,5,4)
#     / |       / |       2: Front face; vertices 2,3,7,6. (1,2,6,5)
#  6 +---------+7 |       3: Right-side face; vertices 3,4,8,7. (2,3,7,6)
#    |  |      |  |       4: Back face; vertices 1,5,8,4. (0,4,7,3)
#    |1 +------|--+ 4     5: Top face; vertices 5,6,7,8. (4,5,6,7)
#    | /       | /
#    |/        |/
#  2 +---------+ 3
#

# Define k-points.
pt1= modelSpace.newKPoint(0,0,0)
pt2= modelSpace.newKPoint(1,0,0)
pt3= modelSpace.newKPoint(1,1,0)
pt4= modelSpace.newKPoint(0,1,0)

pt5= modelSpace.newKPoint(0,0,1)
pt6= modelSpace.newKPoint(1,0,1)
pt7= modelSpace.newKPoint(1,1,1)
pt8= modelSpace.newKPoint(0,1,1)

# Define block.
bodies= preprocessor.getMultiBlockTopology.getBodies
b1= bodies.newBlockPts(pt1.tag, pt2.tag, pt3.tag, pt4.tag, pt5.tag, pt6.tag, pt7.tag, pt8.tag)

# Check returned hexahedron.
h= b1.getHexahedron()

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

testPoint= geom.Pos3d(.5, .5, .5)
pseudoDist= h.pseudoDist(testPoint)
ratio2= abs(pseudoDist+0.5)/0.5
inside= h.In(testPoint, 1e-3)
outside= not h.In(testPoint+geom.Vector3d(1,1,1), 1e-3)

'''
print('vertex position error: ', posError)
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
