# -*- coding: utf-8 -*-
''' Test getIVectorAtLength, getJVectorAtLength and getKVectorAtLength 
    methods.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import json
import os

def computeMaxAngle(pline3d):
    ''' Return the maximum angle between contiguous vertices for the given
        3D polyline.
    '''
    vertices= pline3d.getVertexList()
    p0= vertices[0]
    iVectors= list()
    for p1 in vertices[1:]:
        s= geom.Segment3d(p0, p1)
        iVectors.append(s.getIVector)
        p0= p1

    iV0= iVectors[0]
    maxAngle= 0.0
    for iV in iVectors[1:]:
        maxAngle= max(maxAngle, iV.getAngle(iV0))
        iV0= iV
    return maxAngle

pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
verticesFilePath= pth+'/../../../../aux/polyline3d_test_points.json'

coordinateData= open(verticesFilePath, 'r')
coordinateValues= json.load(coordinateData)
coordinateData.close()
vertices= list()
for coo in coordinateValues:
    vertices.append(geom.Pos3d(coo[0], coo[1], coo[2]))



# Define polyline.
pline3d= geom.Polyline3d(vertices)
lengthBefore= pline3d.getLength()
nvBefore= pline3d.getNumVertices()
## Remove repeated vertices.
tol= lengthBefore/1e4
pline3d.removeRepeatedVertexes(tol)
length= pline3d.getLength()
nv= pline3d.getNumVertices()

lengthDiff= abs(lengthBefore-length)
nvDiff= nvBefore-nv

okRepeatedVertexes= (nvDiff==1) and (abs(lengthDiff)<1e-12)

# Remove little imperfections.
maxAngleBefore= computeMaxAngle(pline3d) # Compute max. angle between contiguous segments.
pline3d.simplify(0.15) # Simplify the polyline
maxAngle= computeMaxAngle(pline3d)

okImperfections= (maxAngle<maxAngleBefore/3)

# Check results at different points.
length= pline3d.getLength() # update length.
relativeLengths= list()
for f in [0.0, 0.25, 0.5, 0.75, 1.0]:
    relativeLengths.append(f*length)

vJs= list()
for s in relativeLengths:
    vJ= pline3d.getJVectorAtLength(s)
    vJs.append(vJ)

vJsRef= [geom.Vector3d(-0.988993, -0.147962, 0), geom.Vector3d(-0.98473, -0.174086, 0), geom.Vector3d(-0.98473, -0.174086, 0), geom.Vector3d(-0.977802, -0.209529, 0)]

err= 0.0
for vJ, vJRef in zip(vJs, vJsRef):
    err+= (vJ-vJRef).getModulus()

'''
print(lengthBefore, length, lengthDiff)
print(nvBefore, nv, nvDiff)
print(okRepeatedVertexes)
print(math.degrees(maxAngleBefore), math.degrees(maxAngle))
print(err)
'''
 
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if okRepeatedVertexes and okImperfections and (err<1e-5):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # Plot wireframe.
# import matplotlib.pyplot as plt
# x= list()
# y= list()
# z= list()
# vertices= pline3d.getVertexList()
# for p in vertices:
#     x.append(p.x)
#     y.append(p.y)
#     z.append(p.z)
# fig = plt.figure()
# ax = plt.axes(projection='3d')
# #ax.axis('equal')
# ax.plot3D(x, y, z)
# plt.show() # Show graph
