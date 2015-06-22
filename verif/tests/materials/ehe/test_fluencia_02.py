# -*- coding: utf-8 -*-
''' Test comprobación del cálculo de la deformación por fluencia
   según EHE-08. '''

from materials.ehe import auxEHE
from materials.ehe import retraccion_fluencia


fck= 30e6
t0= 28
t= 10000
HR= 0.8
u= 2
A= .25
sigma= -10e6
Ec28= auxEHE.getEcm(fck)

fi1= retraccion_fluencia.getPhiFluencia(fck,t,t0,HR,u,A)
epsElastica= sigma/Ec28
epsFluencia= retraccion_fluencia.getDeformacionFluencia(fck,t0,0.25,fi1,sigma)

ratio1= abs(epsFluencia+9.31793e-04)/9.31793e-04

''' 
print "fi1= ",fi1
print "epsElastica= ",epsElastica
print "epsFluencia= ",epsFluencia
print "ratio1= ",(ratio1)
   '''

import os
fname= os.path.basename(__file__)
if (ratio1<1e-7):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
