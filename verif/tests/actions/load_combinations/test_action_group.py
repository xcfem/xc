# -*- coding: utf-8 -*-
''' Trivial test for actions in loadCombinations module.'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import loadCombinations
from actions.load_combination_utils import utils
from misc_utils import log_messages as lmsg


actionTuples= [('TestActionA','Test action description A.','cfA'), ('TestActionB','Test action description B.','cfB')]

# Populate factors container.
factors= loadCombinations.Factors() # Empty factors container.
## Populate partial safety factors.
partial_safety_factors= factors.getPartialSafetyFactors()
partial_safety_factors['test_psf']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
## Populate combination factors.
combination_factors= factors.getCombinationFactors()
combination_factors.insert('cfA',loadCombinations.CombinationFactors(0.75,0.75,0.0))
combination_factors.insert('cfB',loadCombinations.CombinationFactors(0.40,0.40,0.0))

# Create combination generator.
combGenerator= utils.CombGenerator(combGeneratorName= 'Test', factors= factors)

# Define action group.
actionGroup= combGenerator.newActionGroup(family= 'variable', actionTuples= actionTuples, partialSafetyFactorsName= 'test_psf')

actionGroup.relationships.appendIncompatible('IncompA.*')
actionGroup.relationships.appendIncompatible('IncompB.*')
actionGroup.relationships.appendMain('MainAction1')
actionGroup.relationships.appendMain('MainAction2')

actionGroupData= actionGroup.getDict()
refDict= {'ownerAddress': 0, 'className': 'cmb_acc::GroupActionWrapper', 'actions': {'TestActionA': {'ownerAddress': 0, 'className': 'cmb_acc::ActionRepresentativeValues', 'name': 'TestActionA', 'description': 'Test action description A.', 'relations': {'ownerAddress': 0, 'className': 'cmb_acc::ActionRelationships', 'incompatibles': ['IncompA.*', 'IncompB.*'], 'main_actions': ['MainAction1', 'MainAction2'], 'contiene_incomp': False}, 'nodet': False, 'f_pond': 1.0}, 'TestActionB': {'ownerAddress': 0, 'className': 'cmb_acc::ActionRepresentativeValues', 'name': 'TestActionB', 'description': 'Test action description B.', 'relations': {'ownerAddress': 0, 'className': 'cmb_acc::ActionRelationships', 'incompatibles': [], 'main_actions': [], 'contiene_incomp': False}, 'nodet': False, 'f_pond': 1.0}}}
actionGroupData['ownerAddress']= 0 # Owner address can't be the same.
ok= (actionGroupData==refDict)

'''
print(actionGroupData, ok)
'''

import os
fname= os.path.basename(__file__)
if (ok):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
