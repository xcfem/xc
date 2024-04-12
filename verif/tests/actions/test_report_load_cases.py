# -*- coding: utf-8 -*-
''' Test LaTeX report generation of load cases.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega.ort@gmail.com l.pereztato@gmail.com"

import os
import xc
import filecmp
from actions import load_cases

lcDict= dict()
lcDict['G1']= 'Self weight'
lcDict['G2']= 'Dead load'
lcDict['G3']= 'Shrinkage'
lcDict['G4']= 'Creep'
lcDict['Q2']= 'Transverse wind'
lcDict['Q1a1']= 'Locomotive at position: 1'
lcDict['Q1a2']= 'Locomotive at position: 2'
lcDict['Q1a3']= 'Locomotive at position: 3'
lcDict['Q1a4']= 'Locomotive at position: 4'
lcDict['Q1a5']= 'Locomotive at position: 5'
lcDict['Q1a6']= 'Locomotive at position: 6'
lcDict['Q1a7']= 'Locomotive at position: 7'
lcDict['Q1a8']= 'Locomotive at position: 8'
lcDict['Q1a9']= 'Locomotive at position: 9'
lcDict['Q1a10']= 'Locomotive at position: 10'
lcDict['Q1a11']= 'Locomotive at position: 11'
lcDict['Q3A1']= 'Uniform temperature increment'
lcDict['Q3A2']= 'Uniform temperature decrement'
lcDict['A2']= 'Derailment situation II'

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
loadCaseManager= load_cases.LoadCaseManager(preprocessor= preprocessor)

loadCaseNames= list(lcDict.keys())
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

for key in lcDict:
    cLC= loadCaseManager.setCurrentLoadCase(key)
    cLC.description= lcDict[key]


texFileName= '/tmp/report_load_cases.tex'
loadCaseManager.exportToLatex(fileName= texFileName)
# loadCaseManager.exportToPDF(fileName= './report_load_cases.pdf')

## Get current path.
pth= os.path.dirname(__file__)
if(not pth):
    pth= '.'
texFileNameRef= pth+'/../aux/reference_files/ref_report_load_cases.tex'

# Check result.
comparisonOK= filecmp.cmp(texFileNameRef, texFileName, shallow=False)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if comparisonOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
os.remove(texFileName) # Your garbage, you clean it.
