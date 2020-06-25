# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

from rough_calculations import arch_bridge_simple_model as archsm


#Salginatobel Bridge
#horizontal distance between supports
L=295
#rise (depth) of the arch at midspan
d=42.6
#uniformly distributed load applied to the arch (dead load)
qunif=5.7
#Area of the cross-section at quarterpoints
Aquart=4.291 
#concentrated load at quarterpoints
Qlive=55

ArchBridge=archsm.ArchBridgeRoughModel(L,d)
#Abutment reactions
#   Vertical reaction at each abutment due to uniform load:
V=ArchBridge.getQunfVabtm(qunif)
#   Horizontal reaction at each abutment due to uniform load:
H=ArchBridge.getQunfHabtm(qunif)
#Stress at the quarterpoints of the arch under uniform load
fc=ArchBridge.getQunfCompStress(qunif,Aquart)

#Abutment reactions in the three hinged arch due to 
#concentrated loads in quarterpoints
Vlive=ArchBridge.getQconcVabtm(Qlive)
Hlive=ArchBridge.getQconcHabtm(Qlive)
fclive=ArchBridge.getQconcCompStress(Qlive,Aquart)
Mmax=ArchBridge.getMmaxQconc(Qlive)

# print('Vertical reaction at each abutment (dead load): V= ',V)
# print('Horizontal reaction at each abutment  (dead load): H= ',H)
# print('Stress at the quarterpoints of the arch under dead load: fc= ',fc)
# print('Vertical reaction at each abutment (live load): V= ',Vlive)
# print('Horizontal reaction at each abutment  (live load): H= ',Hlive)
# print('Mean stress at the quarterpoints of the arch under live load: fc= ',fclive)
# print('Maximum bending moment at the quarterpoints of the arch under live load: Mmax= ',Mmax)
ratio1=(V-840.75)/840.75
ratio2=(H-1455.5237676)/1455.5237676
ratio3=(fc+339.203861013)/(339.203861013)
ratio4=(Vlive-55)/55
ratio5=(Hlive-95.2171361502)/95.2171361502
ratio6=(fclive+22.1899641459)/22.1899641459
ratio7=(Mmax-1014.0625)/1014.0625

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10 and abs(ratio5)<1e-10 and abs(ratio6)<1e-10 and abs(ratio7)<1e-10 ):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
