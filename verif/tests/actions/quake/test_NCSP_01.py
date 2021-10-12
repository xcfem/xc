# -*- coding: utf-8 -*-
'''Verification of NCSP-07 routines.
   Home made test.
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.quake import NCSE
from actions.quake import NCSP

C= (2.0*5.3+1.3*24.7)/30
g= 9.81
pga= .05*g
rho= 1.0
K= 1.0

dampingIndex= 5/100.0 # 5%

soilAmplificationFactor= NCSE.getSoilAmplificationFactorPGA(C, pga, rho)
ac= 1.0

ulsHorizontalResponseSpectrum= NCSP.HorizontalElasticResponseSpectrum(zeta= dampingIndex, ac= ac, K= 1.0, C= C, uls= True)
Ta= ulsHorizontalResponseSpectrum.Ta
Tb= ulsHorizontalResponseSpectrum.Tb
Tc= ulsHorizontalResponseSpectrum.Tc

STa= ulsHorizontalResponseSpectrum.eval(Ta)
ratio1= (STa-2.5)/2.5
STb= ulsHorizontalResponseSpectrum.eval(Tb)
ratio2= (STb-2.5)/2.5
STc= ulsHorizontalResponseSpectrum.eval(Tc)
STcRef= 2.5*ulsHorizontalResponseSpectrum.nu*Tb*Tc/Tc**2
ratio3= (STc-STcRef)/STcRef

'''
print('STa= '+str(STa)+' m/s2')
print('STb= '+str(STb)+' m/s2')
print('STc= '+str(STc)+' m/s2')
'''

#ulsHorizontalResponseSpectrum.plot()
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-6 and abs(ratio2)<1e-6 and abs(ratio3)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
