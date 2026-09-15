# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"


from actions.load_combination_utils import utils

loadCombinations= dict()
loadCombinations['ELU01']= '1.35*G+1.35*CP+1.5*SCNmax+1.5*LazoCentr+0.9*Viento'
loadCombinations['ELU02']= '1.35*G+1.35*CP+1.5*SCMtmax+1.5*LazoCentr+0.9*Viento'
loadCombinations['ELU03']= '1.35*G+1.35*CP+1.2*SCNmax+1.2*LazoCentr+1.5*Viento'
loadCombinations['ELU04']= '1.35*G+1.35*CP+1.2*SCMtmax+1.2*LazoCentr+1.5*Viento'
loadCombinations['ELU05']= '1.0*G+1.0*CP+0.15*SCMtmax+1.0*SismoLn+0.3*SismoTr+0.3*SismoVert'
loadCombinations['ELU06']= '1.0*G+1.0*CP+0.15*SCMtmax+1.0*SismoLn+0.3*SismoTr-0.3*SismoVert'
loadCombinations['ELU07']= '1.0*G+1.0*CP+0.15*SCMtmax+0.3*SismoLn+1.0*SismoTr+0.3*SismoVert'
loadCombinations['ELU08']= '1.0*G+1.0*CP+0.15*SCMtmax+0.3*SismoLn+1.0*SismoTr-0.3*SismoVert'
loadCombinations['ELU09']= '1.0*G+1.0*CP+0.5*Viento+1.0*Descarr'

result, loadCaseNames= utils.extract_load_factor_pairs(loadCombinations)

refResult= {'ELU01': {'G': 1.35, 'CP': 1.35, 'SCNmax': 1.5, 'LazoCentr': 1.5, 'Viento': 0.9}, 'ELU02': {'G': 1.35, 'CP': 1.35, 'SCMtmax': 1.5, 'LazoCentr': 1.5, 'Viento': 0.9}, 'ELU03': {'G': 1.35, 'CP': 1.35, 'SCNmax': 1.2, 'LazoCentr': 1.2, 'Viento': 1.5}, 'ELU04': {'G': 1.35, 'CP': 1.35, 'SCMtmax': 1.2, 'LazoCentr': 1.2, 'Viento': 1.5}, 'ELU05': {'G': 1.0, 'CP': 1.0, 'SCMtmax': 0.15, 'SismoLn': 1.0, 'SismoTr': 0.3, 'SismoVert': 0.3}, 'ELU06': {'G': 1.0, 'CP': 1.0, 'SCMtmax': 0.15, 'SismoLn': 1.0, 'SismoTr': 0.3, 'SismoVert': -0.3}, 'ELU07': {'G': 1.0, 'CP': 1.0, 'SCMtmax': 0.15, 'SismoLn': 0.3, 'SismoTr': 1.0, 'SismoVert': 0.3}, 'ELU08': {'G': 1.0, 'CP': 1.0, 'SCMtmax': 0.15, 'SismoLn': 0.3, 'SismoTr': 1.0, 'SismoVert': -0.3}, 'ELU09': {'G': 1.0, 'CP': 1.0, 'Viento': 0.5, 'Descarr': 1.0}}
refLoadCaseNames= {'Viento', 'Descarr', 'SCNmax', 'LazoCentr', 'SismoTr', 'CP', 'SismoVert', 'SCMtmax', 'SismoLn', 'G'}

testOK= (result==refResult) and(loadCaseNames==refLoadCaseNames)

# print(result)
# print(loadCaseNames)
# print(testOK)

import os
fname= os.path.basename(__file__)
if (testOK):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
