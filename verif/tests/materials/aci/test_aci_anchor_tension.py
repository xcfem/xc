# -*- coding: utf-8 -*-
''' Verification test according to ACI 349.2 R-07.
  Guide to the Concrete Capacity Design (CCD) Method—Embedment Design Examples
  Example A1. Single stud, tension only, no edge effects.'''

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
ksi2MPa= 6.89476




#    Data
## Applied factored external load
Nua= 8*units_utils.kipToN 
## Stud definition
stud= ACI_limit_state_checking.AnchorBolt(ca1= 12*units_utils.inchToMeter,ca2= 12*units_utils.inchToMeter,ha= 18*units_utils.inchToMeter, concrete= ACI_materials.c4000, steel= ACI_materials.A108, diam= 0.5*units_utils.inchToMeter, hef= 4.69*units_utils.inchToMeter, cast_in= True)
### where:
#### ca1: distance from the center of an anchor shaft to the edge of concrete in one direction. If shear is applied to anchor, ca1 is taken in the direction of the applied shear. If the tension is applied to the anchor, ca1 is the minimum edge distance.
#### ca2: distance from center of an anchor shaft to the edge of concrete in the direction orthogonal to ca1.
#### ha: thickness  of  member  in  which  an  anchor  is located, measured parallel to anchor axis.
#### concrete: concrete material.
#### steel: anchor steel.
#### diam: anchor diameter.
#### hef: effective embedment depth of anchor.
#### cast_in: true if cast-in anchor false for post-installed anchors.
 
# End data

# *Calculate the nominal strength of the anchor in tension (art. D.5.1)
Nsa= stud.getNominalSteelStrengthTension() 
Nsa_kips=Nsa/units_utils.kipToN
ratio1=abs((Nsa_kips-12.7627)/12.7627)

#Concrete breakout failure in tension (art. D.5.2.)
Nb= stud.getBasicConcreteBreakoutStrengthTension() # Basic concrete breakout strength
Nb_kips=Nb/units_utils.kipToN
ratio2=abs((Nb_kips-15.41702)/15.41702)
Ncb=stud.getConcrBreakoutStrengthTension()
Ncb_kips=Ncb/units_utils.kipToN
ratio3=abs((Ncb_kips-15.4170)/15.4170)

#Pullout strength of stud (article D.5.3)
Abearing=0.59*(units_utils.inchToMeter)**2
Npn=stud.getPulloutStrengthTension(Abearing)
Npn_kips=Npn/units_utils.kipToN
ratio4=abs((Npn_kips-18.88)/18.88)

#Check ductility
Ndd=stud.getStrengthDuctilityTension(Abearing,cracking=True)
Ndd_kips=Ndd/units_utils.kipToN
ratio5=abs((Ndd_kips-13.10446)/13.10446)
'''
if Ndd >= Nua:
    print('Ndd >= Nua -> ductility OK')
else:
    print('Ndd < Nua -> NO ductility')
'''

#Check design strength of stud in tension
Nnd=stud.getDesignStrengthTension(Abearing,ductility=True,loadCombAlt=True)
Nnd_kips=Nnd/units_utils.kipToN
ratio6=abs((Nnd_kips-10.21018)/10.21018)
'''
if Nnd >= Nua:
    print('Nnd >= Nua -> design strength checking OK')
else:
    print('Nnd < Nua -> change stud dimensions')
'''

    
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-5) and (ratio2<1e-5) and (ratio3<1e-5) and (ratio4<1e-5) and (ratio5<1e-6) and (ratio6<1e-6)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

