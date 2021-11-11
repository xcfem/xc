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

ratio= abs(sg_m_crit-sg_m_crit_ref)/sg_m_crit_ref

'''
print('ratio= ', ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<.05):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
