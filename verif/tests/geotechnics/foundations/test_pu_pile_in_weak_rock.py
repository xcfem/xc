# -*- coding: utf-8 -*-
'''Calculation of ultimate reaction (force per unit length) of the rock
        according to Reese, L. C. 1997. “Analysis of laterally loaded piles in 
        weak rock,” Journal of Geotechnical and Geoenvironmental Engineering, 
        Vol.123(11), ASCE, Reston, VA.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
from geotechnics.foundations import weak_rock as wr

qu= 40220000.0 # uniaxial unconfined compressive strength of intact rock (Pa).
D= 1.8 # pile diameter.
RQD= 50 # Rock Quality Designation expressed as a percentage.
zi= [0.0, 0.39999999999997726, 1.3999999999999773, 2.3999999999999773, 3.3999999999999773, 4.399999999999977, 5.399999999999977, 6.399999999999977, 7.399999999999977, 8.399999999999977, 9.399999999999977, 10.399999999999977, 11.399999999999977, 12.399999999999977, 13.399999999999977, 14.399999999999977, 15.399999999999977, 16.399999999999977, 17.399999999999977]

refPu_i= [0.0, 63279.46666666582e3, 100818.13333333247e3, 138356.79999999914e3, 175895.46666666583e3, 213434.13333333252e3, 250972.79999999914e3, 250972.8e3, 250972.8e3, 250972.8e3, 250972.8e3, 250972.8e3, 250972.8e3, 250972.8e3, 250972.8e3, 250972.8e3, 250972.8e3, 250972.8e3, 250972.8e3]
pu_i= list()

err= 0.0
for i, z in enumerate(zi):
    pu= wr.pu_weak_rock_reese_1977(zr= z, sigma_ci= qu, D= D, RQD= RQD)
    ref_pu= refPu_i[i]
    err+= (pu-ref_pu)**2

# err= math.sqrt(err)

print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
