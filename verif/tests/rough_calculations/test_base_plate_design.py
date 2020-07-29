# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function
''' Base plate design according to document
    Base Plate and Anchor Rod Design Second Edition
    American Institute of Steel Construction, Inc.
'''

import math
from materials.astm_aisc import ASTM_materials
from rough_calculations import ng_aisc_base_plates as bp

''' Verification test from Example 4.1'''
kip2N= 4448.2216
ksi2Pa= 6.89476e6
in2m= 0.0254

    
def myround(x, prec=2, base=.05):
  return round(base * round(float(x)/base),prec)

steel_W= ASTM_materials.A992   #steel support W shapes
steel_W.gammaM= 1.00
steelShape= ASTM_materials.WShape(steel_W,'W12X96')
fc= 3*ksi2Pa # Minimum concrete compressive strength
Pu= 700*kip2N # Axial load
Fy= 36*ksi2Pa # base plate yield stress

Areq= bp.computeBasePlateArea(Pu, fc)
N= bp.getNDimension(steelShape, Areq)
N= myround(N)
B= myround(Areq/N)
A= B*N
Pp= bp.getConcreteStrength(A,fc)
CF= Pu/Pp
ratio1= (CF-0.990784195587)/0.990784195587
t= bp.computePlateThickness(steelShape, N, B, Pu, Pp, Fy)
ratio2= (t-40.1170824974e-3)/40.1170824974e-3

'''
print('Areq= ', Areq, ' m2 (',Areq/in2m/in2m,' in2).')
print('N= ', N, ' m (',N/in2m,' in).')
print('B= ', B, ' m (',B/in2m,' in).')
print('A= ', A, ' m2 (',A/in2m/in2m,' in2).')
print('CF= ', CF)
print('t= ', t*1000, ' mm (',t/in2m,' in).')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) and (abs(ratio2)<1e-6):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')


