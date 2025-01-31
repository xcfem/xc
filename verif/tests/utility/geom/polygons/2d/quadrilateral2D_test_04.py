# -*- coding: utf-8 -*-
''' Quadrilateral2D class. Test getBilinearGrid method.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

#    
#  p4  +--------------------+ p3
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
#     p1                     p2
#

# Create the quadrilateral surface
sideLength= 1
p1= geom.Pos2d(0,0)
p2= geom.Pos2d(sideLength,0)
p3= geom.Pos2d(sideLength,sideLength)
p4= geom.Pos2d(0,sideLength)
quad= geom.Quadrilateral2d(p1, p2, p3, p4)

# Compute points on grid.
pointGrid= quad.getBilinearGrid(2,2)
numQuads= pointGrid.getNumQuads()
centroids= pointGrid.getQuadCentroids()
areas= pointGrid.getQuadAreas()

# Check results.
lower= 0.25*sideLength
upper= 0.75*sideLength
refCentroids= [geom.Pos2d(lower, lower), geom.Pos2d(upper,lower), geom.Pos2d(lower, upper), geom.Pos2d(upper, upper)]
refQuadArea= sideLength**2/4.0
refAreas= 4*[refQuadArea]

errC= 0.0
for c, refC in zip(centroids, refCentroids):
    errC+= (c-refC).getModulus2()
errC= math.sqrt(errC)
errA= 0.0
for a, refA in zip(areas, refAreas):
    errA+= (a-refA)**2
errA= math.sqrt(errA)

'''
print(numQuads)
print(centroids)
print(errC)
print(areas)
print(errA)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(errC)<1e-10 and abs(errA)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
    
# Graphic output
import matplotlib.pyplot as plt
def getXYVertices(plg):
    x= list()
    y= list()
    vertices= plg.getVertices()
    for p in vertices:
        x.append(p.x)
        y.append(p.y)
    x.append(vertices[0].x)
    y.append(vertices[0].y)
    return x,y
    
fig = plt.figure()
ax = fig.add_subplot(111)
x,y= getXYVertices(plg)
ax.plot(x,y,'tab:gray')
pA= testSegment.getFromPoint()
pB= testSegment.getToPoint()
xi= [pA.x, pB.x]
yi= [pA.y, pB.y]
plt.plot(xi, yi, 'tab:blue')
pa= clippedSegment.getFromPoint()
pb= clippedSegment.getToPoint()
xi= [pa.x, pb.x]
yi= [pa.y, pb.y]
plt.plot(xi, yi, 'tab:red')
plt.show()
