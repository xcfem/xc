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
import xc
from misc_utils import xc_json
from postprocess import RC_material_distribution

## Section geometry.
feProblem= xc.FEProblem() # FE problem.
feProblem.title= 'Building columns effective lengths.'
preprocessor= feProblem.getPreprocessor

reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()

# Read data structure from JSON file.
fName= os.path.basename(__file__)
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
    pth= '.'
inputFileName= pth+"/../../aux/reference_files/ref_xc_json_05.json"

reinfConcreteSectionDistribution.readFromJSON(fileName= inputFileName, preprocessor= preprocessor)
sectionContainer= reinfConcreteSectionDistribution.sectionDefinition
rcMemberSection= sectionContainer.sections[0]
rcRectangularSection= rcMemberSection.templateSections[0]
xcSection= rcRectangularSection.xc_material
sectionBucklingProperties= xcSection.getProp('sectionBucklingProperties')

# print(sectionBucklingProperties.Cz)

# Check some results.
testOK= (sectionBucklingProperties.Cz == 0.24)

'''
print(rawData)
print(xcData)
print(testOK)
'''

from misc_utils import log_messages as lmsg
fName= os.path.basename(__file__)
if(testOK):
    print("test "+fName+": ok.")
else:
    lmsg.error('test: '+fName+' ERROR.')
    
# Report sections.
# reinfConcreteSectionDistribution.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)
