# -*- coding: utf-8 -*-
''' Example 6.3 from EC2 Worked Examples (rev A 31-03-2017)'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.ec2 import EC2_materials

# Geometry
concreteMaterials= [EC2_materials.C12, EC2_materials.C16, EC2_materials.C20, EC2_materials.C25, EC2_materials.C30, EC2_materials.C35,EC2_materials.C40,EC2_materials.C45,EC2_materials.C50]
stirrupsSteel= EC2_materials.S500B

results= list()
for c in concreteMaterials:
    ro_wmin= c.getMinShearReinfRatio(stirrupsSteel, 'Spain')
    results.append(ro_wmin*1e3)

# Values taken frm "EUROCÓDIGO 2 GUÍA RÁPIDA Calter March 2021"
refResults= [0.42, 0.51, 0.59, 0.68, 0.77, 0.86, 0.94, 1.01, 1.09]

error= 0.0
for r, rRef in zip(results, refResults):
    error+= (r-rRef)**2
error= math.sqrt(error)

'''
print(results)
print(error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
