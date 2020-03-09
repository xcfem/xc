# -*- coding: utf-8 -*-
from __future__ import division
from rough_calculations import tied_arch_simple_model as tiedArch

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

#Barqueta Bridge
#horizontal distance between supports
L=551
#rise (depth) of the arch at midspan
d=53
#uniformly distributed load applied to the arch (k/ft)
qdead=8.2     
qlive=5.7
qunif=qdead+qlive
#Cross-sectional area of the arch at the abutments
A=7530
#concentrated load at quarterpoints
Qlive=55

tiedArchBridge=tiedArch.TiedArchBridgeRoughModel(L,d)
#Abutment reactions
#   Vertical reaction at each abutment due to uniform load:
V=tiedArchBridge.getQunfVabtm(qunif)
#   Horizontal reaction at each abutment due to uniform load:
H=tiedArchBridge.getQunfHabtm(qunif)
#Tension in the deck
Tdeck=tiedArchBridge.getTensionDeck(qunif)
# Axial compressive force in the arch
N=tiedArchBridge.getAxialForceArch(qunif)
#Compressive stress in the arch at the abutments
fc=tiedArchBridge.getQunfCompStress(qunif,A)


# print 'Vertical reaction at each abutment: V= ',V
# print 'Horizontal reaction at each abutment : H= ',H
# print 'Tension nin the deck T: ', Tdeck
# print 'Axial compressive force in the arch N: ', N
# print 'Mean compressive stress in the arch at the abutments fc: ',fc


ratio1=(V-3829.45)/3829.45
ratio2=(H-9952.95731132)/9952.95731132
ratio3=(Tdeck-9952.95731132)/9952.95731132
ratio4=(N-10664.2414894)/10664.2414894
ratio5=(fc+1.41623392953)/1.41623392953


import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10 and abs(ratio5) ):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
