# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

import math
from rough_calculations import suspension_bridge_simple_model as sbm

GWBridge= sbm.SuspensionBridgeRoughModel(325,377,3500,650)

q= 39+8

Vm= GWBridge.getVm(q)
Hm= GWBridge.getHm(q)
Hb= GWBridge.getHb(q)
Vb= GWBridge.getVb(q)
Vanchor= GWBridge.getVanchor(q)
alpha= GWBridge.getCableSlopeAtTower(q)
alphaTeor= 0.355635884301
Ntower= GWBridge.getCableAxialForceAtTower(q)
NtowerTeor= 236223.957583
NmidSpan= GWBridge.getCableAxialForceAtMidSpan(q)
NmidSpanTeor= 221442.307692

AoneCable= 800 #in2
fallow= 82 #ksi

A4cables= 4.0*AoneCable
f= NmidSpan/A4cables
efficiency= f/fallow


ratio1= (Vm-82250.0)/82250.0
ratio2= (Hm-221442.307692)/221442.307692
ratio3= (Hb-221442.307692)/221442.307692
ratio4= (Vb-143711.538462)/143711.538462
ratio5= (Vanchor-113161.538462)/113161.538462
ratio6= (alpha-alphaTeor)/alphaTeor
ratio7= (Ntower-NtowerTeor)/NtowerTeor
ratio8= (NmidSpan-NmidSpanTeor)/NmidSpanTeor

'''
print('Vm= ', Vm)
print('Hm= ', Hm)
print('Hb= ', Hb)
print('Vb= ', Vb)
print('Vanchor= ', Vanchor)
print('alpha= ', alpha)
print('Ntower= ', Ntower)
print('NmidSpan= ', NmidSpan)
print('f= ', f)
print('efficiency= ', efficiency)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10 and abs(ratio5)<1e-10 and abs(ratio6)<1e-10 and abs(ratio7)<1e-10 and abs(ratio8)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
