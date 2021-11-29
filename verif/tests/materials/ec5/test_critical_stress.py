# -*- coding: utf-8 -*-
'''
Example 2.3.2 of the book "Calcul de structures en bois. Yves Benoit • Bernard Legrand • Vincent Tastet. Deuxième tirage 2008. EYROLLES AFNOR"
'''


from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.ec5 import EC5_materials # Timber

b= 0.050
h= 0.2
L= 5
timber= EC5_materials.C24
section= EC5_materials.RectangularShape(name= 'scc', wood= timber, b= b, h= h)

l_ef= 0.9*L+2*h

sg_m_crit= section.getCriticalStress(l_ef)
E_05= timber.getFifthPercentileValueOfModulusOfElasticityAlongTheGrain()
sg_m_crit_ref= 0.78*E_05*b**2/h/l_ef
ratio1= abs(sg_m_crit-sg_m_crit_ref)/sg_m_crit_ref

lambda_rel_m= section.getBendingRelativeSlenderness(l_ef)
lambda_rel_m_ref= 1.277
ratio2= abs(lambda_rel_m-lambda_rel_m_ref)/lambda_rel_m_ref

k_crit= section.getLateralBucklingBendingStressReductionFactor(l_ef)
k_crit_ref= .602
ratio3= abs(k_crit_ref-k_crit)/k_crit_ref

'''
print('ratio1= ', ratio1)
print('lambda_rel_m= ', lambda_rel_m)
print('ratio2= ', ratio2)
print('k_crit= ', k_crit)
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<.05 and abs(ratio2)<.05 and abs(ratio3)<.05):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
