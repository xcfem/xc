# -*- coding: utf-8 -*-
'''Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom

pt6= geom.Pos3d(0.137841075845,-0.0249999998934,0.300997851928)
pt8= geom.Pos3d(0.152509261538,-0.0249999998927,0.29581670066)
pt10= geom.Pos3d(0.157690412806,-0.0249999998925,0.310484886354)

poly3d= geom.Polyline3d()
poly3d.appendVertex(pt6)
poly3d.appendVertex(pt8)
poly3d.appendVertex(pt10)

plg1= geom.Polygon3d([pt6,pt8,pt10])
plg2= geom.Polygon3d(pt6, pt8, pt10)
center1= plg1.getCenterOfMass()
center2= plg2.getCenterOfMass()
v= center2-center1
ratio= v.getModulus()

'''
print(center1)
print(center2)
print(ratio)
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio)<1e-15) :
  print("test "+fname+": ok.")
else:
  print("test "+fname+": ERROR.")
