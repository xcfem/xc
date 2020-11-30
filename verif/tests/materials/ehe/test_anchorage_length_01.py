# -*- coding: utf-8 -*-
''' Test calculation of anchorage lengths according to EHE.
    Home made test.'''

from __future__ import division
from __future__ import print_function

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
import os
from misc_utils import log_messages as lmsg

__author__= "Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2020, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Rebar controller.
cCover= .05
rebarControllerPosI= EHE_limit_state_checking.RebarController(concreteCover= cCover, pos= 'I')
rebarControllerPosII= EHE_limit_state_checking.RebarController(concreteCover= cCover, pos= 'II')

# Basic anchorage length.
## Steel B500S
### Position I
concrete= EHE_materials.HA25
ld6I500= rebarControllerPosI.getBasicAnchorageLength(concrete,6e-3,EHE_materials.B500S)
ld6I500Ref= 0.15
ratio1= abs(ld6I500-ld6I500Ref)/ld6I500Ref
ld25I500= rebarControllerPosI.getBasicAnchorageLength(concrete,25e-3,EHE_materials.B500S)
ld25I500Ref= 0.9375
ratio2= abs(ld25I500-ld25I500Ref)/ld25I500Ref

### Position II
ld6II500= rebarControllerPosII.getBasicAnchorageLength(concrete,6e-3,EHE_materials.B500S)
ld6II500Ref= 0.214285714286
ratio3= abs(ld6II500-ld6II500Ref)/ld6II500Ref
ld25II500= rebarControllerPosII.getBasicAnchorageLength(concrete,25e-3,EHE_materials.B500S)
ld25II500Ref= 1.3125
ratio4= abs(ld25II500-ld25II500Ref)/ld25II500Ref

## Steel B400S
### Position I
ld6I400= rebarControllerPosI.getBasicAnchorageLength(concrete,6e-3,EHE_materials.B400S)
ld6I400Ref= 0.12
ratio5= abs(ld6I400-ld6I400Ref)/ld6I400Ref
ld25I400= rebarControllerPosI.getBasicAnchorageLength(concrete,25e-3,EHE_materials.B400S)
ld25I400Ref= 0.75
ratio6= abs(ld25I400-ld25I400Ref)/ld25I400Ref

### Position II
ld6II400= rebarControllerPosII.getBasicAnchorageLength(concrete,6e-3,EHE_materials.B400S)
ld6II400Ref= 0.171428571429
ratio7= abs(ld6II400-ld6II400Ref)/ld6II400Ref
ld25II400= rebarControllerPosII.getBasicAnchorageLength(concrete,25e-3,EHE_materials.B400S)
ld25II400Ref= 1.05
ratio8= abs(ld25II400-ld25II400Ref)/ld25II400Ref

'''
print('ld6I500= ', ld6I500,' m')
print('ratio1= ', ratio1)
print('ld25I500= ', ld25I500,' m')
print('ratio2= ', ratio2)

print('ld6II500= ', ld6II500,' m')
print('ratio3= ', ratio3)
print('ld25II500= ', ld25II500,' m')
print('ratio4= ', ratio4)

print('ld6I400= ', ld6I400,' m')
print('ratio5= ', ratio5)
print('ld25I400= ', ld25I400,' m')
print('ratio6= ', ratio6)

print('ld6II400= ', ld6II400,' m')
print('ratio7= ', ratio7)
print('ld25II400= ', ld25II400,' m')
print('ratio8= ', ratio8)
'''

fname= os.path.basename(__file__)
if((ratio1<1e-6) and (ratio2<1e-6) and (ratio3<1e-6) and (ratio4<1e-6) and (ratio5<1e-6) and (ratio6<1e-6) and (ratio7<1e-6) and (ratio8<1e-6)):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
