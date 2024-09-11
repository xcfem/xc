# -*- coding: utf-8 -*-
''' Test calculation of concrete compressive strength at time t
    acdording to clause 3.1.2 of EC2:2004. Home made test.'''

from __future__ import division
from __future__ import print_function

import math
from materials.ec2 import EC2_materials

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Materials
concrete= EC2_materials.C40

fck7= concrete.getFckt(t= 7)
fck28= concrete.getFckt(t= 28)

err= math.sqrt((fck7+29.382437587427432e6)**2+(fck28+40e6)**2)

'''
print(fck7/1e6)
print(fck28/1e6)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-8):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

# # Grahpical output
# ti= list(range(4,40))
# fcki= list()
# for t in ti:
#     fcki.append(concrete.getFckt(t)/1e6)
    
# import matplotlib.pyplot as plt
# plt.plot(ti, fcki)
# plt.grid()
# plt.show()


