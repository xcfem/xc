# -*- coding: utf-8 -*-

''' Computation of Vsu value. The result is compared to that obtained
from the exercise ER-CA-02 from www.areadecalculo.com. According to EHE-08.
'''

import sys
from materials.ehe import EHE_limit_state_checking
from materials.ehe import EHE_materials
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

gammas= 1.15 #Partial safety factor for steel.

numBarrasEstribo= 4
areaShReinfBranchsEstribo= numBarrasEstribo*EHE_materials.Fi6
sepEstribos= 0.2
AsTrsv= areaShReinfBranchsEstribo/sepEstribos
z= 0.9*0.55
alpha= math.pi/2
theta= math.pi/4
fyd= 500e6/gammas
Vsu= EHE_limit_state_checking.getVsuEHE08(z,alpha,theta,AsTrsv,fyd)
ratio1= abs(Vsu-111.966e3)/111.966e3

##print "areaShReinfBranchsEstribo= ",AsTrsv*1e4," cm2"
##print "Vsu= ",Vsu/1e3," kN"
##print "ratio1= ",ratio1," kN"
  

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio1<0.01:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
  
