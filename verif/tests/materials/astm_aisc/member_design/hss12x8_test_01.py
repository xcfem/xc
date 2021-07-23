# -*- coding: utf-8 -*-
# home made test
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE E.10 Rectangular HSS compression member with slender elements

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from materials.astm_aisc import ASTM_materials

foot2m= .3048
in2m= 0.0254
MPa2ksi= 0.145038
kN2kips= 0.2248
m2Toin2= 1.0/in2m**2

steel= ASTM_materials.A500
shape= ASTM_materials.HSSShape(steel,'HSS12X8X3/16')

effectiveLengthX= 1.0
effectiveLengthY= 24*foot2m
effectiveLengthZ= effectiveLengthY
'''
lambda_r= shape.getLambdaRCompression()
bClassification= shape.getBClassification()
hClassification= shape.getHClassification()
Fcr= shape.getCompressiveCriticalStress(effectiveLengthX, effectiveLengthY, effectiveLengthZ)
compressiveBucklingReductionLimit= shape.getCompressiveBucklingReductionLimit(Fcr)
h_eff= shape.getReducedEffectiveH(compressiveBucklingReductionLimit, Fcr)
b_eff= shape.getReducedEffectiveB(compressiveBucklingReductionLimit, Fcr)
A_eff= shape.getCompressionEffectiveArea(Fcr)
'''
Pn= shape.getNominalCompressiveStrength(effectiveLengthX, effectiveLengthY, effectiveLengthZ)
PnRef= 735.8325968753835e3
ratio1= abs(Pn-PnRef)/PnRef

'''
print('lambda_r= ', lambda_r)
print('hClassification: ', hClassification)
print('bClassification: ', bClassification)
print('Fcr: ', Fcr/1e6, ' MPa', Fcr/1e6*MPa2ksi, ' ksi')
print('compressive buckling reduction limit: ', compressiveBucklingReductionLimit)
print('h_eff: ', h_eff*1e3, ' mm', h_eff/in2m, ' in')
print('b_eff: ', b_eff*1e3, ' mm', b_eff/in2m, ' in')
print('A_eff: ', A_eff*1e4, 'cm2', A_eff*m2Toin2, 'in2')
print('Pn: ', Pn/1e3, ' kN', Pn/1e3*kN2kips, ' kips')
print('ratio1: ', ratio1)
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
