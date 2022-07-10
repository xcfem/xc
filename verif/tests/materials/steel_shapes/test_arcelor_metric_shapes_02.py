# -*- coding: utf-8 -*-
''' Properties of 356 x 127 x 33 UB (Universal Beam). Data from:

   Handbook of Structural Steelwork (Eurocode Edition)

   The British Constructional Steelwork Association Ltd 
   4 Whitehall Court London SW1A 2ES

   The Steel Construction Institute
   Silwood Park
   Ascot
   SL5 7QN
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

from materials.ec3 import EC3_materials
from materials.sections.structural_shapes import arcelor_metric_shapes


shape= arcelor_metric_shapes.UBShape(steel= EC3_materials.S275JR, name= 'UB356x127x33')


cf_tf= float(shape.get('cf_tf'))
cf_tf_ref= 5.82
ratio0= abs(cf_tf-cf_tf_ref)/cf_tf_ref

A= shape.A()
ARef= 42.1e-4
ratio1= abs(A-ARef)/ARef

cw_tw= float(shape.get('cw_tw'))
cw_tw_ref= 51.9
ratio2= abs(cw_tw-cw_tw_ref)/cw_tw_ref

tf= shape.tf()
tf_ref= 8.5e-3
ratio3= abs(tf-tf_ref)/tf_ref

iz= shape.iz()
iz_ref= 14e-2
ratio4= abs(iz-iz_ref)/iz_ref

h= shape.h()
b= shape.b()
h_b= h/b
h_b_ref= 2.78
ratio5= abs(h_b-h_b_ref)/h_b_ref

tw= shape.tw()
tw_ref= 6e-3
ratio6= abs(tw-tw_ref)/tw_ref

iy= shape.iy()
iy_ref= 2.58e-2
ratio7= abs(iy-iy_ref)/iy_ref

hi= float(shape.get('hi'))
hi_ref= 332e-3
ratio8= abs(hi-hi_ref)/hi_ref

r= float(shape.get('r'))
r_ref= 10.2e-3
ratio9= abs(r-r_ref)/r_ref

h_ref= 349e-3
ratio10= abs(h-h_ref)/h_ref

b_ref= 125.4e-3
ratio11= abs(b-b_ref)/b_ref

'''
print('c_f/t_f= ', cf_tf)
print('reference value c_f/t_f= ', cf_tf_ref)
print('ratio0= ', ratio0)

print('ratio1=', ratio1)

print('c_w/t_w= ', cw_tw)
print('reference value c_w/t_w= ', cw_tw_ref)
print('ratio2=', ratio2)

print('tf= ', tf)
print('reference value tf= ', tf_ref)
print('ratio3=', ratio3)

print('iz= ', iz)
print('reference value iz= ', iz_ref)
print('ratio4=', ratio4)

print('h_b= ', h_b)
print('reference value h_b= ', h_b_ref)
print('ratio5=', ratio5)

print('tw= ', tw)
print('reference value tw= ', tw_ref)
print('ratio6=', ratio6)

print('iy= ', iy)
print('reference value iy= ', iy_ref)
print('ratio7=', ratio7)

print('hi= ', hi)
print('reference value hi= ', hi_ref)
print('ratio8=', ratio8)

print('r= ', r)
print('reference value r= ', r_ref)
print('ratio9=', ratio9)

print('h= ', h)
print('reference value h= ', h_ref)
print('ratio10=', ratio10)

print('b= ', b)
print('reference value b= ', b_ref)
print('ratio11=', ratio11)
'''

ok= (abs(ratio0)< 0.01) and (abs(ratio1)<1e-12) and (abs(ratio2)<0.002) and (abs(ratio3)<1e-12) and (abs(ratio4)<0.001) and (abs(ratio5)<0.005) and (abs(ratio6)<1e-12) and (abs(ratio7)<0.0005) and (abs(ratio8)<1e-12) and (abs(ratio9)<0.02) and (abs(ratio10)<1e-12) and (abs(ratio11)<1e-12)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ok:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
