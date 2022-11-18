# -*- coding: utf-8 -*-
''' Rough design of concrete slabs under punching loads according to the
    publication: "Números gordos en el proyecto de estructuras" by
    Juan Carlos Arroyo et al. ISBN: 97884932270-4-3. Cinter 2009.

   https://books.google.es/books?id=tf5-tgAACAAJ
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from rough_calculations import ng_punching_shear
from materials.ehe import EHE_materials

# Materials
concrete= EHE_materials.HA25
steel= EHE_materials.B500S

# Problem geometry
d=0.20 # effective depth of the slab
a=0.25 # column depth
b=0.25 # column width
A= 5*5 # column tributary area.
h=0.25 # slab thickness

# Load 
qk=2e3 #N/m2



# Rough estimation of the punching shear load over a column
Vd= ng_punching_shear.designPunchingLoad(qk,A)

# Approximate design value of punching shear resistance of a slab without 
# punching shear reinforcement, computed at the critical perimeter
# defined to occur at 2d from the column faces.
#
# concrete: concrete material.
# d: effective depth of the floor deck (m)
# a,b: dimensions of the column (m)
Vdmax= ng_punching_shear.maximum_punching_shear_load(concrete= concrete,d= d, a= a, b= b)

# Estimate the needed punching shear reinforcement area computed at the critical
# perimeter defined to occur at 2d from the column faces
#
# concrete: concrete material.
# steel: steel material.
# Vd: Desing value of the punching shear (N)
# d: effective depth of the floor deck (m)
# h: slab depth (m).
# a,b: dimensions of the column (m)
Aws= ng_punching_shear.punching_shear_reinforcement_area(concrete= concrete, steel= steel, Vd= Vd, d= d, a= a, b= b, h= h)

ratio1= (Vd-80e3)/80e3
ratio2= (Vdmax-720e3)/720e3

'''
print("Vd= ",Vd/1e3," kN")
print("ratio1= ",ratio1)
print("Vdmax= ",Vdmax/1e3," kN")
print("ratio2= ",ratio2)
print("Aws= ", Aws," cm2/m")
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5 and abs(Aws)<1e-12:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

