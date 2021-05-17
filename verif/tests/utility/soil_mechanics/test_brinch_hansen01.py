# -*- coding: utf-8 -*-

'''
   Verification of the Brinch Hansen formula.

   See "Guía de cimentaciones en obras de carretera. Ministerio de Fomento
   (Spain) 2002 Table on page 101.
   (https://books.google.ch/books?id=a0eoygAACAAJ)
'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from geotechnics import frictional_cohesional_soil as fcs
import math

#Values in table (Phi,Nq,Nc,Ngamma)
tableValues= [[20,6.4,14.8,3.9],
[21.0,7.1,15.8,4.7],
[22.0,7.8,16.9,5.5],
[23.0,8.7,18.1,6.5],
[24.0,9.6,19.3,7.7],
[25.0,10.7,20.7,9.0],
[26.0,11.8,22.3,10.6],
[27.0,13.2,23.9,12.4],
[28.0,14.7,25.8,14.6],
[29.0,16.4,27.9,17.1],
[30.0,18.4,30.1,20.1],
[31.0,20.6,32.7,23.6],
[32.0,23.2,35.5,27.7],
[33.0,26.1,38.6,32.6],
[34.0,29.4,42.2,38.4],
[35.0,33.3,46.1,45.2],
[36.0,37.8,50.6,53.4],
[37.0,42.9,55.6,63.2],
[38.0,48.9,61.4,74.9],
[39.0,56.0,67.9,89.0],
[40.0,64.2,75.3,106.1],
[41.0,73.9,83.9,126.7],
[42.0,85.4,93.7,151.9],
[43.0,99.0,105.1,182.8],
[44.0,115.3,118.4,220.8],
[45.0,134.9,133.9,267.7]]

err= 0.0
for values in tableValues:
  phiTable= math.radians(values[0])
  soil= fcs.FrictionalCohesionalSoil(phiTable,0.0,0.0)
  errNq= (values[1]-soil.Nq())/values[1]
  errNc= (values[2]-soil.Nc())/values[2]
  errNgamma= (values[3]-soil.Ngamma(2.0))/values[3]
  err+= errNq**2+errNc**2+errNgamma**2

err= math.sqrt(err)

#print('err= ', err)

import os
fname= os.path.basename(__file__)
if (err<0.05):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
