# -*- coding: utf-8 -*-
''' Wind load calculation.'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
from actions.snow import ec1_snow

# Snow load shape coefficients for mUlti-span roofs
alpha1= math.atan2(1,1)
alpha2= math.atan2(1,3)

snow_load_shape_coefficients= ec1_snow.get_snow_load_shape_coefficients_multi_span_roofs(alpha1Radians=alpha1, alpha2Radians= alpha2)

ref_snow_load_shape_coefficients= [[(0.25, 0.25), (0.8, 0.8), (0.25, 0.25), (0.8, 0.8)], [(0.25, 0.25), (0.25, 1.6), (1.6, 0.8), (0.8, 0.8)]]

testOK= (snow_load_shape_coefficients==ref_snow_load_shape_coefficients)

'''
print('alpha1= ', math.degrees(alpha1))
print('alpha2= ', math.degrees(alpha2))
print(snow_load_shape_coefficients)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic output.
# xi= [0.0, 1.0, 4.0, 5.0, 8.0]
# xxi= [xi[0]]
# yi1= list()
# for v in snow_load_shape_coefficients[0]:
#     yi1.append(v[0])
#     yi1.append(v[1])
# yi2= list()
# for v in snow_load_shape_coefficients[1]:
#     yi2.append(v[0])
#     yi2.append(v[1])
# for x in xi[1:-1]:
#     x_minus= x-1e-3
#     xxi.append(x_minus)
#     x_plus= x+1e-3
#     xxi.append(x_plus)
# xxi.append(xi[-1])
# import matplotlib.pyplot as plt
# fig, ax = plt.subplots()
# ax.plot(xxi, yi1, "-b", label="Case (i)")
# ax.plot(xxi, yi2, "-r", label="Case (ii)")
# ax.legend(loc="upper left")
# ax.set(xlabel='x (m)', ylabel='$\mu$',
#        title='Snow load shape coefficients for mUlti-span roofs')
# ax.grid()
 
# # Show graph
# plt.show()
