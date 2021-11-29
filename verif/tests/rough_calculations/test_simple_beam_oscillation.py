# -*- coding: utf-8 -*-
''' Test simplified dynamic analysis of a single span rail bridge.'''

from __future__ import print_function
import rough_calculations.ng_punching_shear

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
from actions.railway_trafic import IAPF_rail_load_models as rm
from rough_calculations import simple_beam_oscillation as sbo

L= 39.2
EI= 7.69879e+11
f= 26.54e-3
rho= 12713.38e3/L
P= -100e3
amortig= 2/100 # Amortiguamiento

w1= (math.pi**2)*math.sqrt(EI/rho/(L**4))
T1= 2*math.pi/w1
f1= 1/T1


M1= rho*L/2
uCentroP= P*L**3/48/EI # Flecha en el centro de vano para la carga P.
#deltaCentro=  # Flecha en el centro de vano para la carga P.


vProy= 270/3.6
vIni= 100/3.6
vFin= 1.2*vProy


fMaxima= open("fMaximaICE2.dat", "w")
fDinTrenICE2= sbo.flechaDinamicaMinimaTrenRangoVel(rm.cargasTrenICE2,rho,L,w1,amortig,L/2,vIni,vFin,fMaxima)
fMaxima.close()
os.system("rm fMaximaICE2.dat")

aMaxima= open("aMaximaICE2.dat", "w")
aExtremaICE2= sbo.aceleracionExtremaTrenRangoVel(rm.cargasTrenICE2,rho,L,w1,amortig,L/2,vIni,vFin,aMaxima)
aMaxima.close()
os.system("rm aMaximaICE2.dat")


fMaxima= open("fMaximaTalgo350.dat", "w")
fDinTrenTalgo350= sbo.flechaDinamicaMinimaTrenRangoVel(rm.cargasTrenTalgo350,rho,L,w1,amortig,L/2,vIni,vFin,fMaxima)
fMaxima.close()
os.system("rm fMaximaTalgo350.dat")


aMaxima= open("aMaximaTalgo350.dat", "w")
aExtremaTalgo350= sbo.aceleracionExtremaTrenRangoVel(rm.cargasTrenTalgo350,rho,L,w1,amortig,L/2,vIni,vFin,aMaxima)
aMaxima.close()
os.system("rm aMaximaTalgo350.dat")

ratio1= abs(fDinTrenICE2+0.0029835051308262864)/0.0029835051308262864
ratio2= abs(aExtremaICE2+0.2514528918690407)/0.2514528918690407
ratio3= abs(fDinTrenTalgo350+0.002642310256771745)/0.002642310256771745
ratio4= abs(aExtremaTalgo350-0.25839798903070293)/0.25839798903070293

'''
print("EI= ",EI/1e3,"kN m2\n")
print("rho= ",rho,"\n")
print("w1= ",w1/math.pi,"*pi rad/s\n")
print("T1= ",T1," s\n")
print("f1= ",f1," Hz\n")

print("fDinTrenICE2= ",fDinTrenICE2," m\n")
print("ratio1= ", ratio1)
print("aExtremaICE2= ",aExtremaICE2," m/s2\n")
print("ratio2= ", ratio2)
print("fDinTrenTalgo350= ",fDinTrenTalgo350," m\n")
print("ratio3= ", ratio3)
print("aExtremaTalgo350= ",aExtremaTalgo350," m/s2\n")
print("ratio4= ", ratio4)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5 and abs(ratio1)<1e-5 and abs(ratio4)<1e-5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
