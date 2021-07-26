# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE E.14 ECCENTRICALLY LOADED SINGLE-ANGLE COMPRESSION MEMBER
# (LONG LEG ATTACHED)

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import xc_base
import geom
from materials.astm_aisc import ASTM_materials
from materials.astm_aisc import AISC_limit_state_checking as aisc
from model import predefined_spaces

inch2meter= 0.0254
MPa2ksi= 0.145038
kN2kips= 0.2248
kip2kN= 1.0/kN2kips
foot2meter= 0.3048
m2Toin2= 1.0/inch2meter**2

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

shape= ASTM_materials.LShape(steel,'L8X4X1/2')
effectiveLengthX= 5*foot2meter
effectiveLengthY= effectiveLengthX
effectiveLengthZ= effectiveLengthY

P= geom.Pos2d(-3/4.0*shape.get('t'), shape.get('b_flat')/2.0)
principalAxesRef= shape.getPrincipalRefSys()
p= principalAxesRef.getLocalPosition(P)
ez= p.x
ratio1= abs(ez+22.61809192580656e-3)/22.61809192580656e-3
ew= p.y
ratio2= abs(ew-36.43875056359321e-3)/36.43875056359321e-3

lambda_R= shape.getLambdaRCompression()
ratio3= abs(lambda_R-13.042239071570494)/13.042239071570494

slendernessRatios= shape.getLegsSlendernessRatios()
classif= shape.getLegsCompressionClassification()
Fcr= shape.getCompressiveCriticalStress(effectiveLengthX= effectiveLengthX, effectiveLengthY= effectiveLengthY, effectiveLengthZ= effectiveLengthZ)
ratio4= abs(Fcr-195.77713639574648e6)/195.77713639574648e6
compressiveBucklingReductionLimit= shape.getCompressiveBucklingReductionLimit(Fcr)
Pn= shape.getNominalCompressiveStrength(effectiveLengthX= effectiveLengthX, effectiveLengthY= effectiveLengthY, effectiveLengthZ= effectiveLengthZ)
ratio5= abs(Pn-705.9555965531148e3)/705.9555965531148e3

beta_w= shape.getBetaW()
ratio6= abs(beta_w-139.0e-3)
Cb= 1.0 # Uniform moment along member length.
Lb= effectiveLengthZ
# Lateral torsional buckling.
Mcr= shape.getWAxisElasticLateralTorsionalBucklingMoment(Lb, Cb)[1] # negative value for beta_w
ratio7= abs(Mcr-84.50820680582561e3)/84.50820680582561e3
My= shape.getWAxisYieldMoment()
ratio8= abs(My-32.75e3)/32.75e3
Mn_lt= shape.getWAxisLateralTorsionalBucklingLimit(Lb, Cb)[1] # negative value for beta_w
ratio9= abs(Mn_lt-39.02643060922943e3)/39.02643060922943e3
# Leg local buckling
Mn_llb= shape.getWAxisLegLocalBucklingLimit()[0]
ratio10= abs(Mn_llb-48.51599302982588e3)/48.51599302982588e3
# W axis available flexural strength
#Pu= 69.15676398762e3/kN2kips # Problem data.
Pu= 67.7e3/kN2kips # Problem data.
MW= -Pu*ew*(1.0/(1-Pu/(3260e3/kN2kips))) # Problem data
MnW= shape.getWAxisNominalFlexuralStrength(Lb, Cb, MW)
ratio11= abs(MnW-Mn_lt)/Mn_lt
MZ= Pu*ez*(1.0/(1-Pu/(343e3/kN2kips))) # Problem data
MnZ= shape.getZAxisNominalFlexuralStrength(MZ)
ratio12= abs(MnZ-11.35e3)/11.35e3
Mzd, Myd= shape.getGeometricAxesMoments(MW, MZ)
efficiency= shape.getBiaxialBendingEfficiency(-Pu,Myd,Mzd)[0]
ratio13= abs(efficiency-1.034293557987282) 

'''
print('ew= ', ew*1e3, ' mm (', ew/inch2meter, ' in)')
print('ratio1= ',ratio1)
print('ez= ', ez*1e3, ' mm (', ez/inch2meter, ' in)')
print('ratio2= ',ratio2)
print('lambda_R= ', lambda_R)
print('ratio3= ',ratio3)
print(slendernessRatios)
print(classif)
print('Fcr: ', Fcr/1e6, ' MPa', Fcr/1e6*MPa2ksi, ' ksi')
print('ratio4= ',ratio4)
print('compressiveBucklingReductionLimit: ', compressiveBucklingReductionLimit)
print('Pn: ', Pn/1e3, ' kN', Pn/1e3*kN2kips, ' kips')
print('ratio5= ',ratio5)
print('beta_w: ', beta_w*1e3, 'mm (', beta_w/inch2meter, ' in)')
print('ratio6= ',ratio6)
print('Mcr= ',Mcr/1e3,' kN m (',Mcr/1e3*kN2kips/foot2meter,' kip-ft or', Mcr/1e3*kN2kips/inch2meter,' kip-in)')
print('ratio7= ',ratio7)
print('My= ',My/1e3,' kN m (',My/1e3*kN2kips/foot2meter,' kip-ft or', My/1e3*kN2kips/inch2meter,' kip-in)')
print('ratio8= ',ratio8)
print('Limit due to lateral-torsional buckling Mn_lt= ',Mn_lt/1e3,' kN m (',Mn_lt/1e3*kN2kips/foot2meter,' kip-ft or', Mn_lt/1e3*kN2kips/inch2meter,' kip-in)')
print('ratio9= ',ratio9)
print('Limit due to leg local buckling Mn_llb= ',Mn_llb/1e3,' kN m (',Mn_llb/1e3*kN2kips/foot2meter,' kip-ft or', Mn_llb/1e3*kN2kips/inch2meter,' kip-in)')
print('ratio10= ',ratio10)
print('Available flexural strength for the W axis MnW= ',MnW/1e3,' kN m (',MnW/1e3*kN2kips/foot2meter,' kip-ft or', MnW/1e3*kN2kips/inch2meter,' kip-in)')
print('ratio11= ',ratio11)
print('Available flexural strength for the Z axis MnZ= ',MnZ/1e3,' kN m (',MnZ/1e3*kN2kips/foot2meter,' kip-ft or', MnZ/1e3*kN2kips/inch2meter,' kip-in)')
print('ratio12= ',ratio12)
print('MW= ',MW/1e3,' kN m (',MW/1e3*kN2kips/foot2meter,' kip-ft or', MW/1e3*kN2kips/inch2meter,' kip-in)')
print('MZ= ',MZ/1e3,' kN m (',MZ/1e3*kN2kips/foot2meter,' kip-ft or', MZ/1e3*kN2kips/inch2meter,' kip-in)')
print('efficiency: ',efficiency)
print('ratio13= ',ratio13)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-6 and ratio2<1e-6 and ratio3<1e-6 and ratio4<1e-6 and ratio5<1e-6 and ratio6<1e-6 and ratio7<1e-6 and ratio8<1e-6 and ratio9<1e-6 and ratio10<1e-6 and ratio11<1e-6 and ratio12<1e-6 and ratio13<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

