# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import math

A= geom.Pos3d(11.3722,5.09673,4.80346)
B= geom.Pos3d(11.3722,3.61173,4.80346)
C= geom.Pos3d(11.3722,3.81673,4.80346)
D= geom.Pos3d(11.3722,1.94173,4.80346)

a= geom.Segment3d(A,B)
b= geom.Segment3d(C,D)

A.logFileName= "/tmp/erase.log" # Ignore warning messages
p= a.getIntersection(b)[0]
A.logFileName= "clog" # Display warnings if any.
pRef= geom.Segment3d(B,C).getCenterOfMass()
ratio= p.dist2(pRef)

'''
print('p: '+str(p)) 
print('pRef: '+str(pRef)) 
print('ratio: '+str(ratio)) 
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio)<1e-10):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
