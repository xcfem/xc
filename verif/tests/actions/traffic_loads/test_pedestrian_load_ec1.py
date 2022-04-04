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
from actions.pedestrian_traffic import EC1_pedestrian_load_models as plm

fs= 2.25 # Hz
walking= True
pedestrianLoad= plm.PedestrianLoad(fs, walking)

ti= pedestrianLoad.getTimeSequence()
vLoad= pedestrianLoad.getVerticalLoadHistory(ti) # Vertical load.
hLoad= pedestrianLoad.getLateralLoadHistory(ti) # Lateral load.

vLoadMaxValue= max(vLoad)
vLoadMinValue= min(vLoad)
hLoadMaxValue= max(hLoad)
hLoadMinValue= min(hLoad)
vLoadMaxValueRef= 1079.558203775136
vLoadMinValueRef= 433.70417543735635
hLoadMaxValueRef= 35.0
hLoadMinValueRef=-35.0

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
    
# Display results
# pedestrianLoad.plot(loadToDisp= 'horizontal')

