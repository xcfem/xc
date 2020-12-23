#-*- coding: utf-8 -*-

from __future__ import division
import math
import sys
import numpy as np
import pydoc
import rough_calculations.masonry_vault as masonry_vault

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#Compute polynomial coefficients from four points that lie in the arc.
#(we take the 4 points obtained as rotules in the thesis example).
xA= 3.156
xC= 6.175
xD= 10.996
xB= 14.001

hA=6.019
hC=7.406
hD=6.659
hB=2.733

a = np.array([[pow(xA,4),pow(xA,3),pow(xA,2),xA], [pow(xB,4),pow(xB,3),pow(xB,2),xB],[pow(xC,4),pow(xC,3),pow(xC,2),xC],[pow(xD,4),pow(xD,3),pow(xD,2),xD]])
b = np.array([hA,hB,hC,hD])

arcGeom=masonry_vault.archGeometry()
arcGeom.coefPolArch=np.linalg.solve(a, b) #[f,j,k,r] coefficients of polynomial y=fx^4+jx^3+kx^2+rx+u (u=0)
#arcGeom.XRot=[xA,xB,xC,xD]     #X coordinates rotules A,B,C,D [m]
arcGeom.arcThick=1             #arch thickness (m)
arcGeom.arcSpan=15             #arch span [m]


fillChar= masonry_vault.FillingCharacteristics()
fillChar.angPhi= math.radians(30)   #internal friction angle
fillChar.cohesion=0                 #cohésion
fillChar.mp= 0.33                   #Correction factor.
fillChar.mc= 0.01                   #Correction factor.
fillChar.alpha= 0.726
fillChar.beta= 6.095
fillChar.swFill=18e3      #specific weight of filling material [N/m3]
fillChar.swSupStr=20e3    #specific weight or superstructure [N/m3]
fillChar.fillThick=9     #thickness of filling material in the endpoint of the arch [m]
fillChar.eqThickRoad=0.5  #equivalent thickness of road material [m]

trLoad=masonry_vault.trafficLoad()
trLoad.delta= math.radians(30)
trLoad.fillThickKeys= 1.5 # Hauteur du remplissage sur la clé de la voûte (m).
trLoad.Q= 160000 # Charge ponctuelle due au trafic (N).
trLoad.qrep= 0.005e6 # Charge uniformément repartie due au trafic (Pa).


permLoadRes=masonry_vault.permLoadResult(arcGeom,fillChar)

trafLoadRes=masonry_vault.trafficLoadResult(arcGeom,trLoad)

Nadmis=-1.25e6 #Effort axial admisible
resist= masonry_vault.resistance(Nadmis,arcGeom,fillChar,trLoad,permLoadRes,trafLoadRes)



res = resist.minimize()

#Results printing
#resist.printResults()

nTeor= 8.25829137054
n=resist.getSafCoef()
ratio1= abs(n-nTeor)/nTeor
HA= resist.getHA()
HAref= 2328579.69359
ratio2= abs(HA-HAref)/HAref
VA= resist.getVA()
VAref= 2140854.81205
ratio3= abs(VA-VAref)/VAref
HB= resist.getHB()
HBref= 893479.609326
ratio4= abs(HB-HBref)/HBref
VB= resist.getVB()
VBref= 2243014.05185
ratio5= abs(VB-VBref)/VBref

'''
print('n= ', n)
print('nTeor= ', nTeor)
print('ratio1= ', ratio1)
print('HA= ', HA)
print('HAref= ', HAref)
print('ratio2= ', ratio2)
print('VA= ', VA)
print('VAref= ', VAref)
print('ratio3= ', ratio3)
print('HB= ', HB)
print('HBref= ', HBref)
print('ratio4= ', ratio4)
print('VB= ', VB)
print('VBref= ', VBref)
print('ratio5= ', ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.06 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10 and abs(ratio5)<1e-10):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
