# -*- coding: utf-8 -*-
''' Compute load combinations for railway loads according to Eurocode 0. Example with hydrostatic and construction loads.'''



__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import filecmp
import loadCombinations
from actions.load_combination_utils import ec0_es # Eurocode 0 Spanish annex.

lcg= ec0_es.bridgeCombGenerator
safetyFactorSet= 'B' # Table A2.4(B)
# Permanent load.
G1= lcg.newSelfWeightAction(actionName=  'G1', actionDescription= 'Peso propio.', safetyFactorSet= safetyFactorSet)
G2= lcg.newEarthPressureAction(actionName=  'G2', actionDescription= 'Empuje tierras.', safetyFactorSet= safetyFactorSet)
# Water pressure load.
Q1h= lcg.newHydrostaticPressureAction(actionName=  'Q1h', actionDescription= 'Presíón hidrostática (marco medio lleno).', safetyFactorSet= safetyFactorSet, incompatibleActions= ['Q1f'])
Q1f= lcg.newHydrostaticPressureAction(actionName=  'Q1f', actionDescription= 'Presíón hidrostática (marco lleno).', safetyFactorSet= safetyFactorSet, incompatibleActions= ['Q1h'])
# Traffic load.
Q2= lcg.newRailwayTrafficAction(actionName= 'Q2', actionDescription= 'LM1', combinationFactorsName= 'LM71_alone_uls', incompatibleActions= ['Q2.*'], safetyFactorSet= safetyFactorSet)
# Construction load.
Q3= lcg.newConstructionAction(actionName= 'Q3', actionDescription= 'Construction load', incompatibleActions= ['Q1.*', 'Q2.*'], context= 'railway_bridge', safetyFactorSet= safetyFactorSet)

lcg.computeCombinations()
combContainer= lcg.getCombContainer()
outputFileName= 'railway_load_combinations_ec0_02.json'
outputPath= '/tmp/'+outputFileName
    
combContainer.writeJSON(outputFileName= outputPath)

# Get current path.
pth= os.path.dirname(__file__)
if(not pth):
    pth= '.'

refFile= pth+'/../../../aux/reference_files/ref_'+outputFileName

## Generate PDF.
# combContainer.exportToPDF(fileName= outputFileName.replace('.json', '.pdf'))

# Check results.
comparisonOK= filecmp.cmp(refFile, outputPath, shallow=False)

# Ouput result test.
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if comparisonOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
if comparisonOK:
    os.remove(outputPath) # clean up after yourself.

