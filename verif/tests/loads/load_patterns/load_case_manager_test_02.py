# -*- coding: utf-8 -*-
'''Check LoadCaseManager class getDict/setFromDict methods. Read data from
   JSON file.'''

__author__= "Ana Ortega (AO_O)  Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, AO_O  LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es l.pereztato@gmail.com"

import os
import json
import xc
from actions import load_cases

# Read dictionary from JSON file.
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print("pth= ", pth)
loadCaseMgrData= pth+'/../../aux/load_patterns/load_case_manager_test_02.json'

with open(loadCaseMgrData, 'r') as f:
    loadCaseMgrDict= json.load(f)

# Create LoadCaseManager object and populate it with the dictionary.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
loadCaseMgr= load_cases.LoadCaseManager(preprocessor= preprocessor)
loadCaseMgr.setFromDict(loadCaseMgrDict)

# Check results.
codesAndDescriptions= loadCaseMgr.getCodesAndDescriptions()
codesAndDescriptionsRef= [('G1', 'Self weight'), ('G2', 'Dead load'), ('G3', 'Shrinkage'), ('G4', 'Creep'), ('Q2', 'Wind'), ('Q1a1', 'Locomotive position: 1'), ('Q1a2', 'Locomotive position: 2'), ('Q1a3', 'Locomotive position: 3'), ('Q1a4', 'Locomotive position: 4'), ('Q1a5', 'Locomotive position: 5'), ('Q1a6', 'Locomotive position: 6'), ('Q1a7', 'Locomotive position: 7'), ('Q1a8', 'Locomotive position: 8'), ('Q1a9', 'Locomotive position: 9'), ('Q1a10', 'Locomotive position: 10'), ('Q1a11', 'Locomotive position: 11'), ('Q3A1', 'Temperature increment'), ('Q3A2', 'Temperatur decrement'), ('A1', 'Derailment situation I'), ('A2', 'Derailment situation II'), ('A3', 'Impact')]

# print(codesAndDescriptions)

testOK= (codesAndDescriptions == codesAndDescriptionsRef)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


