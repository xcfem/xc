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
kNm2kipsft= 0.737562121169657
m2Toin2= 1.0/in2m**2

steel= ASTM_materials.A500
hss= ASTM_materials.HSSShape(steel,'HSS8X8X3/16')
connection= ASTM_materials.MemberConnection(L= 3.5052)
member= ASTM_materials.ConnectedMember(hss,connection)

Pd= -258.9561299844e3
k= member.connection.getEffectiveBucklingLengthCoefficientRecommended()
lambda_r= member.shape.getLimitingWidthToThicknessRatio()
bClassification= member.shape.getBClassification()
hClassification= member.shape.getHClassification()
h_eff= member.shape.getReducedEffectiveH()
b_eff= member.shape.getReducedEffectiveB()
A_eff= member.shape.getEffectiveArea()
sr= member.getSlendernessRatio()
Fe= member.getElasticBucklingStress()
Fcr= member.getCriticalStress()
Pn= member.getNominalCompressiveStrength()

# Z axis bending
Mz= Pd*0.05
bStateZ= ASTM_materials.BendingState(Mmax= Mz,Ma= Mz, Mb= Mz, Mc= Mz)
Mrz= member.getZNominalFlexuralStrength(bStateZ)
# Y axis bending
My= Pd*0.02
bStateY= ASTM_materials.BendingState(Mmax= My,Ma= My, Mb= My, Mc= My)
Mry= member.getYNominalFlexuralStrength(bStateY)


# Capacity factor.
FC= member.getCapacityFactor(Nd= Pd, Mzd= Mz, Myd= My, gammaC= 1.67, bendingStateZ= bStateZ, bendingStateY= bStateY)

ratio= (FC-0.971766509012)/0.971766509012

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
print('Pn: ', Pn/1e3, ' kN', Pn/1e3*kN2kips, ' kips')
print('Pn/Gc: ', Pn/1.67e3, ' kN', Pn/1.67e3*kN2kips, ' kips')
print('Mrz: ', Mrz/1e3, ' kN m', Mrz/1e3*kNm2kipsft, ' kips ft')
print('Mrz/Gc: ', Mrz/1.67e3, ' kN m', Mrz/1.67e3*kNm2kipsft, ' kips ft')
print('Mry: ', Mry/1e3, ' kN m', Mry/1e3*kNm2kipsft, ' kips ft')
print('Mry/Gc: ', Mry/1.67e3, ' kN m', Mry/1.67e3*kNm2kipsft, ' kips ft')
print('FC: ', FC)
print('ratio: ', ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio<1e-13):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
