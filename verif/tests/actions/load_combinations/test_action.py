# -*- coding: utf-8 -*-
''' Trivial test for actions in loadCombinations module.'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import loadCombinations
from misc_utils import log_messages as lmsg

actionA= loadCombinations.Action('TestAction',"Test action description.")
actionA.weightingFactor= 3.141592

actionA.relationships.appendIncompatible('IncompA.*')
actionA.relationships.appendIncompatible('IncompB.*')
actionA.relationships.appendMain('MainAction1')
actionA.relationships.appendMain('MainAction2')

actionAData= actionA.getDict()

# Test reading from dictionary.
actionB= loadCombinations.Action('DummyAction',"Test action reading.")

actionB.setFromDict(actionAData)
actionBData= actionB.getDict()


refDict= {'ownerAddress': 0, 'name': 'TestAction', 'description': 'Test action description.', 'relations': {'ownerAddress': 0, 'incompatibles': ['IncompA.*', 'IncompB.*'], 'main_actions': ['MainAction1', 'MainAction2'], 'contiene_incomp': False}, 'nodet': False, 'f_pond': 3.141592}

ok1= (actionAData==refDict)
ok2= (actionBData==refDict)

'''
print('Action name: '+actionA.name)
print('Action description: '+actionA.description)
print('Action weighting factor: '+str(actionA.weightingFactor))
print('Not-determinant action: '+str(actionA.not_determinant))
print(actionAData, ok1)
print(actionBData, ok2)
'''

import os
fname= os.path.basename(__file__)
if (ok1 and ok2):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
