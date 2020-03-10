# -*- coding: utf-8 -*-
# Verification test according to ACI 349.2 R-07.
# Guide to the Concrete Capacity Design (CCD) Method—Embedment Design Examples
# Example A3. Single stud, combined tension and shear.

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
ft2m= 0.3048
kip2N= 4.4482216e3
ksi2MPa= 6.89476
lb2N=4.4482216
N2lb=1/lb2N
N2kip=1/kip2N

# Data
Nua=8*kip2N  #applied factored external tension load using load factors from Appendix C of the Code.
Vua=6*kip2N  #applied factored external shear load using load factors from Appendix C of the Code.
## Stud definition
stud= ACI_limit_state_checking.AnchorBolt(ca1= 12*in2m,ca2= 20*in2m,ha= 18*in2m, concrete= ACI_materials.c4000, steel= ACI_materials.A108, diam= 5/8*in2m, hef= 6.63*in2m, cast_in= True)
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
#TENSION
#Calculate the nominal strength of the anchor in tension (art. D.5.1)
Nsa=stud.getNominalSteelStrengthTension()
Nsa_kips=Nsa/kip2N
ratio1=abs((Nsa_kips-19.94176)/19.94176)

#Concrete breakout failure in tension (art. D.5.2.)
Nb= stud.getBasicConcreteBreakoutStrengthTension() # Basic concrete breakout strength
Nb_kips=Nb*N2kip
Ncb=stud.getConcrBreakoutStrengthTension()
Ncb_kips=Ncb*N2kip
ratio2=abs((Ncb_kips-25.9126)/25.9126)

#Pullout stregth of stud to check head of the stud (article D.5.3)
Abearing=0.92*(in2m)**2
Npn=stud.getPulloutStrengthTension(Abearing)
Npn_kips=Npn/kip2N
ratio3=abs((Npn_kips-29.44)/29.44)

#Check ductility in tension
Ndd=stud.getStrengthDuctilityTension(Abearing,cracking=True)
Ndd_kips=Ndd/kip2N
ratio4=abs((Ndd_kips-22.025729)/22.025729)

'''
if Ndd >= Nua:
    print ('Ndd >= Nua -> ductility OK')
else:
    print ('Ndd < Nua -> NO ductility') 
'''
#Check design strength of stud in tension
Nnd=stud.getDesignStrengthTension(Abearing,ductility=True,loadCombAlt=True)
Nnd_kips=Nnd/kip2N
ratio5=abs((Nnd_kips-15.9534)/15.9534)

#SHEAR
#Calculate the nominal strength of the anchor in shear
Vsa=stud.getSteelStrengthShear()
Vsa_kips=Vsa/kip2N
ratio6=abs((Vsa_kips-19.94176)/19.94176)

#Concrete breakout failure in shear (art. D.6.2.)
Vcb=stud.getConcrBreakoutStrengthShear()
Vcb_kips=Vcb/kip2N
ratio7=abs((Vcb_kips-25.20286)/25.20286)
    
#Concrete pryout strength of anchor in shear (article D.6.3.)
Vcp=stud.getPryoutStrengthShear()
Vcp_kips=Vcp/kip2N
ratio8=abs((Vcp_kips-51.82524)/51.82524)
    
#Check ductility in shear
Vdd=stud.getStrengthDuctilityShear()
Vdd_kips=Vdd/kip2N
ratio9=abs((Vdd_kips-21.42243)/21.42243)
'''
if Vdd >= Vua:
    print ('Vdd >= Vua -> ductility OK')
else:
    print ('Vdd < Vua -> NO ductility') 
'''

# Check design strength of stud in shear
Vnd=stud.getDesignStrengthShear(ductility=True,loadCombAlt=True)
Vnd_kips=Vnd/kip2N
ratio10=abs((Vnd_kips-14.9563)/14.9563)
'''
if Vnd >= Vua:
    print ('Vnd >= Vua -> design strength checking OK')
else:
    print ('Vnd < Vua -> change stud dimensions') 
'''
#Art. D.7
'''
if Vua <= 0.2*Vnd:
    print ('Vua <= 0.2*Vnd -> full strength in tension is permitted')
if Nua <= 0.2*Nnd:
    print ('Nua <= 0.2*Nnd -> full strength in shear is permitted')
if Vua > 0.2*Vnd and Nua > 0.2*Nnd:
    if Nua/Nnd + Vua/Vnd <= 1.2:
        print ('Nua/Nnd + Vua/Vnd = ',round(Nua/Nnd + Vua/Vnd,2), ' <= 1.2')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-5) and (ratio2<1e-5) and (ratio3<1e-5) and (ratio4<1e-5) and (ratio5<1e-5) and (ratio6<1e-5) and (ratio7<1e-5) and (ratio8<1e-5) and (ratio9<1e-5) and (ratio10<1e-5)):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
