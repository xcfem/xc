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
from materials.ehe import EHE_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section

concr= EHE_materials.HA30
steel= EHE_materials.B500S
## Section geometry.
feProblem= xc.FEProblem() # FE problem.
feProblem.title= 'Building columns effective lengths.'
preprocessor= feProblem.getPreprocessor

diameter= 1.25 # Section diameter expressed in meters.
rcSection= def_simple_RC_section.RCRectangularSection(name= 'beamColumnRCSection', sectionDescr= 'test beam column section', concrType= concr, reinfSteelType= steel, width= 1.45, depth= 0.45, swapReinforcementAxes= True)
## Define XC section
feProblem= xc.FEProblem()
feProblem.title= 'XC JSON encoder/decoder test.'
preprocessor= feProblem.getPreprocessor
xcSection= rcSection.defElasticShearSection3d(preprocessor)

sectionBucklingProperties= EHE_limit_state_checking.SectionBucklingProperties(
    reinforcementFactorZ= 2, # Rectangular section table 43.5.1 of EHE-08.
    sectionDepthZ= diameter,
    reinforcementFactorY= 2, # Rectangular section table 43.5.1 of EHE-08.
    sectionDepthY= diameter,
    Cz= 0.2, # clause 43.1.2 of EHE-08.
    Cy= 0.2, # clause 43.1.2 of EHE-08.
    sectionObject= rcSection)

# Get data in JSON format.
rawData= json.dumps(sectionBucklingProperties, cls=xc_json.XCJSONEncoder)
# Get XC objects from raw data.
xc_json.XCJSONDecoder.preprocessor= preprocessor
xcData= json.loads(rawData, cls=xc_json.XCJSONDecoder)

# Check some results.
testOK= (sectionBucklingProperties == xcData)

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
