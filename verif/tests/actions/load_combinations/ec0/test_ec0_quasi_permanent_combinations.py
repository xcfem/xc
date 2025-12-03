# -*- coding: utf-8 -*-
''' Check quasi-permanent combinations.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
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
# Thermal load.
T= lcg.newThermalAction(actionName=  'T', actionDescription= 'Thermal action.', context= context, safetyFactorSet= safetyFactorSet)

lcg.computeCombinations()

combContainer= lcg.getCombContainer()

# Write results.
fname= os.path.basename(__file__)
outputFileName= fname.replace('.py', '.json')
# combContainer.exportToPDF(fileName= outputFileName.replace('.json', '.pdf'), small= True, superTabular= True)
outputPath= '/tmp/'+outputFileName    
combContainer.writeJSON(outputFileName= outputPath)

# Get current path.
pth= os.path.dirname(__file__)
if(not pth):
    pth= '.'
refFile= pth+'/../../../aux/reference_files/ref_'+outputFileName
comparisonOK= filecmp.cmp(refFile, outputPath, shallow=False)
    
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if comparisonOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
if comparisonOK:
    os.remove(outputPath) # Your garbage, you clean it.
