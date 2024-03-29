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
G= lcg.newPermanentAction(actionName=  'G', actionDescription= 'Self weight.', context= 'road_bridge')
# Settlement load.
S= lcg.newSettlementAction(actionName= 'S', actionDescription= 'Settlement.', context= 'road_bridge')
# Hidrostatic pressure.
W= lcg.newHydrostaticPressureAction(actionName= 'W', actionDescription= 'Hydrostatic pressure', context= 'road_bridge')
# Thermal load.
T= lcg.newThermalAction(actionName=  'T', actionDescription= 'Thermal action.', context= 'road_bridge')
# Uniform traffic load.
TL= lcg.newUniformLoadAction(actionName= 'TL', actionDescription= 'Traffic load.')

lcg.computeCombinations()

# Write results.
outputFileName= 'ec0_road_bridge_context.py'
lcg.writeXCLoadCombinations(outputFileName= outputFileName)

# Get current path.
pth= os.path.dirname(__file__)
if(not pth):
    pth= '.'
refFile= pth+'/../../../aux/reference_files/ref_'+outputFileName
comparisonOK= filecmp.cmp(refFile, outputFileName, shallow=False)
    
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if comparisonOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
os.remove(outputFileName) # Your garbage, you clean it.