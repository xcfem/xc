# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

svd1=geom.SlidingVectorsSystem2d()
ptoAplic=geom.Pos2d(1,1)
vectorDir=geom.Vector2d(1,0)
vec=geom.SlidingVector2d(ptoAplic,vectorDir)
svd1+=vec

svd2=geom.SlidingVectorsSystem2d()
ptoAplic=geom.Pos2d(1,1)
vectorDir=geom.Vector2d(0.5,0)
vec=geom.SlidingVector2d(ptoAplic,vectorDir)
svd2+=vec

R= 5.0*svd1-10.0*svd2
result= R.getResultant()
mom= R.getMoment()
ratio= result.getModulus()+ math.sqrt(mom**2)

'''
print('R= ', R)
print('result= ', result)
print('mom= ', mom)
print('ratio= ', ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio<1e-13):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

