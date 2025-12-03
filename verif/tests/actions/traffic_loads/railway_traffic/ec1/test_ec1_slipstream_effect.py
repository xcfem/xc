# -*- coding: utf-8 -*-
''' Test values of slipstream effect according to EC1-2 clause 6.6.

A slipstream is a region behind a moving object in which a wake of fluid
(typically air or water) is moving at velocities comparable to that of the
moving object, relative to the ambient fluid through which the object is 
moving.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.railway_traffic import ec1_slipstream_effect

# Compute some values
# v: speed of the train.
# a_g: distance from the track axis to the surface of the structure (m).
# h_g: distance from top of rail level to the underside of the structure (m).

# Characteristic value of  the aerodinamic action for simple
# vertical surfaces parallel to the track (q1k) according to
# figure 6.22 of EC1-2.
q1k_300_2dot3= ec1_slipstream_effect.q1k(v= 300/3.6, a_g= 2.3)
q1k_300_6dot3= ec1_slipstream_effect.q1k(v= 300/3.6, a_g= 6.0)

# Characteristic value of  the aerodinamic action for simple
# horizontal surfaces above the track (q2k) according to
# figure 6.23 of EC1-2.
q2k_300_5= ec1_slipstream_effect.q2k(v= 300/3.6, h_g= 5.0)
q2k_300_9= ec1_slipstream_effect.q2k(v= 300/3.6, h_g= 9.0)

# Characteristic value of  the aerodinamic action for simple
# horizontal surfaces adjacent to the track (q3k) according to
# figure 6.24 of EC1-2.
q3k_200_2= ec1_slipstream_effect.q3k(v= 200/3.6, a_g= 2.0)
q3k_200_6= ec1_slipstream_effect.q3k(v= 200/3.6, a_g= 6.0)

# Check results.
ratio1= abs(q1k_300_2dot3-1.755501356315947e3)/1.755501356315947e3
ratio2= abs(q1k_300_6dot3-0.36458333333333326e3)/0.36458333333333326e3
ratio3= abs(q2k_300_5-2.4696901931363495e3)/2.4696901931363495e3
ratio4= abs(q2k_300_9-0.31447376033387586e3)/0.31447376033387586e3
ratio5= abs(q3k_200_2-0.6004943987197074e3)/0.6004943987197074e3
ratio6= abs(q3k_200_6-0.10300925925925926e3)/0.10300925925925926e3

'''
print('q1k_300_2dot3= ', q1k_300_2dot3/1e3, 'kN/m2')
print("ratio1= ", ratio1)
print('q1k_300_6dot3= ', q1k_300_6dot3/1e3, 'kN/m2')
print("ratio2= ", ratio2)

print('q2k_300_5= ', q2k_300_5/1e3, 'kN/m2')
print("ratio3= ", ratio3)
print('q2k_300_9= ', q2k_300_9/1e3, 'kN/m2')
print("ratio4= ", ratio4)

print('q3k_200_2= ', q3k_200_2/1e3, 'kN/m2')
print("ratio5= ", ratio5)
print('q3k_200_6= ', q3k_200_6/1e3, 'kN/m2')
print("ratio6= ", ratio6)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((ratio1<1e-11) and (ratio2<1e-11) and (ratio3<1e-11) and (ratio4<1e-11) and (ratio5<1e-11) and (ratio6<1e-11)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
