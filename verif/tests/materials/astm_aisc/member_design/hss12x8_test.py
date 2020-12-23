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
m2Toin2= 1.0/in2m**2

steel= ASTM_materials.A500
hss= ASTM_materials.HSSShape(steel,'HSS12X8X3/16')
connection= ASTM_materials.MemberConnection(L= 30*0.3048,rotI='fixed',transI='fixed',rotJ= 'free',transJ= 'fixed')
member= ASTM_materials.ConnectedMember(hss,connection)

k= member.connection.getEffectiveBucklingLengthCoefficientRecommended()
lambda_r= member.shape.getLambdaRCompression()
bClassification= member.shape.getBClassification()
hClassification= member.shape.getHClassification()
h_eff= member.shape.getReducedEffectiveH()
b_eff= member.shape.getReducedEffectiveB()
A_eff= member.shape.getEffectiveArea()
sr= member.getSlendernessRatio()
Fe= member.getElasticBucklingStress()
Fcr= member.getCriticalStress()
Pn= member.getNominalCompressiveStrength()

ratio1= (Pn-612.382011207e3)/-612.382011207e3

'''
print('Fy= ', steel.fy/1e6*MPa2ksi,' ksi')
print('Fu= ', steel.fu/1e6*MPa2ksi,' ksi')
print('k= ', k)
print('lambda_r= ', lambda_r)
print('hClassification: ', hClassification)
print('bClassification: ', bClassification)
print('h_eff: ', h_eff*1e3, ' mm', h_eff/in2m, ' in')
print('b_eff: ', b_eff*1e3, ' mm', b_eff/in2m, ' in')
print('A_eff: ', A_eff*1e4, 'cm2', A_eff*m2Toin2, 'in2')
print('slenderness ratio: ', sr)
print('Fe: ', Fe/1e6, ' MPa', Fe/1e6*MPa2ksi, ' ksi')
print('Fcr: ', Fcr/1e6, ' MPa', Fcr/1e6*MPa2ksi, ' ksi')
print('Pn: ', Pn/1e3, ' kN', Pn/1e3*kN2kips, ' ksi')
print('ratio1: ', ratio1)
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-12):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
