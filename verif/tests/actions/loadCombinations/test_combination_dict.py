# -*- coding: utf-8 -*-

from __future__ import print_function

from actions.load_combination_utils import utils
from misc_utils import log_messages as lmsg

combDict= utils.getCombinationDict('1.00*G1 + 1.00*G2 + 1.35*Qwind')
combDictRef= {'G1': 1.0, 'G2': 1.0, 'Qwind': 1.35}

error= False
for key in combDictRef:
    factor= combDict[key]
    refFactor= combDictRef[key]
    if(factor!=refFactor):
        error= True
'''        
print(combDict)
print(error)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (not error):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

