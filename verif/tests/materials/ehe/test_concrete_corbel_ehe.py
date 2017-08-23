# -*- coding: utf-8 -*-

#Design of concrete corbel according to EHE.
#Home made test.

from __future__ import division
import sys

from materials.ehe import mensula_cortaEHE08

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

Fvd=1000e3 #Vertical load on the corbel, positive downwards (N).
Fhd=100e3 #Horizontal load on the corbel, positive outwards (N).
tipoJunta="monolitica" #Joint type according to 64.1.2.1.
a=0.6 #Distance (m) from the applied load axis and the plane of the corbel fixation (see figure 64.1.2 on EHE-08).

fck=35E6 #Characteristic value of concrete strength (Pa).
fyk=500E6 #Characteristic value of steel strength (Pa).

# Results
dmin=mensula_cortaEHE08.getCantoUtilMinimo(tipoJunta,a)
dminTeor=0.988235294118
ratio1=abs(dmin-dminTeor)/dminTeor
T1d=mensula_cortaEHE08.getTraccionMainReinforcement(tipoJunta,Fvd,Fhd)
T1dTeor=814285.65
ratio2=abs(T1d-T1dTeor)/T1dTeor
As1=mensula_cortaEHE08.getAreaNecMainReinforcement(tipoJunta,Fvd,Fhd,fyk/1.15)
As1Teor=T1dTeor/400e6
ratio3=abs(As1-As1Teor)/As1Teor
Ase=mensula_cortaEHE08.getAreaNecCercos(Fvd,fyk/1.15)
AseTeor=0.2*Fvd/400e6
ratio4=abs(Ase-AseTeor)/AseTeor
Aplaca=mensula_cortaEHE08.getAreaNecApoyo(Fvd,fck/1.5)
AplacaTeor=Fvd/0.7/fck*1.5
ratio5=abs(Aplaca-AplacaTeor)/AplacaTeor


##print "dmin= ",dmin," m"
##print "ratio1= ",ratio1
##print "T1d= ",T1d/1e3," kN"
##print "ratio2= ",ratio2
##print "As1= ",As1*1e4," cm2"
##print "ratio3= ",ratio3
##print "Ase= ",Ase*1e4," cm2"
##print "ratio4= ",ratio4
##print "Aplaca= ",Aplaca," m2"
##print "ratio5= ",ratio5
  

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12) and (ratio2<1e-7) and (ratio3<1e-7) and (ratio4<1e-15) and (ratio5<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

  
