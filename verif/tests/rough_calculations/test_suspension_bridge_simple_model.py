# -*- coding: utf-8 -*-

from __future__ import division

from rough_calculations import suspension_bridge_simple_model as sbm

GWBridge= sbm.SuspensionBridgeRoughModel(325,377,3500,650)

q= 39+8

Vm= GWBridge.getVm(q)
Hm= GWBridge.getHm(q)
Hb= GWBridge.getHb(q)
Vb= GWBridge.getVb(q)
Vanchor= GWBridge.getVanchor(q)

ratio1= (Vm-82250.0)/82250.0
ratio2= (Hm-221442.307692)/221442.307692
ratio3= (Hb-221442.307692)/221442.307692
ratio4= (Vb-143711.538462)/143711.538462
ratio5= (Vanchor-113161.538462)/113161.538462

'''
print 'Vm= ', Vm
print 'Hm= ', Hm
print 'Hb= ', Hb
print 'Vb= ', Vb
print 'Vanchor= ', Vanchor
'''

import os
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10 and abs(ratio5)<1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
