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
# Permanent load.
G= lcg.newSelfWeightAction(actionName=  'G', actionDescription= 'Self weight.', context= 'road_bridge', safetyFactorSet= safetyFactorSet)
# Railway traffic load.
Q1= lcg.newRailwayTrafficAction(actionName= 'Q1', actionDescription= 'Traffic track 1.', context= 'railway_bridge', combinationFactorsName= 'LM71_alone_uls', safetyFactorSet= safetyFactorSet)
Q2= lcg.newRailwayTrafficAction(actionName= 'Q2', actionDescription= 'Traffic track 2.', context= 'railway_bridge', combinationFactorsName= 'LM71_alone_uls', safetyFactorSet= safetyFactorSet)
# Accidental actions.
A1= lcg.newAccidentalAction(actionName= 'A1', actionDescription= 'Impact', incompatibleActions= ['Q1.*'])

lcg.computeCombinations()
combContainer= lcg.getCombContainer()

# Check results.
refAccidentalCombinations= {'ULSA0':'1.00*G + 1.00*A1', 'ULSA1':'1.00*G + 0.80*Q2 + 1.00*A1'}
testOK= True
for lComb in combContainer.ULS.acc:
    expr= combContainer.ULS.acc[lComb].expr
    refExpr= refAccidentalCombinations[lComb]
    testOK= testOK and (expr==refExpr)

# ## Generate PDF.
# combContainer.exportToPDF(fileName= outputFileName.replace('.json', '.pdf'))

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


