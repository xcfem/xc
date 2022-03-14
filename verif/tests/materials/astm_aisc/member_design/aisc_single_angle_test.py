# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE F.11A SINGLE-ANGLE FLEXURAL MEMBER WITH BRACING AT ENDS ONLY

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from materials.astm_aisc import ASTM_materials
from materials.astm_aisc import AISC_limit_state_checking as aisc
from model import predefined_spaces

from misc_utils import units_utils


MPa2ksi= 0.145038
kN2kips= 0.2248
kip2kN= 1.0/kN2kips

m2Toin2= 1.0/units_utils.inchToMeter**2

#Materials

## Steel material
steel= ASTM_materials.A36
steel.gammaM= 1.00

## Profile geometry
# *************************************************************************
# AISC single angle profiles.
# *************************************************************************
#         Section axis:         *                *
#      AISC            XC       *    Points:     *    Principal axes:
#                               *                *
#         ^ Y           ^ y     *                *   
#         |             |       *                *    Z-axis
#                               *     A          *       \  |       W-axis
#       |             |         *      |         *        \ |     /
#       | -> X        | -> z    *      |         *         \|   /
#       |             |         *      |         *          | +
#       -------       -------   *     B-------C  *          ----------
#
# *************************************************************************

# The axis used in AISC catalogue are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.
shape= ASTM_materials.LShape(steel,'L4X4X1/4')

# Z-axis available flexural strength.
zAxisPlasticMoment= shape.getZAxisPlasticMoment()
zAxisPlasticMomentRef= 4.8e3
ratio1= abs(zAxisPlasticMoment-zAxisPlasticMomentRef)/zAxisPlasticMomentRef
zAxisElasticLateralTorsionalBucklingMoment= shape.getZAxisElasticLateralTorsionalBucklingMoment()
zAxisElasticLateralTorsionalBucklingMomentRef= zAxisPlasticMomentRef
ratio2= abs(zAxisElasticLateralTorsionalBucklingMoment-zAxisElasticLateralTorsionalBucklingMomentRef)/zAxisElasticLateralTorsionalBucklingMomentRef
zAxisLegLocalBucklingLimit= shape.getZAxisLegLocalBucklingLimit(MZ= 1.0)
zAxisLegLocalBucklingLimitRef= 5.168560188515125e3
ratio3= abs(zAxisLegLocalBucklingLimit-zAxisLegLocalBucklingLimitRef)/zAxisLegLocalBucklingLimitRef
zAxisNominalFlexuralStrength= shape.getZAxisNominalFlexuralStrength(MZ= 1.0)
zAxisNominalFlexuralStrengthRef= zAxisPlasticMomentRef
ratio4= abs(zAxisNominalFlexuralStrength-zAxisNominalFlexuralStrengthRef)/zAxisNominalFlexuralStrengthRef

# W-axis available flexural strength.
wAxisPlasticMoment= shape.getWAxisPlasticMoment()
wAxisPlasticMomentRef= 10.8e3
ratio5= abs(wAxisPlasticMoment-wAxisPlasticMomentRef)/wAxisPlasticMomentRef
wAxisElasticLateralTorsionalBucklingMoment= shape.getWAxisElasticLateralTorsionalBucklingMoment(Lb= 6*units_utils.footToMeter, Cb= 1.14)
wAxisElasticLateralTorsionalBucklingMomentRef= 23.262011718749992e3
ratio6= abs(wAxisElasticLateralTorsionalBucklingMoment-wAxisElasticLateralTorsionalBucklingMomentRef)/wAxisElasticLateralTorsionalBucklingMomentRef
wAxisLateralTorsionalBucklingLimit= shape.getWAxisLateralTorsionalBucklingLimit(Lb= 6*units_utils.footToMeter, Cb= 1.14)[0]
wAxisLateralTorsionalBucklingLimitRef= 9.137366729071001e3
ratio7= abs(wAxisLateralTorsionalBucklingLimit-wAxisLateralTorsionalBucklingLimitRef)/wAxisLateralTorsionalBucklingLimitRef
wAxisLegLocalBucklingLimit= shape.getWAxisLegLocalBucklingLimit()[0]
wAxisLegLocalBucklingLimitRef= 10.632466673516829e3
ratio8= abs(wAxisLegLocalBucklingLimit-wAxisLegLocalBucklingLimitRef)/wAxisLegLocalBucklingLimitRef
wAxisNominalFlexuralStrength= shape.getWAxisNominalFlexuralStrength(lateralUnbracedLength= 6*units_utils.footToMeter, Cb= 1.14, MW= 1.0)
wAxisNominalFlexuralStrengthRef= wAxisLateralTorsionalBucklingLimitRef
ratio9= abs(wAxisNominalFlexuralStrength-wAxisNominalFlexuralStrengthRef)/wAxisNominalFlexuralStrengthRef

'''
print('zAxisPlasticMoment= ',zAxisPlasticMoment/1e3,' kN m (',zAxisPlasticMoment/1e3*kN2kips/units_utils.footToMeter,' kip-ft or', zAxisPlasticMoment/1e3*kN2kips/units_utils.inchToMeter,' kip-in)')
print('ratio1= ',ratio1)
print('zAxisElasticLateralTorsionalBucklingMoment= ',zAxisElasticLateralTorsionalBucklingMoment/1e3,' kN m (',zAxisElasticLateralTorsionalBucklingMoment/1e3*kN2kips/units_utils.footToMeter,' kip-ft or', zAxisElasticLateralTorsionalBucklingMoment/1e3*kN2kips/units_utils.inchToMeter,' kip-in)')
print('ratio2= ',ratio2)
print('zAxisLegLocalBucklingLimit= ',zAxisLegLocalBucklingLimit/1e3,' kN m (',zAxisLegLocalBucklingLimit/1e3*kN2kips/units_utils.footToMeter,' kip-ft or', zAxisLegLocalBucklingLimit/1e3*kN2kips/units_utils.inchToMeter,' kip-in)')
print('ratio3= ',ratio3)
print('zAxisNominalFlexuralStrength= ',zAxisNominalFlexuralStrength/1e3,' kN m (',zAxisNominalFlexuralStrength/1e3*kN2kips/units_utils.footToMeter,' kip-ft or', zAxisNominalFlexuralStrength/1e3*kN2kips/units_utils.inchToMeter,' kip-in)')
print('ratio4= ',ratio4)

print('wAxisPlasticMoment= ',wAxisPlasticMoment/1e3,' kN m (',wAxisPlasticMoment/1e3*kN2kips/units_utils.footToMeter,' kip-ft or', wAxisPlasticMoment/1e3*kN2kips/units_utils.inchToMeter,' kip-in)')
print('ratio5= ',ratio5)
print('wAxisElasticLateralTorsionalBucklingMoment= ',wAxisElasticLateralTorsionalBucklingMoment/1e3,' kN m (',wAxisElasticLateralTorsionalBucklingMoment/1e3*kN2kips/units_utils.footToMeter,' kip-ft or', wAxisElasticLateralTorsionalBucklingMoment/1e3*kN2kips/units_utils.inchToMeter,' kip-in)')
print('ratio6= ',ratio6)
print('wAxisLateralTorsionalBucklingLimit= ',wAxisLateralTorsionalBucklingLimit/1e3,' kN m (',wAxisLateralTorsionalBucklingLimit/1e3*kN2kips/units_utils.footToMeter,' kip-ft or', wAxisLateralTorsionalBucklingLimit/1e3*kN2kips/units_utils.inchToMeter,' kip-in)')
print('ratio7= ',ratio7)
print('wAxisLegLocalBucklingLimit= ',wAxisLegLocalBucklingLimit/1e3,' kN m (',wAxisLegLocalBucklingLimit/1e3*kN2kips/units_utils.footToMeter,' kip-ft or', wAxisLegLocalBucklingLimit/1e3*kN2kips/units_utils.inchToMeter,' kip-in)')
print('ratio8= ',ratio8)
print('wAxisNominalFlexuralStrength= ',wAxisNominalFlexuralStrength/1e3,' kN m (',wAxisNominalFlexuralStrength/1e3*kN2kips/units_utils.footToMeter,' kip-ft or', wAxisNominalFlexuralStrength/1e3*kN2kips/units_utils.inchToMeter,' kip-in)')
print('ratio9= ',ratio9)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-6 and ratio2<1e-6 and ratio3<1e-6 and ratio4<1e-6 and ratio5<1e-6 and ratio6<1e-6 and ratio7<1e-6 and ratio8<1e-6 and ratio9<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

