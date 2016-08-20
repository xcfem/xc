# -*- coding: utf-8 -*-
import rough_calculations.ng_punzonamiento


#CASO 2

qk=7e3    #N/m2
A=6*6     #m2

Vd=rough_calculations.ng_punzonamiento.esfuerzoPunzonamiento(qk,A)
#Da como resultado Vd=403 kN

fck=25e6
d=0.15
a=0.25
b=0.25

Vdmax=rough_calculations.ng_punzonamiento.punzMaximo(fck,d,a,b)
#Da como resultado Vdmax=290 kN

h=0.20
fyd=500e6/1.15

As=rough_calculations.ng_punzonamiento.reinforcementPunz(Vd,fck,d,a,b,h,fyd)
#Da como resultado As=39 cm2

ratio1= (Vd-403.2e3)/403.2e3
ratio2= (Vdmax-290.473750966e3)/290.473750966e3
ratio3= (As-38.7938540862e-4)/38.7938540862e-4

#print "Vd= ",Vd/1e3," kN"
#print "Vdmax= ",Vdmax/1e3," kN"
#print "As= ", As*1e4," cm2/m"
#print "ratio1= ",ratio1
#print "ratio2= ",ratio2
#print "ratio3= ",ratio3

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5 and abs(ratio3)<1e-5:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

