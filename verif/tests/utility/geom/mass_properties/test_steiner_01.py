# -*- coding: utf-8 -*-
''' Test the calculation of mass properties for a set of geometric objects.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
from geom_utils import mass_properties

polA= geom.Polygon2d([geom.Pos2d(-2,-2), geom.Pos2d(2,-2), geom.Pos2d(2,2), geom.Pos2d(-2,2)])
polB= geom.Polygon2d([geom.Pos2d(-1,-1), geom.Pos2d(1,-1), geom.Pos2d(1,1), geom.Pos2d(-1,1)])

inertiaAB= mass_properties.get_inertia_2d([(1.0, polA), (-1.0, polB)])
refInertiaAB= [4**4/12.0-(2**4)/12.0]*2+[0.0]
for iA, iB in zip(inertiaAB, refInertiaAB):
    err= (iA-iB)**2

sq1= geom.Polygon2d([geom.Pos2d(-1,-1), geom.Pos2d(0,-1), geom.Pos2d(0,0), geom.Pos2d(-1,0)])
sq2= geom.Polygon2d([geom.Pos2d(0,-1), geom.Pos2d(1,-1), geom.Pos2d(1,0), geom.Pos2d(0,0)])
sq3= geom.Polygon2d([geom.Pos2d(0,0), geom.Pos2d(1,0), geom.Pos2d(1,1), geom.Pos2d(0,1)])
sq4= geom.Polygon2d([geom.Pos2d(-1,0), geom.Pos2d(0,0), geom.Pos2d(0,1), geom.Pos2d(-1,1)])

inertiaL_addition= mass_properties.get_inertia_2d([(1.0, sq1), (1.0, sq2), (1.0, sq4)])
inertiaL_substraction= mass_properties.get_inertia_2d([(1.0, polB), (-1.0, sq3)])
for iLa, iLb in zip(inertiaL_addition, inertiaL_substraction):
    err+= (iLa-iLb)**2

err= math.sqrt(err)

# print(inertiaAB)
# print(refInertiaAB)
# print(inertiaL_addition)
# print(inertiaL_substraction)
# print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-12:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
