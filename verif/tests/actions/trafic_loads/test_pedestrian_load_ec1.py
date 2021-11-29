# -*- coding: utf-8 -*-
''' Test values of pedestrian loads computed according to EC1.'''

from __future__ import division
from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from actions.pedestrian_trafic import EC1_pedestrian_load_models as plm

fs= 2.25 # Hz
walking= True
pedestrianLoad= plm.PedestrianLoad(fs, walking)

ti= list()
vLoad= list()
hLoad= list()
duration= 2
incT= duration/100
numSteps= int(duration/incT)
t= 0.0
for i in range(0,numSteps):
    ti.append(t)
    Fv= pedestrianLoad.getVerticalLoad(t)
    vLoad.append(Fv) # Vertical load.
    Fh= pedestrianLoad.getLateralLoad(t)
    hLoad.append(Fh) # Lateral load.
    t+= incT

vLoadMaxValue= max(vLoad)
vLoadMinValue= min(vLoad)
hLoadMaxValue= max(hLoad)
hLoadMinValue= min(hLoad)
vLoadMaxValueRef= 1080.4929616488178
vLoadMinValueRef= 422.5249570460704
hLoadMaxValueRef= 34.99568213685812
hLoadMinValueRef=-34.982729612800604

ratio1= abs(vLoadMaxValue-vLoadMaxValueRef)/vLoadMaxValueRef
ratio2= abs(vLoadMinValue-vLoadMinValueRef)/vLoadMinValueRef
ratio3= abs(hLoadMaxValue-hLoadMaxValueRef)/hLoadMaxValueRef
ratio4= abs(hLoadMinValue-hLoadMinValueRef)/-hLoadMinValueRef

'''
print('maximum vertical load: ', vLoadMaxValue)
print("ratio1= ", ratio1)
print('minimum vertical load: ', vLoadMinValue)
print("ratio2= ", ratio2)
print('maximum lateral load: ', hLoadMaxValue)
print("ratio3= ", ratio3)
print('minimum lateral load: ', hLoadMinValue)
print("ratio4= ", ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((ratio1<1e-15) and (ratio2<1e-15) and (ratio3<1e-15) and (ratio4<1e-15)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Display results
# import matplotlib.pyplot as plt
# plt.title('Single pedestrian. Vertical and lateral forces.')
# plt.xlabel('time (s)')
# plt.plot(ti, vLoad, label='Vertical load')
# plt.plot(ti, hLoad, label='Lateral load')
# plt.legend()
# plt.show()
