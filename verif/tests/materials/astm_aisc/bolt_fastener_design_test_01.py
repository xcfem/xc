# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

''' Steel bolts according to AISC 360-16 verification test.
    Based on the:
    Example – Design of a Bolted Tension Bearing-Type Connection
    of the course:
    Design of Bolts in Shear-BearingConnections per AISC LRFD 3rdEdition (2001)
'''
__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

in2m= 25.4e-3
kip2N= 4448.2216

import math
from materials.astm_aisc import ASTM_materials


bolt= ASTM_materials.BoltFastener(0.75*in2m, steelType= ASTM_materials.A325) # group A
designShearStrength= 2.0*bolt.getDesignShearStrength() # double shear
designShearStrengthRef= 159.042810002e3 #31.8*kip2N
ratio1= abs(designShearStrength-designShearStrengthRef)/designShearStrengthRef

Pd= 121.6*kip2N
n= math.ceil(Pd/designShearStrength)
centersDist= bolt.getRecommendedDistanceBetweenCenters()
minEdgeDist= bolt.getMinimumEdgeDistance()
minWidth= 2.0*minEdgeDist+centersDist
netWidth= minWidth-2.0*bolt.getNominalHoleDiameter()
# Yielding in the gross section.
minThicknessA= Pd/0.9/ASTM_materials.A36.fy/minWidth
# Tension fracture in the net section.
minThicknessB= Pd/0.75/ASTM_materials.A36.fu/netWidth
minThickness= max(minThicknessA,minThicknessB)
ratio2= abs(minThickness-27.6747887726e-3)/27.6747887726e-3

'''
print('Pd= ', Pd/1e3, 'kN')
print('d= ', bolt.diameter*1e3, 'mm')
print('design shear strength= ', designShearStrength/1e3, 'kN')
print('reference design shear strength= ', designShearStrengthRef/1e3, 'kN')
print('ratio1= ', ratio1)
print('n= ', n)
print('distance between centers: ', centersDist*1e3, 'mm')
print('minimum edge distance: ', minEdgeDist*1e3, 'mm')
print('minimum width: ', minWidth*1e3, 'mm')
print('minimum thickness (yielding in the gross section): ', minThicknessA*1e3, 'mm')
print('minimum thickness (tension fracture in the net section): ', minThicknessB*1e3, 'mm')
print('minimum thickness: ', minThickness*1e3, 'mm')
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-6 and ratio2<1e-6):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
