# -*- coding: utf-8 -*-
''' Test properties of AISC W shapes.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec3 import EC3_materials
from materials.sections.structural_shapes import plate_shapes

steel= EC3_materials.S275JR
b= 15e-3
h= 0.2
shape= plate_shapes.PlateShape(steel= steel, name= 'test', thickness= b, width= h)

# Iz inertia (strong axis).
Iz= shape.Iz()
refIz= b*h**3/12.0
ratio1= abs(Iz-refIz)/refIz
# Iy inertia (weak axis).
Iy= shape.Iy()
refIy= h*b**3/12.0
ratio2= abs(Iy-refIy)/refIy

'''
print(Iz, ratio1)
print(Iy, ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-12) & (ratio2<1e-12)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
