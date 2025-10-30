# -*- coding: utf-8 -*-
''' Trivial test for removing an existing load pattern.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O)"
__copyright__= "Copyright 2023, LCPT  AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.Ortega@ciccp.es"

import xc
from model import predefined_spaces


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Loads definition
testLP= modelSpace.newLoadPattern(name= 'testLP')
modelSpace.setCurrentLoadPattern(testLP.name)
modelSpace.setCurrentLoadPattern('') # No current load pattern

# Solution
resultOK= modelSpace.removeLoadPattern(testLP.name)

'''
print(resultOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if resultOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
