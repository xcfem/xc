# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from rough_calculations import cable_stayed_bridge_simple_model as bm

bridge= bm.CableStayedBridgeRoughModel(6.0)

q= 130 #kN/m

#Exercise 1
NCable= bridge.getNCable(q/2.0,math.radians(35))

#Exercise 2
NCableA= bridge.getNCable(q/2.0,math.radians(80))
NCableB= bridge.getNCable(q/2.0,math.radians(25))

#Exercise 2
ratioAreas= NCableB/NCableA
ratioDiameters= math.sqrt(ratioAreas)

ratio1= (NCable-679.944250292)/679.944250292
ratio2= (NCableA-396.016378635)/396.016378635
ratio3= (NCableB-922.818617429)/922.818617429

'''
print 'NCable= ', NCable
print 'NCableA= ', NCableA
print 'NCableB= ', NCableB
print 'ratioAreas= ', ratioAreas
print 'ratioDiameters= ', ratioDiameters
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
