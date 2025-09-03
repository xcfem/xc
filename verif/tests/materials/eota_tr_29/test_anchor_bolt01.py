# -*- coding: utf-8 -*-
''' Verification test according to EOTA TR 029.

Results compared with those of the document
"Hilti HIT-HY 150 with rebar".
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.eota import EOTA_TR029_limit_state_checking as eota
import math
import geom

# Datos
gammaMs= 1.4 # Partial safety factor for steel.
gammaMc= 2.1 # Partial safety factor for concrete.
barDiameter= 25e-3 # Bar diameter in meters.
barArea= math.pi*(barDiameter/2.0)**2 # Bar area in square meters.
h= 274e-3 # Concrete element thickness.
hef= 210e-3 # Effective anchor depth.
anchorPosition= geom.Pos2d(.135,0) # Anchor position
baseMaterialContour= geom.Polygon2d([geom.Pos2d(0,-1), geom.Pos2d(1,-1), geom.Pos2d(1,1), geom.Pos2d(0,1)]) # Contour of concrete element.

fuk= 550e6 # Characteristic steel ultimate tensile strength (Pa).
tauRk= 7.5e6 # Characteristic bond strength (taken from ETA-05/0051 table 11).
tauRkUcr= 7.5e6 # Characteristic bond strength for non-cracked concrete.
k1= 10.1 # 7.2 for cracked concrete and 10.1 for non-cracked concrete.
fckCube= 25e6 # Characteristic concrete compression strength measured on cubes with a side length of 150 mm.
# Strength of the anchor itself.
NRds= eota.axialResistanceSteelFailure(barArea,fuk)/gammaMs


# Edge distance influence
CcrN= eota.getCcrNp(barDiameter,hef,tauRkUcr)
C= baseMaterialContour.getCover(anchorPosition) 
Cmin= 5*barDiameter+10e-3

if (C<Cmin):
    print("Too little concrete cover for the anchor.")

f1N= eota.getFactor1N(C,CcrN)

# Extraction gross area.
plgA0pN= eota.getA0pN(barDiameter,anchorPosition,hef,tauRkUcr)
A0pN= plgA0pN.getArea()
# Área neta pull-out
plgApN= plgA0pN
plgApN.clipUsingPolygon(baseMaterialContour)
ApN= plgApN.getArea()
f2pN= eota.getFactor2pN(A0pN,ApN)
N0Rdp= eota.axialInitialResistancePullout(barDiameter,hef,tauRk)/gammaMc
NRdp= N0Rdp*f1N*f2pN # Extraction

# Cone extraction gross area.
plgA0cN= eota.getA0cN(anchorPosition,hef)
A0cN= plgA0cN.getArea()
# Cone extraction effective area.
plgAcN= plgA0cN
plgAcN.clipUsingPolygon(baseMaterialContour)
AcN= plgAcN.getArea()
f2cN= eota.getFactor2cN(A0cN,AcN)
N0Rdc= eota.axialInitialResistanceConeFailure(k1,fckCube,hef)/gammaMc
NRdc= N0Rdc*f1N*f2cN # Cone extraction.

# Splitting
CcrSp= eota.getCcrSpHiltiHY150(h,hef)
ScrSp= 2*CcrSp
f1Nsp= eota.getFactor1N(C,CcrSp)
# Splitting gross area.
plgA0spN= eota.getA0spN(anchorPosition,CcrSp)
A0spN= plgA0spN.getArea()
# Splitting effective area.
plgAspN= plgA0spN
plgAspN.clipUsingPolygon(baseMaterialContour)
AspN= plgAspN.getArea()
f2spN= eota.getFactor2spN(A0spN,AspN)
N0RdSp= N0Rdc
NRdSp= N0RdSp*f1Nsp*f2spN # Splitting.

# Results

''' Tensile strength of the anchor itself without considering
the favourable effect of the reinforcement. '''
NRd= min(min(min(NRdSp,NRdc),NRdp),NRds)

ratio1= abs(NRds-192900)/192900
ratio2= abs(N0Rdp-58.9e3)/58.9e3
ratio3= abs(N0Rdc-73.2e3)/73.2e3
ratio4= abs(f1N-0.828571)/0.828571
f2pNTeor= ((2*CcrN)**2-(CcrN-C)*2*CcrN)/(2*CcrN)**2
ratio5= abs(f2pN-f2pNTeor)/f2pNTeor
CcrNc= eota.getScrN(hef)/2
f2cNTeor= ((2*CcrNc)**2-(CcrNc-C)*2*CcrNc)/(2*CcrNc)**2
ratio6= abs(f2cN-f2cNTeor)/f2cNTeor
ratio7= abs(NRd-34.8621e3)/34.8621e3

'''
print("A_s= ",barArea*1e6," mm2\n")
print("C_{cr,N}= ",CcrN," m\n")
print("C= ",C," m\n")
print("C/C_{cr,N}= ",C/CcrN)
print("Extraction gross area A0pN= ",A0pN," m\n")
print("Extraction effective area ApN= ",ApN," m\n")
print("Extraction gross area cono A0cN= ",A0cN," m\n")
print("Extraction effective area cono AcN= ",AcN," m\n")
print("Splitting gross area A0spN= ",A0spN," m\n")
print("Splitting effective area AcN= ",AspN," m\n")
print("f1N= ",f1N)
print("f2pNTeor= ",f2pNTeor)
print("f2pN= ",f2pN)
print("f2cN= ",f2cN)
print("C_{cr,sp}= ",CcrSp," m\n")
print("N_{Rd,s}= ",NRds/1e3," kN\n")
print("N_{Rd,p}= ",NRdp/1e3," kN\n")
print("N_{Rd,c}= ",NRdc/1e3," kN\n")
print("N_{Rd,sp}= ",NRdSp/1e3," kN\n")
print("N_{Rd}= ",NRd/1e3," kN\n")
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
print("ratio4= ",(ratio4))
print("ratio5= ",(ratio5))
print("ratio6= ",(ratio6))
print("ratio7= ",(ratio7))
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<5e-4) & (ratio2<5e-4) & (ratio3<5e-4) & (ratio4<5e-5) & (ratio5<0.03) & (ratio6<0.03) & (ratio6<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
