# -*- coding: utf-8 -*-
'''Railway bridge loads. Rail uniform load.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from actions.railway_traffic import uniform_rail_load

# Problem geometry.
side= 10.0
p0= geom.Pos3d(0.0, 0.0, 0.0)
p1= geom.Pos3d(side, 0.0, 0.0)
p2= geom.Pos3d(side, side, 0.0)
p3= geom.Pos3d(0.0, side, 0.0)

pA= geom.Pos3d(side/2.0, side/2.0, 0.0)

deckMidplane= geom.Plane3d(p0, p1, p3)

sleeperThickness= 22e-2
sleeperLength= 2.6
standardGauge= 1.435
lateralWidth= sleeperLength-standardGauge
sleeperSpreadToDepthRatio= lateralWidth/sleeperThickness

ballastThickness= 0.6
ballastSpreadToDepthRatio= 1/4.0 # See Eurocode 1 part 2 figure 6.5.

spreadingLayers= [(sleeperThickness, sleeperSpreadToDepthRatio), (ballastThickness, ballastSpreadToDepthRatio)]

## Rail axis.
zRail= ballastThickness
railUpVector= geom.Vector3d(0,0,zRail)
railAxis= geom.Polyline3d([p0+railUpVector, pA+railUpVector, p2+railUpVector])
load= 40e3
dynamicFactor= 1.2
classificationFactor= 1.0
uniformRailLoad= uniform_rail_load.UniformRailLoad(railAxis, load, dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)

axisProjection= uniformRailLoad.getRailAxisProjection(midplane= deckMidplane)

loadedContour= uniformRailLoad.getLoadedContour(midplane= deckMidplane, spreadingLayers= spreadingLayers)

loadedContourArea= loadedContour.getArea()
loadedContourAreaRefValue= 37.193816690412405 # loaded contour with flat extremities.
ratio1= abs(loadedContourArea-loadedContourAreaRefValue)/loadedContourAreaRefValue

'''
print(lateralWidth)
print(sleeperSpreadToDepthRatio)
print(loadedContourArea)
print(ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff
# import matplotlib.pyplot as plt

# # Deck polygon
# deckPolygon= list()
# for p in [p0, p1, p2, p3]:
#     deckPolygon.append([p.x, p.y])
# deckPolygon.append(deckPolygon[0]) #repeat the first point to create a 'closed loop'

# xs, ys = zip(*deckPolygon) #create lists of x and y values

# plt.figure()
# plt.fill(xs,ys)

# loadedPolygon= list()
# for p in loadedContour.getVertexList():
#     loadedPolygon.append([p.x, p.y])
# loadedPolygon.append(loadedPolygon[0]) #repeat the first point to create a 'closed loop'
# xs, ys = zip(*loadedPolygon) #create lists of x and y values
# plt.fill(xs, ys)
# plt.show()
