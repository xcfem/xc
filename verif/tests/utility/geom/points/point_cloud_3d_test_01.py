# -*- coding: utf-8 -*-
''' Point cloud orientation trivial test (3D).'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

# Points along the x axis.
xPoints= list()
for i in range(0,25):
    x= float(i)
    xPoints.append(geom.Pos3d(x, 0.0, 0.0))
    
mirrorXPoints= list()
for p in reversed(xPoints[1:]):
    mirrorXPoints.append(geom.Pos3d(-p.x, p.y, p.z))
xPoints= mirrorXPoints+xPoints
xPointCloud= geom.PointCloud3d(xPoints)
xPrincipalAxes= xPointCloud.getPrincipalAxes()

# Points along the y axis.
yPoints= list()
for p in xPoints:
    yPoints.append(geom.Pos3d(0.0, p.x, 0.0))
yPointCloud= geom.PointCloud3d(yPoints)
yPrincipalAxes= yPointCloud.getPrincipalAxes()

# Points along the z axis.
zPoints= list()
for p in xPoints:
    zPoints.append(geom.Pos3d(0.0, 0.0, p.x))
zPointCloud= geom.PointCloud3d(zPoints)
zPrincipalAxes= zPointCloud.getPrincipalAxes()


# Check results.
refValue= 204+1/6.0
err= (xPrincipalAxes.I1-refValue)**2+xPrincipalAxes.I2**2+xPrincipalAxes.I3**2
err+= (yPrincipalAxes.I1-refValue)**2+yPrincipalAxes.I2**2+yPrincipalAxes.I3**2
err+= (zPrincipalAxes.I1-refValue)**2+zPrincipalAxes.I2**2+zPrincipalAxes.I3**2

err+= (xPrincipalAxes.getAxis1VDir()-geom.Vector3d(1,0,0)).getModulus2()
err+= (xPrincipalAxes.getAxis2VDir()-geom.Vector3d(0,0,1)).getModulus2()
err+= (xPrincipalAxes.getAxis3VDir()-geom.Vector3d(0,-1,0)).getModulus2()

err+= (yPrincipalAxes.getAxis1VDir()-geom.Vector3d(0,1,0)).getModulus2()
err+= (yPrincipalAxes.getAxis2VDir()-geom.Vector3d(0,0,1)).getModulus2()
err+= (yPrincipalAxes.getAxis3VDir()-geom.Vector3d(1,0,0)).getModulus2()

err+= (zPrincipalAxes.getAxis1VDir()-geom.Vector3d(0,0,1)).getModulus2()
err+= (zPrincipalAxes.getAxis2VDir()-geom.Vector3d(0,1,0)).getModulus2()
err+= (zPrincipalAxes.getAxis3VDir()-geom.Vector3d(-1,0,0)).getModulus2()

err= math.sqrt(err)

'''
print('\nX axis.')
print(xPrincipalAxes.I1, xPrincipalAxes.getAxis1VDir())
print(xPrincipalAxes.I2, xPrincipalAxes.getAxis2VDir())
print(xPrincipalAxes.I3, xPrincipalAxes.getAxis3VDir())
print('\nY axis.')
print(yPrincipalAxes.I1, yPrincipalAxes.getAxis1VDir())
print(yPrincipalAxes.I2, yPrincipalAxes.getAxis2VDir())
print(yPrincipalAxes.I3, yPrincipalAxes.getAxis3VDir())
print('\nZ axis.')
print(zPrincipalAxes.I1, zPrincipalAxes.getAxis1VDir())
print(zPrincipalAxes.I2, zPrincipalAxes.getAxis2VDir())
print(zPrincipalAxes.I3, zPrincipalAxes.getAxis3VDir())
print('\nError')
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+": ERROR.")
