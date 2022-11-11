# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"


from actions.load_combination_utils import utils

combTest= "1.35*G1 + 1.35*G2 + 1.00*G3 + 1.00*P1 + 1.35*Q1a1 + 0.90*Q32"
combTerms= ['G1', 'P1', 'Q32']
part1, part2= utils.splitCombination(combTest, combTerms)

testOk= (part1=='1.35*G1+1.0*P1+0.9*Q32') and (part2=='1.35*G2+1.0*G3+1.35*Q1a1')


'''        
print(part1)
print(part2)
print(testOk)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (testOk):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

