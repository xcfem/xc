# -*- coding: utf-8 -*-
''' Verification test according to ACI 349.2 R-07.
    Guide to the Concrete Capacity Design (CCD) Method—Embedment Design Examples
    Example A3. Single stud, combined tension and shear.'''

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


# Data
Nua=8*units_utils.kipToN  #applied factored external tension load using load factors from Appendix C of the Code.
Vua=6*units_utils.kipToN  #applied factored external shear load using load factors from Appendix C of the Code.
## Stud definition
stud= ACI_limit_state_checking.AnchorBolt(ca1= 12*units_utils.inchToMeter,ca2= 20*units_utils.inchToMeter,ha= 18*units_utils.inchToMeter, concrete= ACI_materials.c4000, steel= ACI_materials.A108, diam= 5/8*units_utils.inchToMeter, hef= 6.63*units_utils.inchToMeter, cast_in= True)
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
Nsa_kips=Nsa/units_utils.kipToN
ratio1=abs((Nsa_kips-19.94176)/19.94176)

#Concrete breakout failure in tension (art. D.5.2.)
Nb= stud.getBasicConcreteBreakoutStrengthTension() # Basic concrete breakout strength
Nb_kips=Nb/units_utils.kipToN
Ncb=stud.getConcrBreakoutStrengthTension()
Ncb_kips=Ncb/units_utils.kipToN
ratio2=abs((Ncb_kips-25.9126)/25.9126)

#Pullout stregth of stud to check head of the stud (article D.5.3)
Abearing=0.92*(in2m)**2
Npn=stud.getPulloutStrengthTension(Abearing)
Npn_kips=Npn/units_utils.kipToN
ratio3=abs((Npn_kips-29.44)/29.44)

#Check ductility in tension
Ndd=stud.getStrengthDuctilityTension(Abearing,cracking=True)
Ndd_kips=Ndd/units_utils.kipToN
ratio4=abs((Ndd_kips-22.025729)/22.025729)

'''
if Ndd >= Nua:
    print('Ndd >= Nua -> ductility OK')
else:
    print('Ndd < Nua -> NO ductility')
'''
#Check design strength of stud in tension
Nnd=stud.getDesignStrengthTension(Abearing,ductility=True,loadCombAlt=True)
Nnd_kips=Nnd/units_utils.kipToN
ratio5=abs((Nnd_kips-15.9534)/15.9534)

#SHEAR
#Calculate the nominal strength of the anchor in shear
Vsa=stud.getSteelStrengthShear()
Vsa_kips=Vsa/units_utils.kipToN
ratio6=abs((Vsa_kips-19.94176)/19.94176)

#Concrete breakout failure in shear (art. D.6.2.)
Vcb=stud.getConcrBreakoutStrengthShear()
Vcb_kips=Vcb/units_utils.kipToN
ratio7=abs((Vcb_kips-25.20286)/25.20286)
    
#Concrete pryout strength of anchor in shear (article D.6.3.)
Vcp=stud.getPryoutStrengthShear()
Vcp_kips=Vcp/units_utils.kipToN
ratio8=abs((Vcp_kips-51.82524)/51.82524)
    
#Check ductility in shear
Vdd=stud.getStrengthDuctilityShear()
Vdd_kips=Vdd/units_utils.kipToN
ratio9=abs((Vdd_kips-21.42243)/21.42243)
'''
if Vdd >= Vua:
    print('Vdd >= Vua -> ductility OK')
else:
    print('Vdd < Vua -> NO ductility')
'''

# Check design strength of stud in shear
Vnd=stud.getDesignStrengthShear(ductility=True,loadCombAlt=True)
Vnd_kips=Vnd/units_utils.kipToN
ratio10=abs((Vnd_kips-14.9563)/14.9563)
'''
if Vnd >= Vua:
    print('Vnd >= Vua -> design strength checking OK')
else:
    print('Vnd < Vua -> change stud dimensions')
'''
#Art. D.7
'''
if Vua <= 0.2*Vnd:
    print('Vua <= 0.2*Vnd -> full strength in tension is permitted')
if Nua <= 0.2*Nnd:
    print('Nua <= 0.2*Nnd -> full strength in shear is permitted')
if Vua > 0.2*Vnd and Nua > 0.2*Nnd:
    if Nua/Nnd + Vua/Vnd <= 1.2:
        print('Nua/Nnd + Vua/Vnd = ',round(Nua/Nnd + Vua/Vnd,2), ' <= 1.2')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-5) and (ratio2<1e-5) and (ratio3<1e-5) and (ratio4<1e-5) and (ratio5<1e-5) and (ratio6<1e-5) and (ratio7<1e-5) and (ratio8<1e-5) and (ratio9<1e-5) and (ratio10<1e-5)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

'''
# Export results to LaTeX
print('\\documentclass[12pt]{article}')
print('\\begin{document}')

print('The design of anchors below follows the methodology of the Guide to the Concrete Capacity Design (CCD) Method, reported by ACI Committee 349. \\\\ ')
#                   DATA
print('\\paragraph{Data}')
print('\\begin{center}')
print('\\begin{tabular}{p{8cm}r}')

# Materials
print('Anchor diameter & ',round(stud.diam*1e3,0), ' mm \\\\')
print('Effective embedment depth of anchor & ',round(stud.hef*1e3,0), ' mm \\\\')

print('Concrete $f_c^,$ = & ', round(abs(stud.concrete.fck)*1e-6,1), 'MPa \\\\')
print('Steel $f_{uta}$ = & ',round(stud.getFuta()*1e-6,1), 'MPa \\\\')
#Loads
#print('Critical load combination & ',comb_expr,' \\\\')
print('Applied factored external tension load N$_{ua}$= & ', round(Nua*1e-3,2), 'kN \\\\')
print('Applied factored external shear load V$_{ua}$= & ', round(Vua*1e-3,2), 'kN \\\\')
if stud.cast_in:
    print('cast in place stud $\\rightarrow\\ k_c$= & 24 \\\\')
else:
    print('post-installed stud $\\rightarrow\\ k_c$= & 17 \\\\')

print('\\end{tabular}')
print('\\end{center}')


print('\\paragraph{Nominal strength of the anchor in tension} \\emph{(art. D.5.1.2)} \\\\ ')
print('$$N_{sa} = nA_{se}f_{uta} = ' ,round(stud.getAnchorArea(),6),'\\times',round(stud.steel.fmaxk()*1e-3,1),' = ',  round(stud.getNominalSteelStrengthTension()*1e-3,1) , ' kN $$')
 

print('\\paragraph{Concrete breakout failure in tension} \\emph{(art. D.5.2.)}\\\\')

print('Basic concrete breakout strength of a single anchor in tension in cracked concrete: ')

print('$$N_b=k_c \\sqrt{f_c^,} h_{h_{ef}^{1.5}} = ',round(stud.getBasicConcreteBreakoutStrengthTension()*1e-3,1),' kN $$')
cracking=True
print('Nominal concrete breakout strength of a single anchor in tension: ')
print('$$N_{cb} = \\frac{A_{Nc}}{A_{Nco}} \\Psi_{ed,N}\\Psi_{C,N}\\Psi_{cp} N_b = \\frac{',stud.getANc(),'}{',stud.getANco(),'}\\cdot',  stud.getFactorEdgeN(),'\\cdot',stud.getFactorCrackingN(cracking),'\\cdot',stud.getFactorModifN(cracking),'\\cdot',round(stud.getBasicConcreteBreakoutStrengthTension()*1e-3,1),'= ',round(stud.getConcrBreakoutStrengthTension(cracking)*1e-3,1),' kN     $$')

print('\\paragraph{Pullout stregth of stud to check head of the stud} \\emph{(article D.5.3)}')
print('$$A_{bearing} = ',Abearing,' m^2 $$')
print('$$N_{pn} = ',round(stud.getPulloutStrengthTension(Abearing)*1e-3,1),' kN $$')
print('\\paragraph{Check ductility in tension}')
print('$$N_{dd}=', round(Ndd*1e-3,1), 'kN$$  $$N_{ua}=', round(Nua*1e-3,1), ' kN $$')
if Ndd >= Nua:
    print('$$N_{dd} \\ge N_{ua} \\rightarrow\\ ductility\\ OK$$')
else:
    print('$$N_{dd} \\lt N_{ua} \\rightarrow\\ NO\\ ductility$$')

print('\\paragraph{Nominal strength of the anchor in shear} \\emph{(art. D.6.1.2)}')
print('$$V_{sa} = 0.6 A_{se} f_{uta} = ', round(stud.getSteelStrengthShear()*1e-3,1),  ' kN $$')
print('\\paragraph{Concrete breakout failure in shear}\\emph{(art. D.6.2.)}')
print('$$V_{cb}= \\frac{A_{Vc}}{A_{Vco}}\\Psi_{ed,V}\\Psi_{c,V}V_b = ',round(Vcb*1e-3,1),' kN $$')
print('\\paragraph{Concrete pryout strength of anchor in shear} \\emph{(art. D.6.3.)}')
print('$$ V_{cp} = k_{cp} N_{cb} = ', round(Vcp*1e-3,1),' kN $$')
print('\\paragraph{Check ductility in shear}')
print('$$V_{dd}=', round(Vdd*1e-3,1), ' kN$$  $$V_{ua}=',round(Vua*1e-3,1), ' kN$$')
if Vdd >= Vua:
    print('$$ V_{dd} \\ge V_{ua} \\rightarrow\\ ductility\\ OK $$')
else:
    print('$$ V_{dd} \\lt V_{ua} \\rightarrow\\ NO\\ ductility $$')
print('\\paragraph{Check design strength of stud in shear}')
print('$$V_{nd}=', round(Vnd*1e-3,1), ' kN$$  $$ V_{ua}=',round(Vua*1e-3,1), ' kN$$')
if Vnd >= Vua:
    print('$$V_{nd} \\ge V_{ua} \\rightarrow\\ design\\ strength\\ checking\\ OK$$')
else:
    print('$$V_{nd} \\lt V_{ua} \\rightarrow\\ change\\ stud\\ dimensions$$')
if Vua <= 0.2*Vnd:
    print('$$V_{ua} \\le 0.2V_{nd} \\rightarrow\\ full\\ strength\\ in\\ tension\\ is\\ permitted$$')
if Nua <= 0.2*Nnd:
    print('$$N_{ua} \\le 0.2N_{nd} \\rightarrow\\ full strength in shear is permitted$$')
if Vua > 0.2*Vnd and Nua > 0.2*Nnd:
    if Nua/Nnd + Vua/Vnd <= 1.2:
        print('$$N_{ua}/N_{nd} + V_{ua}/V_{nd} = ',round(Nua/Nnd + Vua/Vnd,2), ' \\le 1.2$$')

print('\\end{document}')
'''
