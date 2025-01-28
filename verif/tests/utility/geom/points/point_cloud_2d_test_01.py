# -*- coding: utf-8 -*-
''' Pos3D trivial test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

xPoints= list()
for i in range(0,25):
    x= float(i)
    xPoints.append(geom.Pos2d(x, 0.0))
    
mirrorXPoints= list()
for p in reversed(xPoints[1:]):
    mirrorXPoints.append(geom.Pos2d(-p.x, p.y))
xPoints= mirrorXPoints+xPoints

xPointCloud= geom.PointCloud2d(xPoints)

xPrincipalAxes= xPointCloud.getPrincipalAxes()

yPoints= list()
for p in xPoints:
    yPoints.append(geom.Pos2d(0.0, p.x))
yPointCloud= geom.PointCloud2d(yPoints)
yPrincipalAxes= yPointCloud.getPrincipalAxes()

# Check results.
refValue= 204+1/6.0
print(refValue)
err= (xPrincipalAxes.I1-refValue)**2+xPrincipalAxes.I2**2
err+= (yPrincipalAxes.I1-refValue)**2+yPrincipalAxes.I2**2
xV1=  xPrincipalAxes.getAxis1VDir()
xV2=  xPrincipalAxes.getAxis2VDir()
err+= (xV1[0]-1)**2+(xV1[1])**2
err+= (xV2[0])**2+(xV2[1]-1)**2
yV1=  yPrincipalAxes.getAxis1VDir()
yV2=  yPrincipalAxes.getAxis2VDir()
err+= (yV1[0])**2+(yV1[1]-1)**2
err+= (yV2[0]+1)**2+(yV2[1])**2
err= math.sqrt(err)

'''
print(xPrincipalAxes.I1, xPrincipalAxes.getAxis1VDir())
print(xPrincipalAxes.I2, xPrincipalAxes.getAxis2VDir())

print('\n', yPrincipalAxes.I1, yPrincipalAxes.getAxis1VDir())
print(yPrincipalAxes.I2, yPrincipalAxes.getAxis2VDir())
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+": ERROR.")
