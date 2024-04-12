# -*- coding: utf-8 -*-
''' Home made test.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from actions.wind import IAP_wind

T= 100 # year
k= 0.2
n= 0.5

B= 3.8
h_eq= 1.60
tC= 'III' # terrain category.
vb= 29 # basic wind velocity.
z= 5

c_prob= IAP_wind.getProbabilityFactor(T= 100.0, K= 0.2, n= 0.5)
c_fx= IAP_wind.getSolidWebBridgeDragForceCoefficient(B, h_eq)
f_wx= IAP_wind.getUnitWindLoadDeck(terrainCategory=tC, v_b0= vb, H_max= z)
eta= IAP_wind.getHidingFactor(relativeSpacement= 2.2, solidityRatio= 1.0)

percent= 0.6
pressureDistribution= IAP_wind.getTrapezoidalPressureDistribution(h= h_eq, heightFraction= percent, averagePressure= f_wx)
a= pressureDistribution(h_eq)
b= pressureDistribution(0.0)

p_vba= 1/(1+eta)*a
p_vbb= 1/(1+eta)*b
p_vsa= eta/(1+eta)*a
p_vsb= eta/(1+eta)*b

ratio1= math.sqrt((p_vba-2.3960720130932893e3)**2+(p_vbb-0.599018003273323e3)**2+(p_vsa-0.5319279869067104e3)**2+(p_vsb-0.13298199672667774e3)**2)

A= (a+b)/2.0*h_eq
lever= h_eq-h_eq/3.0*(2*b+a)/(b+a)

# Vertical wind.
(verticalPressure0,verticalPressure1)= IAP_wind.getVerticalPressureDistribution(terrainCategory= tC, x0= -1.4, x1= 1.4, z= z, v_b= vb)
vp00= verticalPressure0(-1.4)
vp01= verticalPressure0(1.4)
vp10= verticalPressure1(-1.4)
vp11= verticalPressure1(1.4)

ratio2= math.sqrt((vp00+1.5234182239774472e3)**2+(vp01-0.304683644795489e3)**2+(vp10-1.5234182239774472e3)**2+(vp11+0.304683644795489e3)**2)

'''
print('a= ', a)
print('b= ', b)
print('A= ', A)
print('lever= ', lever, ' lever/h_eq= ', lever/h_eq)

print('Probability factor _prob(T='+str(T)+', k= '+str(k)+', n= '+str(n)+')= '+ str(c_prob))
print('Drag force coefficient: c_fx= '+ str(c_fx))
print('Horizontal wind.')
print('  wind load by square meter on bridge deck: f_wx= '+str(f_wx/1e3)+' kN/m2.')
print('  wind unit drag on the top of the deck: '+str(a/1e3)+' kN/m2.')
print('  wind unit drag on the bottom of the deck: '+str(b/1e3)+' kN/m2.')
print('  average wind pressure on the deck: '+str((a+b)/2e3)+' kN/m2.')
print('  hiding factor: '+str(eta))
print('  wind pressure on the top of the windward beam: '+str(p_vba/1e3)+' kN/m2.')
print('  wind pressure on the bottom of windward beam: '+str(p_vbb/1e3)+' kN/m2.')
print('  wind pressure on the top of the leeward beam: '+str(p_vsa/1e3)+' kN/m2.')
print('  wind pressure on the bottom of leeward beam: '+str(p_vsb/1e3)+' kN/m2.')
print('  ratio1= ', ratio1)
print('Vertical wind.')
print('  downwards wind pressure on the leftmost point of the deck: ', str(vp00/1e3)+' kN/m2.')
print('  downwards wind pressure on the rightmost point of the deck: ', str(vp01/1e3)+' kN/m2.')
print('  upwards wind pressure on the leftmost point of the deck: ', str(vp10/1e3)+' kN/m2.')
print('  upwards wind pressure on the rightmost point of the deck: ', str(vp11/1e3)+' kN/m2.')
print('  ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) and (abs(ratio2)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
