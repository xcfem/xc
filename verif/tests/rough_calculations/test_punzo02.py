# -*- coding: utf-8 -*-
from __future__ import print_function
import rough_calculations.ng_punching_shear

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#CASO 2

qk=7e3    #N/m2
A=6*6     #m2

Vd=rough_calculations.ng_punching_shear.esfuerzoPunzonamiento(qk,A)
#Da como resultado Vd=403 kN

fck=25e6
d=0.15
a=0.25
b=0.25

Vdmax=rough_calculations.ng_punching_shear.punzMaximo(fck,d,a,b)
#Da como resultado Vdmax=290 kN

h=0.20
fyd=500e6/1.15

As=rough_calculations.ng_punching_shear.reinforcementPunz(Vd,fck,d,a,b,h,fyd)
#Da como resultado As=39 cm2

ratio1= (Vd-403.2e3)/403.2e3
ratio2= (Vdmax-290.473750966e3)/290.473750966e3
ratio3= (As-38.7938540862e-4)/38.7938540862e-4

#print("Vd= ",Vd/1e3," kN")
#print("Vdmax= ",Vdmax/1e3," kN")
#print("As= ", As*1e4," cm2/m")
#print("ratio1= ",ratio1)
#print("ratio2= ",ratio2)
#print("ratio3= ",ratio3)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5 and abs(ratio3)<1e-5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

