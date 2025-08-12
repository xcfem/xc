# -*- coding: utf-8 -*-
''' Test based on the document: 

COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
Volume 1: Design Examples
EXAMPLE H.1B W-SHAPE SUBJECT TO COMBINED COMPRESSION AND BENDING ABOUT
BOTH AXES (BRACED FRAME)
'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from materials.astm_aisc import ASTM_materials
from materials.astm_aisc import AISC_limit_state_checking as aisc
from misc_utils import units_utils

#Materials
## Steel material
steel= ASTM_materials.A992
steel.gammaM= 1.00
## Shape geometry
shape= ASTM_materials.WShape(steel,'W14X99')
## Steel column
column= aisc.Member('column', shape, unbracedLengthX= 14.0*units_utils.footToMeter, Cb= 1.0)

## Load.
Pd= -400.0*units_utils.kipToN
Myd= 80.0*units_utils.kipToN*units_utils.footToMeter
Mzd= 250.0*units_utils.kipToN*units_utils.footToMeter

## Compressive strength
CF= column.getBiaxialBendingEfficiency(Nd= Pd, Myd= Myd, Mzd= Mzd)
ratio1= abs(CF[0]-0.9243540648400457)/0.9243540648400457
NcRd= CF[1] # available axial strength.
ratio2= abs(NcRd-5059.76358542e3)/5059.763585423e3
NcRdRef= 1130*units_utils.kipToN
ratio5= abs(NcRd-NcRdRef)/NcRdRef
McRdy= CF[2] # available flexural strength minor axis.
McRdyRef= 421.659398e3
ratio3= abs(McRdy-McRdyRef)/McRdyRef
McRdz= CF[3] # available flexural strength major axis.
McRdzRef= 870.435156e3
ratio4= abs(McRdz-McRdzRef)/McRdzRef

'''
print('CF= ', CF[0])
print('ratio1= ',ratio1)
print('Pd= ',Pd/1e3,' kN (',Pd/units_utils.kipToN,'kips)')
print('NcRd= ',NcRd/1e3,' kN (',NcRd/units_utils.kipToN,'kips)')
print('ratio2= ',ratio2)
print('NcRdRef= ',NcRdRef/1e3,' kN (',NcRdRef/units_utils.kipToN,'kips)')
print('ratio5= ',ratio5)
print('McRdy= ',McRdy/1e3,' kN m (',McRdy/units_utils.kipToN/units_utils.footToMeter,'kip-ft)')
print('McRdyRef= ',McRdyRef/1e3,' kN m (',McRdyRef/units_utils.kipToN/units_utils.footToMeter,'kip-ft)')
print('ratio3= ',ratio3)
print('McRdz= ',McRdz/1e3,' kN m (',McRdz/units_utils.kipToN/units_utils.footToMeter,'kip-ft)')
print('McRdzRef= ',McRdzRef/1e3,' kN m (',McRdzRef/units_utils.kipToN/units_utils.footToMeter,'kip-ft)')
print('ratio4= ',ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-6 and ratio2<1e-6 and ratio3<1e-2 and ratio4<1e-2 and ratio5<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
