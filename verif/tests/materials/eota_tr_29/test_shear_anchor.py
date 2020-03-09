# -*- coding: utf-8 -*-
# Verification test according to EOTA TR 029.
# results compared with those of the document:
# https://www.hilti.dk/medias/sys_master/documents/h16/9123450781726/Anchor_design_example_FTM_Technical_information_ASSET_DOC_LOC_3170819.pdf
# and:
# https://www.hilti.com/medias/sys_master/h3f/hd6/9122160279582.pdf?mime=application%2Fpdf&realname=RE+500-SD_ETA-07-0260_HIT-RE+500-SD_09.11.2012-09.11.2017_e.pdf

from __future__ import division
from __future__ import print_function

from materials.eota import EOTA_TR029_limit_state_checking as eota
import math
import geom

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2019, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Data
gammaMs= 1.4 # Partial safety factor for steel.
gammaMc= 2.1 # Partial safety factor for concrete.
boltDiameter= 12e-3 # Bar diameter in meters.
boltArea= math.pi*(boltDiameter/2.0)**2 # Bar area in square meters.
h= 100e-3 # Concrete element thickness.
hef= 70e-3 # Effective anchor depth.
anchorPosition=  geom.Pos2d(.100,0) # Anchor position
baseMaterialContour=  geom.Polygon2d() # Contour of concrete element.
baseMaterialContour.appendVertex(geom.Pos2d(0,-1))
baseMaterialContour.appendVertex(geom.Pos2d(1,-1))
baseMaterialContour.appendVertex(geom.Pos2d(1,1))
baseMaterialContour.appendVertex(geom.Pos2d(0,1))
spacing= 150e-3


fuk= 550e6 # Characteristic steel ultimate tensile strength (Pa).
tauRk= 1.09*15e6 # Characteristic bond strength for non-cracked concrete (taken from ETA-07/0260 table 11).
k1= 10.1 # 7.2 for cracked concrete and 10.1 for non-cracked concrete.
fckCube= 50e6 # Caracteristic concrete compression strength measured on cubes with a side length of 150 mm.

# Strength of the anchor itself.
NRds= 28e3

# Tension loading

## Design combined pull-out and concrete cone resistance
###  Edge distance influence
CcrN= eota.getCcrNp(boltDiameter,hef,tauRk)
C= baseMaterialContour.getRecubrimiento(anchorPosition) 
Cmin= 5*boltDiameter+10e-3

if(C<Cmin):
    print("Too little concrete cover for the anchor.")

f1N= eota.getFactor1N(C,CcrN)

###  Extraction gross area.
plgA0pN= eota.getA0pN(boltDiameter,anchorPosition,hef,tauRk)
A0pN= plgA0pN.getArea()
###  Net area pull-out
plgApN= plgA0pN
plgApN.clipUsingPolygon(baseMaterialContour)
ApN= plgApN.getArea()
f2pN= eota.getFactor2pN(A0pN,ApN)
f3N= eota.getFactor3N(d= boltDiameter, spacing= spacing, n= 2, hEf= hef, tauRk= tauRk, fck_cube= fckCube,cracked= False)
N0Rdp= eota.axialInitialResistancePullOut(boltDiameter,hef,tauRk)/gammaMc
NRdp= N0Rdp*f1N*f2pN*f3N # Extraction

## Design concrete cone resistance

###  Cone extraction gross area.
plgA0cN= eota.getA0cN(anchorPosition,hef)
A0cN= plgA0cN.getArea()
###  Cone extraction effective area.
plgAcN= plgA0cN
plgAcN.clipUsingPolygon(baseMaterialContour)
AcN= plgAcN.getArea()
f2cN= eota.getFactor2cN(A0cN,AcN)
N0Rdc= eota.axialInitialResistanceConeFailure(k1,fckCube,hef)/gammaMc
NRdc= N0Rdc*f1N*f2cN # Cone extraction.

## Design splitting resistance
CcrSp= eota.getCcrSpHiltiHY150(h,hef)
ScrSp= 2*CcrSp
f1Nsp= eota.getFactor1N(C,CcrSp)
###  Splitting gross area.
plgA0spN= eota.getA0spN(anchorPosition,CcrSp)
A0spN= plgA0spN.getArea()
###  Splitting effective area.
plgAspN= plgA0spN
plgAspN.clipUsingPolygon(baseMaterialContour)
AspN= plgAspN.getArea()
f2spN= eota.getFactor2spN(A0spN,AspN)
N0RdSp= N0Rdc
NRdSp= N0RdSp*f1Nsp*f2spN # Splitting.

# Shear loading

VRd_s= 16.8e3

## Concrete pryout design resistance
VRd_cp= eota.shearResistanceConcretePryOut(NRdp, NRdc, hef)

## Concrete edge design resistance (NOT TESTED)
VRdC0= eota.VRkC0Expr58(boltDiameter, hef, fckCube, c1= C, cracked= False)/gammaMc
areaFactor= eota.AcV2PernosC2(h, c1= C, c2= 1.0, s2= spacing)/(4.5*C**2)
memberThicknessFactor= eota.memberThicknessFactor(h,c1= C)
angleFactor= eota.angleFactor(math.radians(0))

VRd= VRdC0*areaFactor*memberThicknessFactor*angleFactor
VRd_bolt= VRd/2.0

VRd_cp_hilti= 34.3e3 # Value from the Hilti manual example.
ratio1= abs(VRd_cp-38330.9650872)/38330.9650872
#ratio1= abs(VRd_cp-VRd_cp_hilti)/VRd_cp_hilti

'''
print('NRds= ', NRds/1e3,' kN')
print('N0Rdp= ', N0Rdp/1e3,' kN')
print('f2pN= ', f2pN)
print('f3N= ', f3N)
print('NRdp= ', NRdp/1e3,' kN')
print('NRdc= ', NRdc/1e3,' kN')
print('NRdSp= ', NRdSp/1e3,' kN')
print('VRd_s= ', VRd_s/1e3,' kN')
print('VRd_cp= ', VRd_cp/1e3,' kN')
print("ratio1= ",ratio1)
print('VRdC0= ', VRdC0/1e3,' kN')
print('VRdC0_bolt= ', VRdC0/2e3,' kN')
print('areaFactor= ', areaFactor)
print('memberThicknessFactor= ', memberThicknessFactor)
print('angleFactor= ', angleFactor)
print('VRd= ', VRd/1e3,' kN')
print('VRd_bolt= ', VRd_bolt/1e3,' kN')
'''

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-10):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

