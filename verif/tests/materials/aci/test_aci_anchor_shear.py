# -*- coding: utf-8 -*-
# Verification test according to ACI 349.2 R-07.
# -*- coding: utf-8 -*-
'''Guide to the Concrete Capacity Design (CCD) Method—Embedment Design Examples
Example A2. Single stud, shear only.'''

from __future__ import division
from __future__ import print_function

import math
from materials.aci import ACI_materials
from materials.aci import ACI_limit_state_checking
from misc_utils import units_utils

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega.ort@gmail.com"

#units
ksi2MPa= units_utils.ksiToPa/1e6

#    Data
## Applied factored external load
Vua=6*units_utils.kipToN
## Stud definition
stud= ACI_limit_state_checking.AnchorBolt(ca1= 10*units_utils.inchToMeter,ca2= 18*units_utils.inchToMeter,ha= 18*units_utils.inchToMeter, concrete= ACI_materials.c4000, steel= ACI_materials.A108, diam= 0.5*units_utils.inchToMeter, hef= 3.06*units_utils.inchToMeter, cast_in= True)
### where:
#### ca1: distance from the center of an anchor shaft to the edge of concrete in one direction. If shear is applied to anchor, ca1 is taken in the direction of the applied shear. If the tension is applied to the anchor, ca1 is the minimum edge distance.
### ca2: distance from center of an anchor shaft to the edge of concrete in the direction orthogonal to ca1.
### ha: thickness  of  member  in  which  an  anchor  is located, measured parallel to anchor axis.
### concrete: concrete material.
### steel: anchor steel.
### diam: anchor diameter.
### hef: effective embedment depth of anchor.
### cast_in: true if cast-in anchor false for post-installed anchors.
 
# End data

#Calculate the nominal strength of the anchor in shear  (art. D.6.1.)
Vsa=stud.getSteelStrengthShear()
Vsa_kips=Vsa/units_utils.kipToN
ratio1=abs((Vsa_kips-12.7627)/12.7627)

#Concrete breakout failure in shear (art. D.6.2.)
Vcb=stud.getConcrBreakoutStrengthShear()
Vcb_kips=Vcb/units_utils.kipToN
ratio2=abs((Vcb_kips-16.2538)/16.2538)
    
#Concrete pryout strength of anchor in shear (article D.6.3.)
Vcp=stud.getPryoutStrengthShear()
Vcp_kips=Vcp/units_utils.kipToN
ratio3=abs((Vcp_kips-16.25)/16.25)
 
#Pullout stregth of stud to check head of the stud (article D.5.3)
Abearing=0.589*(units_utils.inchToMeter)**2
Npn=stud.getPulloutStrengthTension(Abearing)
Npn_kips=Npn/units_utils.kipToN
ratio4=abs((Npn_kips-18.848)/18.848)

#Check ductility
Vdd=stud.getStrengthDuctilityShear()
Vdd_kips=Vdd/units_utils.kipToN
ratio5=abs((Vdd_kips-13.8125)/13.8125)
'''
if Vdd >= Vua:
    print('Vdd >= Vua -> ductility OK')
else:
    print('Vdd < Vua -> NO ductility')
'''

# Check design strength of stud in shear
Vnd=stud.getDesignStrengthShear(ductility=True,loadCombAlt=True)
Vnd_kips=Vnd/units_utils.kipToN
ratio6=abs((Vnd_kips-9.57204)/9.57204)
'''
if Vnd >= Vua:
    print('Vnd >= Vua -> design strength checking OK')
else:
    print('Vnd < Vua -> change stud dimensions')
'''



import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-5) and (ratio2<1e-5) and (ratio3<1e-5) and (ratio4<1e-5) and (ratio5<1e-6) and (ratio6<1e-6)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

