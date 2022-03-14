# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE H.3 W-SHAPE SUBJECT TO COMBINED AXIAL TENSION AND FLEXURE

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

MPa2ksi= 0.145038
kN2kips= 0.2248
N2kips= 0.2248e-3
kip2kN= 1.0/kN2kips


m2Toin2= 1.0/units_utils.inchToMeter**2

#Materials
## Steel material
steel= ASTM_materials.A992
steel.gammaM= 1.00
## Shape geometry
shape= ASTM_materials.WShape(steel,'W14X82')
## Steel column
column= aisc.Member('column', shape,unbracedLengthX= 30.0*units_utils.footToMeter, Cb= 1.41)

## Load.
Pd= 174.0*units_utils.kipToN
Mzd= 192.0*units_utils.kipToN*units_utils.footToMeter
Myd= 67.6*units_utils.kipToN*units_utils.footToMeter

## Compressive strength
CF= column.getBiaxialBendingEfficiency(Nd= Pd, Myd= Myd, Mzd= Mzd)
NcRd= CF[1] # available axial strength.
NcRdRef= 1080*units_utils.kipToN
McRdy= CF[2] # available flexural strength minor axis.
McRdz= CF[3] # available flexural strength major axis.
McRdzRefText= 492*units_utils.kipToN*units_utils.footToMeter
ratio1= abs(CF[0]-0.8623825305387269)/0.8623825305387269
ratio2= abs(NcRd-4812.75e3)/4812.75e3
ratio3= abs(McRdy-227.907e3)/227.907e3
ratio4= abs(McRdz-707.94e3)/707.94e3
ratio5= abs(NcRd-NcRdRef)/NcRdRef
ratio6= abs(McRdz-McRdzRefText)/McRdzRefText
# The difference here is due mainly to the different values of Lr
# Lr= 33.2 ft according to example H.3
# Lr= 34.4568988173 according to equation F2-6

'''
print('CF= ', CF[0])
print('Pd= ',Pd/1e3,' kN (',Pd/units_utils.kipToN,'kips)')
print('NcRd= ',NcRd/1e3,' kN (',NcRd/units_utils.kipToN,'kips)')
print('McRdy= ',McRdy/1e3,' kN m (',McRdy/units_utils.kipToN/units_utils.footToMeter,'kip-ft)')
print('McRdz= ',McRdz/1e3,' kN m (',McRdz/units_utils.kipToN/units_utils.footToMeter,'kip-ft)')
print('McRdzRefText/McRdz= ',McRdzRefText/McRdz)
print('ratio1= ',ratio1)
print('ratio2= ',ratio2)
print('ratio3= ',ratio3)
print('ratio4= ',ratio4)
print('ratio5= ',ratio5)
print('ratio6= ',ratio6)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-6 and ratio2<1e-6 and ratio3<1e-6 and ratio4<1e-6 and ratio5<1e-2 and ratio6<0.1):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
