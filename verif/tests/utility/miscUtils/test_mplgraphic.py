# -*- coding: utf-8 -*-
''' Test linear interpolation class.'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from postprocess.reports import graph_material
import geom
import math

interactionDiagram= graph_material.InteractionDiagramGraphic(title= 'Test')
plg= geom.Polygon2d([geom.Pos2d(0.5,0), geom.Pos2d(1,0.5), geom.Pos2d(0.5,1), geom.Pos2d(0,0.5)])

# compute matplotlib path
pth= interactionDiagram.getPathPolygon(plg= plg)

# check results.
scaleFactor= interactionDiagram.fScale
error= 0.0
for v1, v2 in zip(pth.vertices, plg.getVertexList()):
    error+= (v1[0]-v2[1]*scaleFactor)**2
    error+= (v1[1]-v2[0]*scaleFactor)**2
error= math.sqrt(error)

'''
print(pth.vertices)
print(error)
interactionDiagram.setupGraphic(diag= plg)
interactionDiagram.show()
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
