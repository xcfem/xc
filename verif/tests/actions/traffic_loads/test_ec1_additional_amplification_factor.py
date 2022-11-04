# -*- coding: utf-8 -*-
''' Simple test checking the additional amplification factor according to 
    the expression 4.7 of the clause 4.6.1 (6) of Eurocode 1 part 2.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import numpy as np
from actions.roadway_traffic import EC1_load_models

xi= np.linspace(0.0,12.0, num= 13)
yi= list()
for x in xi:
    yi.append(EC1_load_models.getAdditionalAmplificationFactor(D= x))

refYi= [1.3, 1.25, 1.2, 1.15, 1.1, 1.05, 1.0, 1, 1, 1, 1, 1, 1]
err= 0.0
for y, yR in zip(yi, refYi):
    err+= (y-yR)**2
err= math.sqrt(err)

'''
print(xi, yi, err)

import matplotlib.pyplot as plt
plt.title("Additional amplification factor.")
plt.xlabel("D")
plt.ylabel(r'$\Delta \varphi_{fat}$')
plt.plot(xi, yi)
plt.grid()
plt.show()
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-11):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
