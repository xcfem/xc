# -*- coding: utf-8 -*-
''' Pos3D trivial test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

xPoints= list()
for i in range(0,25):
    x= float(i)
    xPoints.append(geom.Pos3d(x, 0.0, 0.0))
    
mirrorXPoints= list()
for p in reversed(xPoints[1:]):
    mirrorXPoints.append(geom.Pos3d(-p.x, p.y, p.z))
xPoints= mirrorXPoints+xPoints
xPointCloud= geom.PointCloud3d(xPoints)
xPrincipalAxes= xPointCloud.getPrincipalAxesOfInertia()

print(xPrincipalAxes.I1, xPrincipalAxes.getAxis1VDir())
print(xPrincipalAxes.I2, xPrincipalAxes.getAxis2VDir())
print(xPrincipalAxes.I3, xPrincipalAxes.getAxis3VDir())
