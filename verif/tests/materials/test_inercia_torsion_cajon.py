# -*- coding: utf-8 -*-
''' Home made test a partir del ejemplo expuesto
en la página C-17 del "Bridge Strudl Manual" de noviembre de 1973 '''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

bs= 26.51
bi= 21.05
h= 5.48
ts= .56
ti= .48
td= 1

from materials import parametrosSeccionCajonMonocelular

J= parametrosSeccionCajonMonocelular.getInerciaTorsionCajonMonocelular(bs,bi,h,ts,ti,td)
ratio1= abs(J-656.6)/656.6

''' 
print "J= ",J
print "ratio1= ",ratio1
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<2e-4):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
