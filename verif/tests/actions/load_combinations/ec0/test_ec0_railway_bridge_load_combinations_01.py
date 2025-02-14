# -*- coding: utf-8 -*-
''' Compute load combinations for railway loads according to Eurocode 0.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
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
G2= lcg.newDeadLoadAction(actionName=  'G2', actionDescription= 'Carga muerta.', safetyFactorSet= safetyFactorSet)
G3= lcg.newRheologicalAction(actionName=  'G3', actionDescription= 'Retracción.', safetyFactorSet= safetyFactorSet)

# Traffic load.
Q1= lcg.newRailwayTrafficAction(actionName= 'Q1', actionDescription= 'LM1', combinationFactorsName= 'LM71_alone_uls', incompatibleActions= ['Q1.*'], safetyFactorSet= safetyFactorSet)

# Wind load.
Q2= lcg.newWindAction(actionName= 'Q2', actionDescription= 'Viento transversal.', dependsOn= None, incompatibleActions= None, context= 'railway_bridge', safetyFactorSet= safetyFactorSet)

# Thermal load.
Q3A1= lcg.newThermalAction(actionName= 'Q3A1', actionDescription= 'Incremento de temperatura.', dependsOn= None, incompatibleActions= ['Q3A.*'], context= 'railway_bridge', notDeterminant= True, safetyFactorSet= safetyFactorSet) # Not determinant load.
Q3A2= lcg.newThermalAction(actionName= 'Q3A2', actionDescription= 'Decremento de temperatura.', dependsOn= None, incompatibleActions= ['Q3A.*'], context= 'railway_bridge', safetyFactorSet= safetyFactorSet)

# Derailment. Design situation II
A2= lcg.newAccidentalAction(actionName= 'A2', actionDescription= 'Descarrilamiento situación II', dependsOn= None, incompatibleActions= None, context= None)

lcg.computeCombinations()
combContainer= lcg.getCombContainer()
outputFileName= 'railway_load_combinations_ec0_01.json'
outputPath= '/tmp/'+outputFileName
    
combContainer.writeJSON(outputFileName= outputPath)

# Get current path.
pth= os.path.dirname(__file__)
if(not pth):
    pth= '.'

refFile= pth+'/../../../aux/reference_files/ref_'+outputFileName

# Check results.
comparisonOK= filecmp.cmp(refFile, outputPath, shallow=False)

## Generate PDF.
# combContainer.exportToPDF(fileName= outputFileName.replace('.json', '.pdf'))

# Ouput result test.
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if comparisonOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputPath) # clean up after yourself.
