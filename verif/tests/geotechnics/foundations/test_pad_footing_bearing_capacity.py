# -*- coding: utf-8 -*-
'''
Example based on the example 10.1 of the publication: Decoding Eurocode 7 by 
Andrew Bond and Andrew Harris.

https://www.decodingeurocode7.com/Chapter%2010/index.html
'''

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega.ort.com"

import math
from scipy.constants import g
from geotechnics import frictional_soil as fs

B= 1.5 # breadth.
L= 2.5 # length.
d= 0.5 # depth.

WGk= L*B*d*25e3
VGk= 800e3
VQk= 450e3

foundationSoil= fs.FrictionalSoil(phi= math.radians(35), rho= 18e3/g, gammaMPhi= 1.0)

# Design approach 2.
Vd= 1.35*(WGk+VGk)+1.5*VQk  # Design action.
qEd= Vd/B/L
## Bearing resistance.
q= foundationSoil.gamma()*d # dverburden at foundation base
qu_k= foundationSoil.qu(q= q, D= d, Beff= B, Leff= L, Vload= Vd, HloadB= 0.0, HloadL= 0.0, NgammaCoef= 1.75)
gammaR= 1.4
qRd= qu_k/gammaR
CF= qEd/qRd
refCF= 0.7519644365538797
ratio= abs(refCF-CF)/refCF

'''
print('WGk= ', WGk/1e3, 'kN')
print('Vd= ', Vd/1e3, 'kN')
print('qEd= ', qEd/1e3, 'kPa')
print('q= ', q/1e3, 'kPa')
print('qu_k= ', qu_k/1e3, 'kPa')
print('qRd= ', qRd/1e3, 'kPa')
print('CF= ', CF)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
