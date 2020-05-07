# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE H.1B W-SHAPE SUBJECT TO COMBINED COMPRESSION AND BENDING ABOUT
# BOTH AXES (BRACED FRAME)

from __future__ import division
from __future__ import print_function

import xc_base
import geom
import xc
from materials.astm_aisc import ASTM_materials
from materials.astm_aisc import AISC_limit_state_checking as aisc

inch2meter= 0.0254
MPa2ksi= 0.145038
kN2kips= 0.2248
N2kips= 0.2248e-3
kip2kN= 1.0/kN2kips
kip2N= 1e3*kip2kN
foot2meter= 0.3048
m2Toin2= 1.0/inch2meter**2

#Materials
## Steel material
steel= ASTM_materials.A992
steel.gammaM= 1.00
## Shape geometry
shape= ASTM_materials.WShape(steel,'W14X99')
## Steel column
column= aisc.Member('column', shape,unbracedLengthX= 14.0*foot2meter, Cb= 1.0)

## Load.
Pd= -400.0*kip2N
Mzd= 250.0*kip2N*foot2meter
Myd= 80.0*kip2N*foot2meter

## Compressive strength
CF= column.getBiaxialBendingEfficiency(Nd= Pd, Myd= Myd, Mzd= Mzd)
NcRd= CF[1] # available axial strength.
NcRdRef= 1130*kip2N
McRdy= CF[2] # available flexural strength minor axis.
McRdz= CF[3] # available flexural strength minor axis.
ratio1= abs(CF[0]-0.9233821660167778)/0.9233821660167778
ratio2= abs(NcRd-5059.76358542e3)/5059.763585423e3
ratio3= abs(McRdy-425.385e3)/425.385e3
ratio4= abs(McRdz-878.715e3)/878.715e3
ratio5= abs(NcRd-NcRdRef)/NcRdRef

'''
print('CF= ', CF[0])
print('Pd= ',Pd/1e3,' kN (',Pd*N2kips,'kips)')
print('NcRd= ',NcRd/1e3,' kN (',NcRd*N2kips,'kips)')
print('McRdy= ',McRdy/1e3,' kN m (',McRdy*N2kips/foot2meter,'kip-ft)')
print('McRdz= ',McRdz/1e3,' kN m (',McRdz*N2kips/foot2meter,'kip-ft)')
print('ratio1= ',ratio1)
print('ratio2= ',ratio2)
print('ratio3= ',ratio3)
print('ratio4= ',ratio4)
print('ratio5= ',ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-6 and ratio2<1e-6 and ratio3<1e-6 and ratio4<1e-6 and ratio5<1e-2):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
