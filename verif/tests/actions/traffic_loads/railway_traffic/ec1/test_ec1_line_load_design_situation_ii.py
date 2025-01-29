# -*- coding: utf-8 -*-
''' Uniformly distributed equivalent line load for Design Situation II

Test based on the example 6.2 of the book Designers’ Guide to Eurocode 1: Actions on Bridges. J.-A. Calgaro, M. Tschumi and H. Gulvanessian Thomas Telford 2010 https://www.icevirtuallibrary.com/doi/pdf/10.1680/dgeab.31586.fm
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.railway_traffic import EC1_rail_load_models

locomotiveLM1= EC1_rail_load_models.get_locomotive_LM1()
trainLoadModel= EC1_rail_load_models.TrainLoadModel(locomotive= locomotiveLM1, uniformLoad= 80e3, dynamicFactor= 1.0, classificationFactor= 1.33)

# Compute some values
qA2d8m= trainLoadModel.getUniformLineLoadForDesignSituationII(bridgeLength= 8.0)
qA2d30m= trainLoadModel.getUniformLineLoadForDesignSituationII(bridgeLength= 30.0)

# Check results.
ratio1= abs(qA2d8m-262.542e3)/262.542e3
ratio2= abs(qA2d30m-194.3928e3)/194.3928e3

'''
print('qA2d8m= ', qA2d8m/1e3, 'kN/m')
print("ratio1= ", ratio1)
print('qA2d8m= ', qA2d30m/1e3, 'kN/m')
print("ratio2= ", ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((ratio1<1e-6) and (ratio2<1e-6)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
