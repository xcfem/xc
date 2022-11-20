# -*- coding: utf-8 -*-
''' Test based on the example in section 3.9.3.1 of the document [Bridge Design to Eurocodes Worked examples](https://eurocodes.jrc.ec.europa.eu/sites/default/files/2022-06/Bridge_Design-Eurocodes-Worked_examples.pdf) page 62.
'''
__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import filecmp
from actions.load_combination_utils import ec0_es # Eurocode 0 Spanish annex.

lcg= ec0_es.combGenerator
# Permanent load.
G= lcg.newPermanentAction(actionName=  'G', actionDescription= 'Self weight.')
# Settlement load.
S= lcg.newSettlementAction(actionName= 'S', actionDescription= 'Settlement.')
# Thermal load.
T= lcg.newThermalActionOnRoadBridge(actionName=  'T', actionDescription= 'Thermal action.')
# Traffic load.
actionTuples= [('TS','Tandem system','road_traffic_loads_gr1a_trucks'), ('UDL','Uniformly distributed load.','road_traffic_loads_gr1a_udl')]
actionGroup= lcg.newActionGroup(family= 'variables', actionTuples= actionTuples, partialSafetyFactorsName= 'road_traffic')

lcg.computeCombinations()

# Write results.
outputFileName= 'ec0_traffic_groups.py'
lcg.writeXCLoadCombinations(outputFileName= outputFileName)

# Get current path.
pth= os.path.dirname(__file__)
if(not pth):
    pth= '.'
refFile= pth+'/../../aux/reference_files/ref_'+outputFileName
comparisonOK= filecmp.cmp(refFile, outputFileName, shallow=False)
    
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if comparisonOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
os.remove(outputFileName) # Your garbage, you clean it.
