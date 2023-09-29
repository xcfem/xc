# -*- coding: utf-8 -*-
''' Test the getDict/setFromDict methods.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import json
import math
import geom
import xc

CooMax= 10.0

# Test problem.
fePrb= xc.FEProblem()
prep=  fePrb.getPreprocessor

# Read model from JSON file.
jsonFileName= '/tmp/test_export_xc_model_04.json'
f= open(jsonFileName,'r') # Open JSON file
inputData= json.load(f) # returns JSON object as a dictionary.

# Populate from the previous dictionary.
prep.setFromDict(inputData)

testSet1= prep.getSets.getSet('testSet1')
testSet1.fillDownwards()
numPoints= len(testSet1.points)
numLines= len(testSet1.lines)
numFaces= len(testSet1.surfaces)

err= math.sqrt((numPoints-4)**2+(numLines-4)**2+(numFaces-1)**2)

'''
# print(mbtDict)
print(d1)
print(d2)
print(d3)
print(err)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    

os.system('rm -f '+jsonFileName) # Your garbage you clean it
