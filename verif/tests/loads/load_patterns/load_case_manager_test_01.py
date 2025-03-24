# -*- coding: utf-8 -*-
'''Check LoadCaseManager class getDict/setFromDict methods.'''

__author__= "Ana Ortega (AO_O)  Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, AO_O  LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es l.pereztato@gmail.com"

import xc
from actions import load_cases

lcDict= dict()
lcDict['G1']= 'Self weight'
lcDict['G2']= 'Dead load'
lcDict['G3']= 'Shrinkage'
lcDict['G4']= 'Creep'
lcDict['Q2']= 'Wind'
lcDict['Q1a1']= 'Locomotive position: 1'
lcDict['Q1a2']= 'Locomotive position: 2'
lcDict['Q1a3']= 'Locomotive position: 3'
lcDict['Q1a4']= 'Locomotive position: 4'
lcDict['Q1a5']= 'Locomotive position: 5'
lcDict['Q1a6']= 'Locomotive position: 6'
lcDict['Q1a7']= 'Locomotive position: 7'
lcDict['Q1a8']= 'Locomotive position: 8'
lcDict['Q1a9']= 'Locomotive position: 9'
lcDict['Q1a10']= 'Locomotive position: 10'
lcDict['Q1a11']= 'Locomotive position: 11'
lcDict['Q3A1']= 'Temperature increment'
lcDict['Q3A2']= 'Temperatur decrement'
lcDict['A1']= 'Derailment situation I'
lcDict['A2']= 'Derailment situation II'
lcDict['A3']= 'Impacto vehículo carretero'

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
lcManagerA= load_cases.LoadCaseManager(preprocessor= preprocessor)

loadCaseNames= list(lcDict.keys())
lcManagerA.defineSimpleLoadCases(loadCaseNames)

for key in lcDict:
    cLC= lcManagerA.setCurrentLoadCase(key)
    cLC.description= lcDict[key]

dctA= lcManagerA.getDict()

lcManagerB= load_cases.LoadCaseManager(preprocessor= preprocessor)
lcManagerB.setFromDict(dctA)
dctB= lcManagerB.getDict()

testOK= (dctA==dctB)

'''
print(dctA)
print(dctB)
print(testOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
