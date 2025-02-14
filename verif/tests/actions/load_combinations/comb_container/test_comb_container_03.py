# -*- coding: utf-8 -*-
''' Read load combinations from CSV file.'''

import os
import filecmp
from actions import combinations

# Create a container for load combinations.
combContainer= combinations.CombContainer()

# Read load combinations from file.
# Read load combinations from file.
## Get current path.
pth= os.path.dirname(__file__)
if(not pth):
    pth= '.'
csvFileName= pth+'/../../../aux/reference_files/ref_railway_load_combinations_ec0_01.csv'

combContainer.readFromCSV(inputFileName= csvFileName)
combContainer.renumber(start= 1000)

texFileNameRef= csvFileName.replace('.csv', '_1000.tex')
## Extract file name
texFileName= os.path.basename(texFileNameRef)
texFileName= texFileName.replace('ref_','./')
## Write LaTeX file
combContainer.exportToLatex(fileName= texFileName)

# ## Generate PDF.
# pdfFileName= csvFileName.replace('.csv','.pdf')
# combContainer.exportToPDF(fileName= pdfFileName)

# Check result.
comparisonOK= filecmp.cmp(texFileNameRef, texFileName, shallow=False)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if comparisonOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
os.remove(texFileName) # Your garbage, you clean it.

