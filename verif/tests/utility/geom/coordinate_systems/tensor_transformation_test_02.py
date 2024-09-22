# -*- coding: utf-8 -*-
''' Trivial test of tensor transformation code.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

sigma_x= 90e6
sigma_y= -60e6
tau_xy= 20e6

# Compute principal stresses.
avg_sigma= (sigma_x+sigma_y)/2.0
square_root= math.sqrt(((sigma_x-sigma_y)/2.0)**2+tau_xy**2)
sigma_I= avg_sigma+square_root
sigma_II= avg_sigma-square_root
tan_two_theta= tau_xy/((sigma_x-sigma_y)/2.0)
theta= math.atan(tan_two_theta)/2.0

# Original stress tensor
stressTensor= geom.Matrix([[sigma_x, tau_xy],
                           [tau_xy, sigma_y]])

# Define coordinate system coincident with principal axes.
dirVector= geom.Vector2d(math.cos(theta), math.sin(theta))
cooSys= geom.CooSysRect2d2d(dirVector)
localStressTensor= cooSys.getLocalMatrix(stressTensor).getList()

sgI= localStressTensor[0][0]
sgII= localStressTensor[1][1]
tau= (localStressTensor[0][1]+localStressTensor[1][0])/2.0

ratio0= abs(sgI-sigma_I)/sigma_I
ratio1= abs(sgII-sigma_II)/sigma_II
ratio2= abs(tau)/abs(sigma_II)

testOK= (abs(ratio0)<1e-12) and (abs(ratio1)<1e-12) and (abs(ratio2)<1e-12)

'''
print(sigma_I/1e6)
print(sigma_II/1e6)
print(math.degrees(theta))
print(localStressTensor)
print(ratio0)
print(ratio1)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (testOK):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


