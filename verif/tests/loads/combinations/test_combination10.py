# -*- coding: utf-8 -*-
'''Test that active combinations are also cleared when removeAllLoadPatterns is
called.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces

# Create finite element problem object.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor  
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

modelSpace.newLoadPattern(name= 'lp01')
modelSpace.newLoadPattern(name= 'lp02')
modelSpace.addNewLoadCaseToDomain(loadCaseName= 'test', loadCaseExpression= '1.0*lp01+1.0*lp02', reset= False) # no previous loads, so no need to reset

currentLCNameBefore= modelSpace.getCurrentLoadCaseName() # Must be 'test'
modelSpace.removeAllLoadPatternsFromDomain() # Now it must be wiped-out.
currentLCNameAfter= modelSpace.getCurrentLoadCaseName() # Must be 'test'

testOK= True
if(currentLCNameAfter!=''):
    testOK= False
'''
print(currentLCNameBefore)
print(currentLCNameAfter)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
