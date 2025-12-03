# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
svd=geom.SlidingVectorsSystem2d()

dataDict= {'className': 'SlidingVectorsSystem2d', 'exts':True, 'x': -0.6804372034975499, 'y': 36.434333573138545, 'org': {'className': 'Pos2d', 'exts': True, 'x': 0.1761, 'y': 0.265}, 'moment': 0.8921095909023141}

svd.setFromDict(dataDict)

err= 0.0
R= svd.getResultant()
refR= geom.Vector2d(dataDict['x'],dataDict['y'])
err+= (R-refR).getModulus2()
M= svd.getMoment()
refM= dataDict['moment']
err+= (M-refM)**2

err= math.sqrt(err)

'''
print(R)
print(M)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(err)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
