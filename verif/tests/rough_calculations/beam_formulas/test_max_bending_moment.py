# -*- coding: utf-8 -*-
''' Maximum bending moment under load model 1 of EC1-2:2003. Inspired on the
example from: http://www.bridgedesign.org.uk/tutorial/lm1-lm3-example.php
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

from rough_calculations import ng_simple_beam
import numpy as np
from scipy.optimize import minimize

deckSpan= 34.0 # deck span.
axleLoad= 300e3 # Vertical load of each axle.

sb= ng_simple_beam.SimpleBeam(l= deckSpan)

# Compute bending moment.
def getBendingMoment(a, x):
    ''' Return the bending moment due to both axles.

    :param a: position of the centroid.
    :param x: position of the section to compute the bending moment.
    '''
    posRearAxle= a-0.6
    posFrontAxle= a+0.6
    retval= sb.getBendingMomentUnderConcentratedLoad(P= -axleLoad, a= posRearAxle, x= x)
    retval+= sb.getBendingMomentUnderConcentratedLoad(P= -axleLoad, a= posFrontAxle, x= x)
    return retval

def func_to_minimize(v):
    return -getBendingMoment(a= v[0], x= v[1])

first_guess= [deckSpan/2.0, deckSpan/2.0]
#first_guess= [deckSpan/2-0.3, deckSpan/2.0+0.3] That's not true.
result= minimize(func_to_minimize, first_guess)
mMax= -result.fun
aMax= result.x[0]
xMax= result.x[1]
ratio1= abs(mMax-4920e3)/4920e3
ratio2= abs(aMax-deckSpan/2.0)/(deckSpan/2.0)
ratio3= abs(xMax-deckSpan/2.0)/(deckSpan/2.0)

'''
print(ratio1)
print(ratio2)
print(result.success)
print(result.message)
print(aMax, xMax, mMax/1e3)

xi= np.linspace(0, deckSpan, 100)
yi= list()
for x in xi:
    yi.append(getBendingMoment(a= aMax, x=x)/1e3)
    
import matplotlib.pyplot as plt
plt.plot(xi, yi)
plt.show()
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-12 and abs(ratio2)<1e-12 and abs(ratio3)<1e-12 and result.success):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
