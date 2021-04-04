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

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega.ort@gmail.com"

#units
in2m= 0.0254
m2in=1/in2m
feet2meter= 0.3048
kip2N= 4.4482216e3
ksi2MPa= 6.89476
lb2N=4.4482216
N2lb=1/lb2N

#    Data
## Applied factored external load
Vua=6*kip2N
## Stud definition
stud= ACI_limit_state_checking.AnchorBolt(ca1= 10*in2m,ca2= 18*in2m,ha= 18*in2m, concrete= ACI_materials.c4000, steel= ACI_materials.A108, diam= 0.5*in2m, hef= 3.06*in2m, cast_in= True)
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
Vsa_kips=Vsa/kip2N
ratio1=abs((Vsa_kips-12.7627)/12.7627)

#Concrete breakout failure in shear (art. D.6.2.)
Vcb=stud.getConcrBreakoutStrengthShear()
Vcb_kips=Vcb/kip2N
ratio2=abs((Vcb_kips-16.2538)/16.2538)
    
#Concrete pryout strength of anchor in shear (article D.6.3.)
Vcp=stud.getPryoutStrengthShear()
Vcp_kips=Vcp/kip2N
ratio3=abs((Vcp_kips-16.25)/16.25)
 
#Pullout stregth of stud to check head of the stud (article D.5.3)
Abearing=0.589*(in2m)**2
Npn=stud.getPulloutStrengthTension(Abearing)
Npn_kips=Npn/kip2N
ratio4=abs((Npn_kips-18.848)/18.848)

#Check ductility
Vdd=stud.getStrengthDuctilityShear()
Vdd_kips=Vdd/kip2N
ratio5=abs((Vdd_kips-13.8125)/13.8125)
'''
if Vdd >= Vua:
    print('Vdd >= Vua -> ductility OK')
else:
    print('Vdd < Vua -> NO ductility')
'''

# Check design strength of stud in shear
Vnd=stud.getDesignStrengthShear(ductility=True,loadCombAlt=True)
Vnd_kips=Vnd/kip2N
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

