# -*- coding: utf-8 -*-
''' Dummy test to check rectangular hollow section class.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import math
from scipy.constants import g
from materials.ec3 import EC3_materials
from rough_calculations import ng_simple_beam as sb

#Materials
## Steel material
steel= EC3_materials.S275JR
steel.gammaM= 1.00

## Steel sections
section1= EC3_materials.RHSShape(steel= steel, name= 'RHS120x60x5')
section2= EC3_materials.RHSShape(steel= steel, name= 'RHS200x80x6')


## Compute the deflection under self weight (section1, span: 5.0 m).
E= section1.steelType.E
I1= section1.Iz()
L1= 5.0
beam1= sb.SimpleBeam(E= E, I= I1,l= L1)
w1= section1.getRho()*g
f1= beam1.getDeflectionUnderUniformLoad(q= w1, x= L1/2.0)

## Compute the deflection under self weight (section2, span: 6.4 m).
I2= section2.Iz()
L2= 6.4
beam2= sb.SimpleBeam(E= E, I= I2,l= L2)
w2= section2.getRho()*g
f2= beam2.getDeflectionUnderUniformLoad(q= w2, x= L2/2.0)

weightRatio= w2/w1
deflectionRatio= f2/f1

'''
print('Deflection with shape '+section1.name+ ':'+str(f1*1e3)+' mm')
print('Deflection with shape '+section2.name+ ':'+str(f2*1e3)+' mm')
print('weightRatio= ', weightRatio)
print('deflectionRatio= ', deflectionRatio)
'''

testOK= ((abs(weightRatio-1.9095015576323988)/1.9095015576323988)<1e-4 and (abs(deflectionRatio-0.9956172658517105)/0.9956172658517105)<1e-4)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

