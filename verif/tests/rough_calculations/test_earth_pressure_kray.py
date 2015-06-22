# -*- coding: utf-8 -*-
# home made test
# Verification of Krey expressions for earth lateral pressures.

import xc_base
import geom
import xc
from rough_calculations import nb_poussee_terres as pt
import math

fi= math.pi/4.0
pDist= 1.0/math.sqrt(2)
P= pDist/4/math.tan(math.pi/4.0+fi/2.0)

ep= pt.KreyEarthPressurUnderConcentratedLoad(P,pDist,fi)

a= ep.a
b= ep.b
z_lim_sup= ep.getZLimSup()
z_lim_inf= ep.getZLimInf()
Q= ep.Q
p_max= ep.p_max
r1= ep.getRatioY(0.0,0.0)
r2= ep.getRatioY(a/4.0,0.0)
r3= ep.getRatioZ(0.0,z_lim_sup)
r4= ep.getRatioZ(0.0,z_lim_inf)
p= ep.getLateralPressure(0.0,z_lim_sup)
ratio1= (p-1.0)

# print "a= ", a
# print "b= ", b
# print "z_lim_sup= ", z_lim_sup
# print "z_lim_inf= ", z_lim_inf
# print "Q= ", Q
# print "p_max= ", p_max
# print "r1= ", r1
# print "r2= ", r2
# print "r3= ", r3
# print "r4= ", r4
# print "p= ", p

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."


