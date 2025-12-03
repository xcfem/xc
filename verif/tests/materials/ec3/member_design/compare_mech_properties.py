# -*- coding: utf-8 -*-
''' Compare mechanical properties of a C10X25 ASTM steel shape and a UPN_260
    European steel shape.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
from materials.astm_aisc import ASTM_materials
from materials.sections.structural_shapes import aisc_metric_shapes
from materials.ec3 import EC3_materials

#Materials
## Steel material
usSteel= ASTM_materials.A36
usSteel.gammaM= 1.00
euSteel= EC3_materials.S275JR
euSteel.gammaM= 1.00

## Profile geometry
usLabel= aisc_metric_shapes.getUSLabel('C250X37')
usShape= ASTM_materials.CShape(usSteel,usLabel)
euLabel= 'UPN_260'
euShape= EC3_materials.UPNShape(euSteel,euLabel)
ratioStr= usLabel+'/'+euLabel

aRatio= usShape.A()/euShape.A()
izRatio= usShape.Iz()/euShape.Iz()
iyRatio= usShape.Iy()/euShape.Iy()
shearRatio= usSteel.fyd()*usShape.getShearArea()/(euSteel.fyd()*euShape.getShearArea())

error= math.sqrt((aRatio-0.9813664596273293)**2+(izRatio-0.7863070539419087)**2+(iyRatio-0.4384858044164038)**2+(shearRatio-1.140922499329579)**2)

'''
print('US label: ', usLabel)
print('area ratio A('+ratioStr+')=',aRatio)
print('area ratio Iz('+ratioStr+')=',izRatio)
print('area ratio Iy('+ratioStr+')=',iyRatio)
print('area ratio fyd*Avy('+ratioStr+')=', shearRatio)
print('error= ', error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
