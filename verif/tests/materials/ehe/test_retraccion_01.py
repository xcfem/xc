# -*- coding: utf-8 -*-
''' Test comprobación del cálculo de la deformación por retraccion
   según EHE-08. '''
import xc_base
import geom
import xc
from materials.ehe import EHE_concrete
import math

tS= 7
velCemento= "normal"
concrHA30=EHE_concrete.HA30
concrHA30.cemType='N'
concrHA70=EHE_concrete.HA70
concrHA70.cemType='N'

# Comprobamos con los valores de la tabla 39.7.c de la norma.
err2= 0.0
epsR1= concrHA30.getShrEpscs(14,tS,50,50)
err2= err2+(epsR1+186e-6)**2
epsR2= concrHA30.getShrEpscs(10000,tS,50,50)
err2= err2+(epsR2+532e-6)**2
epsR3= concrHA30.getShrEpscs(14,tS,90,600)
err2= err2+(epsR3+28e-6)**2
epsR4= concrHA30.getShrEpscs(10000,tS,90,600)
err2= err2+(epsR4+149e-6)**2

# Comprobamos con los valores de la tabla 39.7.d de la norma.
epsR1d=concrHA70.getShrEpscs(14,tS,50,50)
err2= err2+(epsR1d+178e-6)**2
epsR2d=concrHA70.getShrEpscs(10000,tS,50,50)
err2= err2+(epsR2d+448e-6)**2
epsR3d=concrHA70.getShrEpscs(14,tS,90,600)
err2= err2+(epsR3d+80e-6)**2
epsR4d=concrHA70.getShrEpscs(10000,tS,90,600)
err2= err2+(epsR4d+211e-6)**2

ratio1= math.sqrt(err2)

#'' 
print "epsR1= ",epsR1*1e6,"x10^(-6)\n"
print "epsR2= ",epsR2*1e6,"x10^(-6)\n"
print "epsR3= ",epsR3*1e6,"x10^(-6)\n"
print "epsR4= ",epsR4*1e6,"x10^(-6)\n"
print "epsR1d= ",epsR1d*1e6,"x10^(-6)\n"
print "epsR2d= ",epsR2d*1e6,"x10^(-6)\n"
print "epsR3d= ",epsR3d*1e6,"x10^(-6)\n"
print "epsR4d= ",epsR4d*1e6,"x10^(-6)\n"
print "ratio1= ",(ratio1)
#  '''

import os
fname= os.path.basename(__file__)
if (ratio1<8e-7):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
