# -*- coding: utf-8 -*-
''' Pre-tensioned tendon stress variation along its lenght.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
#import matplotlib.pyplot as plt
from materials.prestressing import pre_tensioned_tendons
from materials.ehe import EHE_materials

crossSection= EHE_materials.Y1860S7Strand_15_3
straightStrand= pre_tensioned_tendons.StraightStrand(crossSection, transfLength= 1.4, xInit= 0.0, xEnd= 25.0)

xi= [0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5, 10.0, 10.5, 11.0, 11.5, 12.0, 12.5, 13.0, 13.5, 14.0, 14.5, 15.0, 15.5, 16.0, 16.5, 17.0, 17.5, 18.0, 18.5, 19.0, 19.5, 20.0, 20.5, 21.0, 21.5, 22.0, 22.5, 23.0, 23.5, 24.0, 24.5, 25.0]

fi= list()
for x in xi:
    fi.append(straightStrand.getStressFactor(x))

fi_ref= [0.0, 0.35714285714285715, 0.7142857142857143, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.7142857142857151, 0.35714285714285754, 0.0]

err= 0.0
for f,f_ref in zip(fi, fi_ref):
    err+= (f-f_ref)**2

err= math.sqrt(err)

'''
print(xi)
print(fi)
print('err= ',err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


# # Display results
# ## Display bending moments.
# fig, ax = plt.subplots()  # Create a figure and an axes.
# ax.plot(xi, fi, label='stress factor')
# ax.legend()  # Add a legend.
# plt.show()
