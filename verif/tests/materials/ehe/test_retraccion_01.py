# -*- coding: utf-8 -*-
''' Test comprobación del cálculo de la deformación por retraccion
   según EHE-08. '''
import xc_base
import geom
import xc
from materials.ehe import auxEHE
from materials.ehe import retraccion_fluencia
import math

tS= 7
velCemento= "normal"

# Comprobamos con los valores de la tabla 39.7.c de la norma.
err2= 0.0
epsR1= retraccion_fluencia.getDeformacionRetraccion(30e6,14,tS,0.5,2,50e-3,velCemento)
err2= err2+(epsR1+186e-6)**2
epsR2= retraccion_fluencia.getDeformacionRetraccion(30e6,10000,tS,0.5,2,50e-3,velCemento)
err2= err2+(epsR2+532e-6)**2
epsR3= retraccion_fluencia.getDeformacionRetraccion(30e6,14,tS,0.9,2,600e-3,velCemento)
err2= err2+(epsR3+28e-6)**2
epsR4= retraccion_fluencia.getDeformacionRetraccion(30e6,10000,tS,0.9,2,600e-3,velCemento)
err2= err2+(epsR4+149e-6)**2

# Comprobamos con los valores de la tabla 39.7.d de la norma.
epsR1d= retraccion_fluencia.getDeformacionRetraccion(70e6,14,tS,0.5,2,50e-3,velCemento)
err2= err2+(epsR1d+178e-6)**2
epsR2d= retraccion_fluencia.getDeformacionRetraccion(70e6,10000,tS,0.5,2,50e-3,velCemento)
err2= err2+(epsR2d+448e-6)**2
epsR3d= retraccion_fluencia.getDeformacionRetraccion(70e6,14,tS,0.9,2,600e-3,velCemento)
err2= err2+(epsR3d+80e-6)**2
epsR4d= retraccion_fluencia.getDeformacionRetraccion(70e6,10000,tS,0.9,2,600e-3,velCemento)
err2= err2+(epsR4d+211e-6)**2

ratio1= math.sqrt(err2)

''' 
print "epsR1= ",epsR1*1e6,"x10^(-6)\n"
print "epsR2= ",epsR2*1e6,"x10^(-6)\n"
print "epsR3= ",epsR3*1e6,"x10^(-6)\n"
print "epsR4= ",epsR4*1e6,"x10^(-6)\n"
print "epsR1d= ",epsR1d*1e6,"x10^(-6)\n"
print "epsR2d= ",epsR2d*1e6,"x10^(-6)\n"
print "epsR3d= ",epsR3d*1e6,"x10^(-6)\n"
print "epsR4d= ",epsR4d*1e6,"x10^(-6)\n"
print "ratio1= ",(ratio1)
   '''

import os
fname= os.path.basename(__file__)
if (ratio1<8e-7):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
