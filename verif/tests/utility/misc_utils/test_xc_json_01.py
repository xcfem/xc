# -*- coding: utf-8 -*-
''' Test remove_accents function.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2024 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import json
from misc_utils import xc_json

pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
    pth= '.'
sys.path.append(pth+"/../../aux/")
import xc_json_test_objects

## Get data in JSON format.
rawData= json.dumps(xc_json_test_objects.data, cls=xc_json.XCJSONEncoder)

## Get XC objects from raw data.
xcData= json.loads(rawData, cls=xc_json.XCJSONDecoder)

# Check result.
reinfRow= xc_json_test_objects.reinfRow
testOK= (reinfRow==xcData['reinforcement_row'])
concrete= xc_json_test_objects.eheConcrete
testOK&= (concrete==xcData['ehe_concrete'])
steel= xc_json_test_objects.eheSteel
testOK&= (steel==xcData['ehe_steel'])
rectgRCSection= xc_json_test_objects.rectgRCSection
testOK&= (rectgRCSection==xcData['rectangular_rc_section'])
testOK&= (rectgRCSection==xcData['copy_rectangular_rc_section'])
circularRCSection= xc_json_test_objects.circularRCSection
testOK&= (circularRCSection==xcData['circular_rc_section'])
sectionContainer= xc_json_test_objects.sectionContainer
testOK&= (sectionContainer==xcData['section_container'])
sectionDistribution= xc_json_test_objects.reinfConcreteSectionDistribution
testOK&= (sectionDistribution==xcData['section_distribution'])


'''
print('raw data= ', rawData)
print(testOK)
'''

from misc_utils import log_messages as lmsg
fName= os.path.basename(__file__)
if(testOK):
    print("test "+fName+": ok.")
else:
    lmsg.error('test: '+fName+' ERROR.')
