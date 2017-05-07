# -*- coding: utf-8 -*-


from actions.wind import vientoCTE
from actions.wind import vientoCubiertaDosAguasCTE
from actions.wind import vientoParamVerticalesCTE
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#execfile("interpolacion.lcmdm")

cpPruebaF= vientoCubiertaDosAguasCTE.cpDosAguasZonaFVTrsv(5.5,math.radians(30))
cpPruebaG= vientoCubiertaDosAguasCTE.cpDosAguasZonaGVTrsv(5.5,math.radians(-15))
cpPruebaGL= vientoCubiertaDosAguasCTE.cpDosAguasZonaGVLong(5.5,math.radians(45))
# Paramentos verticales, zona A
cpPruebaAPV0= vientoParamVerticalesCTE.cpParamVertZonaAVTrsv(15,5,5)
cpPruebaAPV1= vientoParamVerticalesCTE.cpParamVertZonaAVTrsv(7.5,5,5)
cpPruebaAPV2= vientoParamVerticalesCTE.cpParamVertZonaAVTrsv(3.5,5,5)
cpPruebaAPV3= vientoParamVerticalesCTE.cpParamVertZonaAVTrsv(1.5,5,5)
cpPruebaAPV4= vientoParamVerticalesCTE.cpParamVertZonaAVTrsv(0.5,5,5)
# Paramentos verticales, zona B
cpPruebaBPV0= vientoParamVerticalesCTE.cpParamVertZonaBVTrsv(15,5,5)
cpPruebaBPV1= vientoParamVerticalesCTE.cpParamVertZonaBVTrsv(7.5,5,5)
cpPruebaBPV2= vientoParamVerticalesCTE.cpParamVertZonaBVTrsv(3.5,5,5)
cpPruebaBPV3= vientoParamVerticalesCTE.cpParamVertZonaBVTrsv(1.5,5,5)
cpPruebaBPV4= vientoParamVerticalesCTE.cpParamVertZonaBVTrsv(0.5,5,5)
# Paramentos verticales, zona C
cpPruebaCPV0= vientoParamVerticalesCTE.cpParamVertZonaCVTrsv(15,5,5)
cpPruebaCPV1= vientoParamVerticalesCTE.cpParamVertZonaCVTrsv(7.5,5,5)
cpPruebaCPV2= vientoParamVerticalesCTE.cpParamVertZonaCVTrsv(3.5,5,5)
cpPruebaCPV3= vientoParamVerticalesCTE.cpParamVertZonaCVTrsv(1.5,5,5)
cpPruebaCPV4= vientoParamVerticalesCTE.cpParamVertZonaCVTrsv(0.5,5,5)
# Paramentos verticales, zona D
cpPruebaDPV0= vientoParamVerticalesCTE.cpParamVertZonaDVTrsv(15,5,5)
cpPruebaDPV1= vientoParamVerticalesCTE.cpParamVertZonaDVTrsv(7.5,5,5)
cpPruebaDPV2= vientoParamVerticalesCTE.cpParamVertZonaDVTrsv(3.5,5,5)
cpPruebaDPV3= vientoParamVerticalesCTE.cpParamVertZonaDVTrsv(1.5,5,5)
cpPruebaDPV4= vientoParamVerticalesCTE.cpParamVertZonaDVTrsv(0.5,5,5)
# Paramentos verticales, zona E
cpPruebaEPV0= vientoParamVerticalesCTE.cpParamVertZonaEVTrsv(15,3,1)
cpPruebaEPV1= vientoParamVerticalesCTE.cpParamVertZonaEVTrsv(7.5,3,1)
cpPruebaEPV2= vientoParamVerticalesCTE.cpParamVertZonaEVTrsv(3.5,3,1)
cpPruebaEPV3= vientoParamVerticalesCTE.cpParamVertZonaEVTrsv(1.5,3,1)
cpPruebaEPV4= vientoParamVerticalesCTE.cpParamVertZonaEVTrsv(0.5,3,1)


ratio1= (cpPruebaF+0.7596373105057561)
ratio2= (cpPruebaG+1.481746117354029)
ratio3= (cpPruebaGL+1.555782386303454)

ratioA4= (cpPruebaAPV0+1.2)
ratioA5= (cpPruebaAPV1+1.25)
ratioA6= (cpPruebaAPV2+1.3)
ratioA7= (cpPruebaAPV3+1.35)
ratioA8= (cpPruebaAPV4+1.4)

ratioB4= (cpPruebaBPV0+0.8)
ratioB5= (cpPruebaBPV1+0.85)
ratioB6= (cpPruebaBPV2+0.95)
ratioB7= (cpPruebaBPV3+1.05)
ratioB8= (cpPruebaBPV4+1.1)

ratioC4= (cpPruebaCPV0+0.5)
ratioC5= (cpPruebaCPV1+0.5)
ratioC6= (cpPruebaCPV2+0.5)
ratioC7= (cpPruebaCPV3+0.5)
ratioC8= (cpPruebaCPV4+0.5)

ratioD4= (cpPruebaDPV0-0.8)
ratioD5= (cpPruebaDPV1-0.85)
ratioD6= (cpPruebaDPV2-0.9)
ratioD7= (cpPruebaDPV3-0.95)
ratioD8= (cpPruebaDPV4-1.0)

ratioE4= (cpPruebaEPV0+0.6)
ratioE5= (cpPruebaEPV1+0.6)
ratioE6= (cpPruebaEPV2+0.6)
ratioE7= (cpPruebaEPV3+0.6)
ratioE8= (cpPruebaEPV4+0.6)

'''
print "cpPruebaF= ",cpPruebaF
print "ratio1= ",ratio1
print "cpPruebaG= ",cpPruebaG
print "ratio2= ",ratio2
print "cpPruebaGL= ",cpPruebaGL
print "ratio3= ",ratio3

print "cpPruebaAPV0= ",cpPruebaAPV0
print "ratioA4= ",ratioA4
print "cpPruebaAPV1= ",cpPruebaAPV1
print "ratioA5= ",ratioA5
print "cpPruebaAPV2= ",cpPruebaAPV2
print "ratioA6= ",ratioA6
print "cpPruebaAPV3= ",cpPruebaAPV3
print "ratioA7= ",ratioA7
print "cpPruebaAPV4= ",cpPruebaAPV4
print "ratioA8= ",ratioA8

print "cpPruebaBPV0= ",cpPruebaBPV0
print "ratioB4= ",ratioB4
print "cpPruebaBPV1= ",cpPruebaBPV1
print "ratioB5= ",ratioB5
print "cpPruebaBPV2= ",cpPruebaBPV2
print "ratioB6= ",ratioB6
print "cpPruebaBPV3= ",cpPruebaBPV3
print "ratioB7= ",ratioB7
print "cpPruebaBPV4= ",cpPruebaBPV4
print "ratioB8= ",ratioB8

print "cpPruebaCPV0= ",cpPruebaCPV0
print "ratioC4= ",ratioC4
print "cpPruebaCPV1= ",cpPruebaCPV1
print "ratioC5= ",ratioC5
print "cpPruebaCPV2= ",cpPruebaCPV2
print "ratioC6= ",ratioC6
print "cpPruebaCPV3= ",cpPruebaCPV3
print "ratioC7= ",ratioC7
print "cpPruebaCPV4= ",cpPruebaCPV4
print "ratioC8= ",ratioC8

print "cpPruebaDPV0= ",cpPruebaDPV0
print "ratioD4= ",ratioD4
print "cpPruebaDPV1= ",cpPruebaDPV1
print "ratioD5= ",ratioD5
print "cpPruebaDPV2= ",cpPruebaDPV2
print "ratioD6= ",ratioD6
print "cpPruebaDPV3= ",cpPruebaDPV3
print "ratioD7= ",ratioD7
print "cpPruebaDPV4= ",cpPruebaDPV4
print "ratioD8= ",ratioD8

print "cpPruebaEPV0= ",cpPruebaEPV0
print "ratioE4= ",ratioE4
print "cpPruebaEPV1= ",cpPruebaEPV1
print "ratioE5= ",ratioE5
print "cpPruebaEPV2= ",cpPruebaEPV2
print "ratioE6= ",ratioE6
print "cpPruebaEPV3= ",cpPruebaEPV3
print "ratioE7= ",ratioE7
print "cpPruebaEPV4= ",cpPruebaEPV4
print "ratioE8= ",ratioE8
''' 


import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12) & (abs(ratio3)<1e-12) & (abs(ratioA4)<1e-12) & (abs(ratioA5)<1e-12) & (abs(ratioA6)<1e-12) & (abs(ratioA7)<1e-12) & (abs(ratioA8)<1e-12) & (abs(ratioB4)<1e-12) & (abs(ratioB5)<1e-12) & (abs(ratioB6)<1e-12) & (abs(ratioB7)<1e-12) & (abs(ratioB8)<1e-12) & (abs(ratioC4)<1e-12) & (abs(ratioC5)<1e-12) & (abs(ratioC6)<1e-12) & (abs(ratioC7)<1e-12) & (abs(ratioC8)<1e-12) & (abs(ratioD4)<1e-12) & (abs(ratioD5)<1e-12) & (abs(ratioD6)<1e-12) & (abs(ratioD7)<1e-12) & (abs(ratioD8)<1e-12) & (abs(ratioE4)<1e-12) & (abs(ratioE5)<1e-12) & (abs(ratioE6)<1e-12) & (abs(ratioE7)<1e-12) & (abs(ratioE8)<1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
