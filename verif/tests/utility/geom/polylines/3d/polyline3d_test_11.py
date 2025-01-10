# -*- coding: utf-8 -*-
''' Verification of the getNormalVectorAtVertices method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

# Define polyline.
pline= geom.Polyline3d([geom.Pos3d(1,0,0), geom.Pos3d(1,1,0), geom.Pos3d(0,1,0)])

# Compute vertex normals.
normals= pline.getNormalVectorAtVertices()

# Check results.
half_sqrt_2= math.sqrt(2)/2.0
ref_normals= [geom.Vector3d(1,0,0), geom.Vector3d(half_sqrt_2, half_sqrt_2, 0), geom.Vector3d(0,1,0)]

err= 0.0
for n, n_ref in zip(normals, ref_normals):
    err+= (n-n_ref).getModulus2()
err= math.sqrt(err)

'''
print(normals)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-8):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
