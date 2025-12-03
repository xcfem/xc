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

lcg= ec0_es.bridgeCombGenerator
safetyFactorSet= 'B' # Table A2.4(B)
context= 'road_bridge'
# Permanent load.
G= lcg.newSelfWeightAction(actionName=  'G', actionDescription= 'Self weight.', context= context, safetyFactorSet= safetyFactorSet)
# Settlement load.
S= lcg.newSettlementAction(actionName= 'S', actionDescription= 'Settlement.', context= context, safetyFactorSet= safetyFactorSet)
# Hidrostatic pressure.
W= lcg.newHydrostaticPressureAction(actionName= 'W', actionDescription= 'Hydrostatic pressure', context= context, safetyFactorSet= safetyFactorSet)
# Thermal load.
T= lcg.newThermalAction(actionName=  'T', actionDescription= 'Thermal action.', context= context, safetyFactorSet= safetyFactorSet)
# Uniform traffic load.
TL= lcg.newUniformLoadAction(actionName= 'TL', actionDescription= 'Traffic load.', safetyFactorSet= safetyFactorSet)

lcg.computeCombinations()

# Write results.
outputFileName= 'ec0_road_bridge_context.py'
lcg.write_xc_load_combinations(outputFileName= outputFileName)

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
if(comparisonOK):
    os.remove(outputFileName) # Your garbage, you clean it.
