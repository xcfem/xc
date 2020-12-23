# -*- coding: utf-8 -*-
from __future__ import print_function
import rough_calculations.ng_punching_shear

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#CASO 1

qk=2e3      #N/m2
A=5*5     #m2

Vd= rough_calculations.ng_punching_shear.esfuerzoPunzonamiento(qk,A)
#Da como resultado Vd=80 kN

fck=25e6
d=0.20
a=0.25
b=0.25

Vdmax= rough_calculations.ng_punching_shear.punzMaximo(fck,d,a,b)
#Da como resultado Vdmax=387 kN

h=0.25
fyd=500e6/1.15

As= rough_calculations.ng_punching_shear.reinforcementPunz(Vd,fck,d,a,b,h,fyd)
#Da como resultado Vd<Vcu -> No necesita reinforcement de punzonamiento


ratio1= (Vd-80e3)/80e3
ratio2= (Vdmax-387.298334621e3)/387.298334621e3

#print("Vd= ",Vd/1e3," kN")
#print("Vdmax= ",Vdmax/1e3," kN")
#print("As= ", As," cm2/m")
#print("ratio1= ",ratio1)
#print("ratio2= ",ratio2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5 and abs(As)<1e-15:
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')

