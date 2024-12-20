# -*- coding: utf-8 -*-
''' Check force distribution algorithm.

    This example illustrates how, in certain cases, the algorithm gives a
    result that is statically correct but mechanically unsatisfactory due to 
    the load concentrations it produces (see reaction in fourth node -n4- 
    below).
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

# Load
f= 2e3 # Load magnitude.
# Sliding vector system.
O= geom.Pos3d(0,0,0)
V= geom.Vector3d(f,0,-f)
svs1= geom.SlidingVectorsSystem3d(O,V)

leverArm= 1
points= [geom.Pos3d(-leverArm, 0, -leverArm), geom.Pos3d(leverArm, 0, -leverArm)]

# Distribute load.
vectors1= svs1.distribute(points)

# Check results.
R1= vectors1[0].getVector3d()
R2= vectors1[1].getVector3d()

err= (R1[0]+R2[0]-f)**2 # equlibrium on x
err+= (R1[2]+R2[2]+f)**2 # equlibrium on y
err+= (R1[0]*leverArm-R1[2]*leverArm+R2[0]*leverArm+R2[2]*leverArm)**2 # moment equilibrium

'''
print(R1)
print(R2)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(err)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

