# -*- coding: utf-8 -*-
''' Check the get_combination_dict function (see utils.py in 
    load_combination_utils).

   Given a combination expression of the form '1.0*G1+1.35*G2+1.5*Q+...'
   this fuction returns its components in a dictionary with the form:
   {'G1':1.0, 'G2':1.35, 'Q':1.5, ...}
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"


from actions.load_combination_utils import utils

combDict= utils.get_combination_dict('1.00*G1 + 1.00*G2 + 1.35*Qwind')
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

