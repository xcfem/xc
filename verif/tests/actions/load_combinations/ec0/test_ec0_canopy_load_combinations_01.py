# -*- coding: utf-8 -*-
''' Compute load combinations for loads on an canopy according to Eurocode 0.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import filecmp
import loadCombinations
from actions.load_combination_utils import ec0_es # Eurocode 0 Spanish annex.

lcg= ec0_es.BuildingCombGenerator(altitude= 700.0)

safetyFactorSet= 'B' # Table A1.2(B)
designSituation= 'STR'
# Permanent load.
G1= lcg.newPermanentAction(actionName= 'G1', actionDescription= 'Peso propio.', safetyFactorSet= safetyFactorSet, designSituation= designSituation)
G2= lcg.newPermanentAction(actionName=  'G2', actionDescription= 'Carga muerta.', safetyFactorSet= safetyFactorSet, designSituation= designSituation)

windLoadArrangements= ['Q1a', 'Q1b', 'Q1c', 'Q1d', 'Q1e', 'Q1f']
windLoadActions= list()
for windLoadArrangement in windLoadArrangements:
    actionName= windLoadArrangement
    actionDescription= 'Wind load '+actionName
    windLoadActions.append(lcg.newWindLoadAction(actionName= actionName, actionDescription= actionDescription, safetyFactorSet= safetyFactorSet, designSituation= designSituation, incompatibleActions= ['Q1.*']))

Q2= lcg.newSnowLoadAction(actionName=  'Q2', actionDescription= 'Snow', safetyFactorSet= safetyFactorSet, designSituation= designSituation)
A= lcg.newAccidentalAction(actionName=  'A', actionDescription= 'Accidental')

lcg.computeCombinations()
combContainer= lcg.getCombContainer()
outputFileName= 'canopy_load_combinations_ec0_01.json'
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
