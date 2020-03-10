# -*- coding: utf-8 -*-
''' Creep strain computation test according to EHE-08. '''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ehe import EHE_materials


t0= 28
t= 10000
RH= 80
u= 2e3
A= .25e6
h0mm=2*A/u
sigma= -10e6
Ec28= EHE_materials.HA30.getEcm()

fi1= EHE_materials.HA30.getCreepFitt0(t,t0,RH,2*A/u)
epsElastica= sigma/Ec28
epsFluencia= EHE_materials.HA30.getCreepDeformation(t,t0,RH,h0mm,sigma)

# Comparing value of epsFluencia is changed from 9.31793e-04 to 9.319563e-04
# after detecting an error in the formula used to obtain betaH
ratio1= abs(epsFluencia+9.319563e-04)/9.319563e-04

''' 
print "fi1= ",fi1
print "epsElastica= ",epsElastica
print "epsFluencia= ",epsFluencia
print "ratio1= ",(ratio1)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-7):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
