# -*- coding: utf-8 -*-
''' Trivial test for the DeformationPlane class.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc

testVector= xc.Vector([1e-3, 2e-4, 3e-4])
defPlane= xc.DeformationPlane(testVector)

# epsilon: axial strain
# curvZ: curvature around Z axis.
# curvY: curvature around Y axis.
strainsA= defPlane.strains # generalized strains (epsilon, curvZ, curvY)
errA= (strainsA-testVector).Norm()

testVectorB= xc.Vector([testVector[0], testVector[1]*0.5, testVector[2]/3])
defPlane.strains= testVectorB
strainsB= defPlane.strains
errB= (strainsB-testVectorB).Norm()

'''
print(strainsA, errA)
print(strainsB, errB)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(errA)<1e-12) & (abs(errB)<1e-12)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
