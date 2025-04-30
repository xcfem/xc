# -*- coding: utf-8 -*-
'''Check LoadCase class basic methods.'''

__author__= "Ana Ortega (AO_O)  Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, AO_O  LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es l.pereztato@gmail.com"

from actions import load_cases

lc1= load_cases.LoadCase(preprocessor= None, name= "lc1", loadPType="default", timeSType= "constant_ts")
lc2= load_cases.LoadCase(preprocessor= None, name= "lc2", loadPType="uniform_excitation", timeSType= "linear_ts")

dct1= lc1.getDict()
dct2= lc2.getDict()

lc3= load_cases.LoadCase(preprocessor= None, name= None)
lc3.setFromDict(dct1)

lc4= load_cases.LoadCase(preprocessor= None, name= None)
lc4.setFromDict(dct2)

dct3= lc3.getDict()
dct4= lc4.getDict()

testOK= (dct1==dct3) and (dct2==dct4)

'''
print(dct1)
print(dct2)
print(dct3)
print(dct4)
print(testOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
