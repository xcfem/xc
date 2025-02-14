# -*- coding: utf-8 -*-
''' Write load combinations in CSV format.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import filecmp
from actions import combinations

# Create a container for load combinations.
combContainer= combinations.CombContainer()

# Read load combinations from file.
## Get current path.
pth= os.path.dirname(__file__)
if(not pth):
    pth= '.'
jsonFileName= pth+'/../../../aux/reference_files/ref_railway_load_combinations_ec0_01.json'

combContainer.readFromJSON(inputFileName= jsonFileName)

# Export to CSV.
csvFileNameRef= jsonFileName.replace('.json', '.csv')
## Extract file name
csvFileName= os.path.basename(csvFileNameRef)
csvFileName= csvFileName.replace('ref_','./')
## Write LaTeX file
with open(csvFileName, 'w') as csvFile:
    combContainer.exportToCSV(os= csvFile)

# Check result.
comparisonOK= filecmp.cmp(csvFileNameRef, csvFileName, shallow=False)
    
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if comparisonOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
os.remove(csvFileName) # Your garbage, you clean it.
