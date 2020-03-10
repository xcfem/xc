# -*- coding: utf-8 -*-

'''Design of concrete corbel according to EHE. Home made test.'''

from __future__ import division
import sys
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

Fvd=1000e3 #Vertical load on the corbel, positive downwards (N).
Fhd=100e3 #Horizontal load on the corbel, positive outwards (N).

concreteCorbel= EHE_limit_state_checking.ConcreteCorbel(EHE_materials.HA35,EHE_materials.B500S,'monolitica') #Joint type according to clause 64.1.2.1.

a=0.6 #Distance (m) from the applied load axis and the plane of the corbel fixation (see figure 64.1.2 on EHE-08).

fck=35E6 #Characteristic value of concrete strength (Pa).
fyk=500E6 #Characteristic value of steel strength (Pa).

# Results
dmin= concreteCorbel.getMinimumEffectiveDepth(a)
dminTeor= 0.988235294118
ratio1= abs(dmin-dminTeor)/dminTeor
T1d= concreteCorbel.getMainReinforcementTension(Fvd,Fhd)
T1dTeor= 814285.65
ratio2= abs(T1d-T1dTeor)/T1dTeor
As1= concreteCorbel.getAreaNecMainReinforcement(Fvd,Fhd)
As1Teor= T1dTeor/400e6
ratio3= abs(As1-As1Teor)/As1Teor
Ase= concreteCorbel.getAreaNecCercos(Fvd)
AseTeor= 0.2*Fvd/400e6
ratio4= abs(Ase-AseTeor)/AseTeor
Aplaca= concreteCorbel.getAreaNecApoyo(Fvd)
AplacaTeor= Fvd/0.7/fck*1.5
ratio5= abs(Aplaca-AplacaTeor)/AplacaTeor

'''
print "dmin= ",dmin," m"
print "ratio1= ",ratio1
print "T1d= ",T1d/1e3," kN"
print "ratio2= ",ratio2
print "As1= ",As1*1e4," cm2"
print "ratio3= ",ratio3
print "Ase= ",Ase*1e4," cm2"
print "ratio4= ",ratio4
print "Aplaca= ",Aplaca," m2"
print "ratio5= ",ratio5
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12) and (ratio2<1e-7) and (ratio3<1e-7) and (ratio4<1e-15) and (ratio5<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

  
