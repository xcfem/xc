# -*- coding: utf-8 -*-
from __future__ import print_function
''' Verification test for the concrete shrinkage computation
   according to EHE-08. '''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ehe import EHE_materials
import math

tS= 7
velCemento= "normal"
concrHA30= EHE_materials.HA30
concrHA30.cemType='N'
concrHA70= EHE_materials.HA70
concrHA70.cemType='N'

# Comprobamos con los valores de la tabla 39.7.c de la norma.
err2= 0.0
epsR1= concrHA30.getShrEpscs(14,tS,50,50e-3)
err2= err2+(epsR1+186e-6)**2
epsR2= concrHA30.getShrEpscs(10000,tS,50,50e-3)
err2= err2+(epsR2+532e-6)**2
epsR3= concrHA30.getShrEpscs(14,tS,90,600e-3)
err2= err2+(epsR3+28e-6)**2
epsR4= concrHA30.getShrEpscs(10000,tS,90,600e-3)
err2= err2+(epsR4+149e-6)**2

# Comprobamos con los valores de la tabla 39.7.d de la norma.
epsR1d=concrHA70.getShrEpscs(14,tS,50,50e-3)
err2= err2+(epsR1d+178e-6)**2
epsR2d=concrHA70.getShrEpscs(10000,tS,50,50e-3)
err2= err2+(epsR2d+448e-6)**2
epsR3d=concrHA70.getShrEpscs(14,tS,90,600e-3)
err2= err2+(epsR3d+80e-6)**2
epsR4d=concrHA70.getShrEpscs(10000,tS,90,600e-3)
err2= err2+(epsR4d+211e-6)**2

ratio1= math.sqrt(err2)

''' 
print("epsR1= ",epsR1*1e6,"x10^(-6)\n")
print("epsR2= ",epsR2*1e6,"x10^(-6)\n")
print("epsR3= ",epsR3*1e6,"x10^(-6)\n")
print("epsR4= ",epsR4*1e6,"x10^(-6)\n")
print("epsR1d= ",epsR1d*1e6,"x10^(-6)\n")
print("epsR2d= ",epsR2d*1e6,"x10^(-6)\n")
print("epsR3d= ",epsR3d*1e6,"x10^(-6)\n")
print("epsR4d= ",epsR4d*1e6,"x10^(-6)\n")
print("ratio1= ",(ratio1))
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<8e-7):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
