# -*- coding: utf-8 -*-
# home made test
# Rectangular HSS compression member with slender elements

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from materials.astm_aisc import ASTM_materials

in2m= 0.0254
MPa2ksi= 0.145038
kN2kips= 0.2248
kNm2kipsft= 0.737562121169657
m2Toin2= 1.0/in2m**2

steel= ASTM_materials.A500
shape= ASTM_materials.HSSShape(steel,'HSS8X8X3/16')

# Effective section (AISC-360-16 section F7).
Ieff= shape.getEffectiveInertia(majorAxis= True)
IeffRef= 2.19026289469e-05
Seff= shape.getEffectiveSectionModulus(majorAxis= True)
SeffRef= 0.000211604241488
## Flange local buckling (F7.2)
Mn= shape.getFlangeLocalBucklingLimit(majorAxis= True)
MnRef= 66.6553360686e3

ratio1= abs(Ieff-IeffRef)/IeffRef
ratio2= abs(Seff-SeffRef)/SeffRef
ratio3= abs(Mn-MnRef)/MnRef

'''
print('Fy= ', steel.fy/1e6,' MPa (',steel.fy/1e6*MPa2ksi,' ksi)')
print('Ieff: ', Ieff, ' m4(', Ieff/(in2m**4), ' in4)')
print('ratio1: ', ratio1)
print('Seff: ', Seff, ' m3(', Seff/(in2m**3), ' in3)')
print('ratio2: ', ratio2)
print('Fy*Seff= ',  steel.fy*Seff/1e3)
print('Mn: ', Mn/1e3, ' kN m', Mn/1e3*kNm2kipsft, ' kips ft')
print('ratio3: ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-8 and ratio2<1e-8 and ratio3<1e-8):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
