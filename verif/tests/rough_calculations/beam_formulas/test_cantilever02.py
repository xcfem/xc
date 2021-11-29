# -*- coding: utf-8 -*-
''' Test fundamental bending frequencies calculation for simple beams.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.ec3 import EC3_materials # Steel shapes.
from rough_calculations import ng_cantilever as cv

steelType= EC3_materials.S235JR # steel type
shape= EC3_materials.IPEShape(steel= steelType, name= 'IPE_200')
E= shape.steelType.E
I= shape.Iz()
L= 12
beam= cv.Cantilever(E= E, I= I,l= L)

frequencies= beam.getFundamentalBendingFrequencies(3, shape.getRho())
frequency= frequencies[0]
refFrequency= 0.56*math.sqrt(E*I/shape.getRho()/L**4)
ratio= abs(frequency-refFrequency)/refFrequency

'''
print(frequency/math.sqrt(E*I/shape.getRho()/L**4))
print('f= ', frequency, 'Hz')
print('f_ref= ', refFrequency, 'Hz')
print("ratio= ", ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-3:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

