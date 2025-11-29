# -*- coding: utf-8 -*-
''' Trivial test that checks that newDeadLoadAction method works as intended.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

from math import sqrt
import os
import filecmp
import loadCombinations
from actions.load_combination_utils import ec0_es # Eurocode 0 Spanish annex.

lcg= ec0_es.bridgeCombGenerator
safetyFactorSet= 'B' # Table A2.4(B)
# Permanent load.
G2= lcg.newPrestressingAction(actionName=  'G5', actionDescription= 'Prestressing', safetyFactorSet= safetyFactorSet, p2Type= True, localVerifications= True)
partialSafetyFactors= G2.getPartialSafetyFactors()
ulsPartialSafetyFactors= partialSafetyFactors.getULSPartialSafetyFactors
slsPartialSafetyFactors= partialSafetyFactors.getSLSPartialSafetyFactors

ulsFavourable= ulsPartialSafetyFactors.favourable
ulsUnfavourable= ulsPartialSafetyFactors.unfavourable
ulsFavourableAccidental= ulsPartialSafetyFactors.favourableAccidental
ulsUnfavourableAccidental= ulsPartialSafetyFactors.unfavourableAccidental
slsFavourable= slsPartialSafetyFactors.favourable
slsUnfavourable= slsPartialSafetyFactors.unfavourable

error= (ulsFavourable-1)**2
error+= (ulsUnfavourable-1.35)**2
error+= (ulsFavourableAccidental-1)**2
error+= (ulsUnfavourableAccidental-1)**2
error+= (slsFavourable-1)**2
error+= (slsUnfavourable-1)**2
error= sqrt(error)

'''
print('ulsFavourable= ', ulsFavourable)
print('ulsUnfavourable= ', ulsUnfavourable)
print('ulsFavourableAccidental= ', ulsFavourableAccidental)
print('ulsUnfavourableAccidental= ', ulsUnfavourableAccidental)
print('slsFavourable= ', slsFavourable)
print('slsUnfavourable= ', slsUnfavourable)
print('error= ', error)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error< 1e-5):
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


