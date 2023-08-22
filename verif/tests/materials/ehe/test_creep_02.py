# -*- coding: utf-8 -*-
from __future__ import print_function
''' Creep strain computation test according to EHE-08 and EC2. '''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ehe import EHE_materials
from materials.ec2 import EC2_materials


t0= 28
t= 10000
RH= 80
u= 2
A= .25
h0= 2*A/u
sigma= -10e6
Ec28EHE= EHE_materials.HA30.getEcm()


fi1= EHE_materials.HA30.getCreepFitt0(t,t0,RH,2*A/u)
epsInstant= sigma/Ec28EHE
epsCreepEHE= EHE_materials.HA30.getCreepDeformation(t,t0,RH,h0,sigma)
epsCreepEC2= EC2_materials.C30.getCreepDeformation(t,t0,RH,h0,sigma)



# Comparing value of epsCreep is changed from 9.31793e-04 to 9.319563e-04
# after detecting an error in the formula used to obtain betaH
ratio1= abs(epsCreepEHE+5.8202195e-4)/5.8202195e-4
ratio2=abs(epsCreepEC2+5.0651821e-4)/5.0651821e-4

''' 
print("fi1= ",fi1)
print("epsInstant= ",epsInstant)
print("epsCreepEHE= ",epsCreepEHE)
print("epsCreepEC2= ",epsCreepEC2)
print("ratio1= ",(ratio1))
 print("ratio2= ",(ratio2))
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-7) and (ratio2<1e-7):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
