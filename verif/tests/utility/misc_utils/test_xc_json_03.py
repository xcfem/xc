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
import xc
from misc_utils import xc_json
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_column_RC_section

pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
    pth= '.'
auxPth= pth+"/../../aux/reference_files/"

## Define XC section
feProblem= xc.FEProblem()
feProblem.title= 'XC JSON encoder/decoder test.'
preprocessor= feProblem.getPreprocessor

inputFileName= auxPth+'ref_xc_json_02.json'

## Get XC objects from raw data.
xc_json.XCJSONDecoder.preprocessor= preprocessor
with open(inputFileName,'r') as f:
    xcData= json.load(f, cls=xc_json.XCJSONDecoder)

iZ= xcData['sectionData'].Iz()
refIz= 0.11984224905356572
ratioIz= abs(iZ-refIz)/refIz

# Check some results.
testOK= (xcData['sectionDepthZ']==1.25)
testOK&= (abs(ratioIz)<1e-5)

from misc_utils import log_messages as lmsg
fName= os.path.basename(__file__)
if(testOK):
    print("test "+fName+": ok.")
else:
    lmsg.error('test: '+fName+' ERROR.')
