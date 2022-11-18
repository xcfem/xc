# -*- coding: utf-8 -*-
''' Rough design of concrete slabs under punching loads according to the
    publication: "Números gordos en el proyecto de estructuras" by
    Juan Carlos Arroyo et al. ISBN: 97884932270-4-3. Cinter 2009.

   https://books.google.es/books?id=tf5-tgAACAAJ
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, AO_O and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

from rough_calculations import ng_punching_shear
from materials.ec2 import EC2_materials

# Materials
concrete= EC2_materials.C30
steel= EC2_materials.S500B

# Loads.
## Compute uniform load on slab from column axial load.
columnAxialLoad= 215e3
tributaryArea= 1.0*3.95/2.0
slabUniformLoad= columnAxialLoad/tributaryArea

# Problem geometry.
d= 0.23 # distance from the micropile plate to the top surface of the slab.
slabThickness= 0.4
micropilePlateSide= 0.2
criticalPerimeterLength= 2.4589

# Maximum punching shear
# Approximate design value of punching shear resistance of a slab without 
# punching shear reinforcement, computed at the critical perimeter
# defined to occur at 2d from the column faces.
#
# concrete: concrete material.
# d: effective depth of the floor deck (m)
# a,b: dimensions of the column (m)
VdMax= ng_punching_shear.maximum_punching_shear_load(concrete, d= d, a= micropilePlateSide, b= micropilePlateSide)
ratio1= abs(777.2160000000002e3-VdMax)/777.2160000000002e3

# Design value of punching shear resistance of a slab without punching
# shear reinforcement.
#
# concrete: concrete material.
# d: effective depth of the floor deck (m)
# a,b: dimensions of the column (m)
# criticalPerimeterLength: length of the critical perimeter.
Vcu= ng_punching_shear.concrete_resisted_punching_load(concrete= concrete, d= d, a= micropilePlateSide, b= micropilePlateSide, criticalPerimeterLength= criticalPerimeterLength)
ratio2= abs(291.2582591531955e3-Vcu)/291.2582591531955e3

# Required punching shear reinforcement.
# Estimate the needed punching shear reinforcement area computed at the critical
# perimeter defined to occur at 2d from the column faces.
#
# concrete: concrete material.
# steel: steel material.
# Vd: Desing value of the punching shear (N)
# d: effective depth of the floor deck (m)
# h: slab depth (m).
# a,b: dimensions of the column (m)
# criticalPerimeterLength: length of the critical perimeter.
Asw= ng_punching_shear.punching_shear_reinforcement_area(concrete= concrete, steel= steel, Vd= columnAxialLoad, d= d, h= slabThickness, a= micropilePlateSide, b= micropilePlateSide, criticalPerimeterLength= criticalPerimeterLength)

'''
print('slab uniform load: ', slabUniformLoad/1e3, 'kN/m2')
print('Maximum punching shear VdMax= ', VdMax/1e3, 'kN')
print('Concrete resisted punching shear Vcu= ', Vcu/1e3, 'kN')
print('Required punching shear reinforcement area Asw= ', Asw*1e4, 'cm2')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5 and abs(Asw)<1e-15:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
