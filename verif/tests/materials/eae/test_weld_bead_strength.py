# -*- coding: utf-8 -*-
from __future__ import print_function
'''Computation of the strength of a weld bead from the example on page 57
of the monograph «Estudio de los diferentes métodos de dimensionamiento para uniones atornilladas y soldadas de acuerdo con la normativa vigente» de Marc 
Masnou Malgosa.(http://hdl.handle.net/2099.1/6080)'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.eae import EAE_limit_state_checking
import math

# Stresses on the throat plane revolved over the bead face.
tn= 72.07e6 # Tangential stress normal to weld axis.
ta= -33.22e6 # Tangential stress parallel to weld axis.
n= -252.4e6 # Normal stress.

# Stresses applied to the throat area of the weld.
sigmaN= EAE_limit_state_checking.getSigmaNThroatPlane(n,tn) 
tauN= EAE_limit_state_checking.getTauNThroatPlane(n,tn)
tauPll= ta 

# Equivalent stress.
sigmaCo= EAE_limit_state_checking.getFilletWeldYieldCriteriaLHS(n,tn,ta)
sigmaUlt= EAE_limit_state_checking.getFilletWeldYieldCriteriaRHSValue(430e6,275e6,1.25)
fcCond1= EAE_limit_state_checking.getFilletWeldCondition1CapacityFactor(n,tn,ta,430e6,275e6,1.25)
sigmaNUlt= EAE_limit_state_checking.getFilletWeldUltimateNormalStress(430e6,1.25)
fcCond2= EAE_limit_state_checking.getFilletWeldCondition2CapacityFactor(n,tn,430e6,1.25)

ratio1= ((sigmaN+229.44e6)/229.44e6)
ratio2= ((tauN+127.54e6)/127.54e6)
ratio3= ((sigmaCo-323.65e6)/323.65e6)
ratio4= ((sigmaUlt-404.706e6)/404.706e6)
ratio5= (fcCond1-sigmaCo/sigmaUlt)
ratio6= (fcCond2+sigmaN/sigmaNUlt)


'''
print("sigmaN= ",sigmaN)
print("tauN= ",tauN)
print("tauPll= ",tauPll)
print("sigmaCo= ",sigmaCo)
print("sigmaUlt= ",sigmaUlt)
print("fcCond1= ",fcCond1)
print("sigmaNUlt= ",sigmaNUlt)
print("fcCond2= ",fcCond2)

print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio3= ",ratio4)
print("ratio5= ",ratio5)
print("ratio6= ",ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.001) & (abs(ratio2)<0.001) & (abs(ratio3)<0.001) & (abs(ratio4)<0.001) & (abs(ratio5)<0.001) & (abs(ratio6)<0.001):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

