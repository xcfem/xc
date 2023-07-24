# -*- coding: utf-8 -*-
''' Test of earth pressure due to a uniform load in the backfill
surface. Based on the the "A.4 worked example to accompany Chapter 4" of 
the publication: Eurocode 7: Geotechnical Design Worked examples. Worked 
examples presented at the Workshop “Eurocode 7: Geotechnical 
Design” Dublin, 13-14 June, 2013.

https://publications.jrc.ec.europa.eu/repository/handle/JRC85029
'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com" 

import math
import geom
from actions.earth_pressure import earth_pressure as ep

#
#                             /+
#                           /  | V
#                         /    | i
#     Backfill slope -> /      | r            
#                     /        | t
#      zTopWall --- /          | u
#               |  |           | a
#               |  |           | l
#               |  |           |
#               |  |           | b
#               |  |           | a
#               |  |           | c
#               |  |           | k
#          +----   ------------+    <- zTopFooting
#          |Toe          Heel  |
#          +-------------------+
#

# Define wall virtual back.
virtualBack= geom.Segment2d(geom.Pos2d(2.95,0.818933), geom.Pos2d(2.95, -6.8))

# Define pressure.
qUnif= 5e3
zBottomSoils=[-1e3]
kSoils= [0.486]
soil= ep.UniformPressureOnBackfill(zGround=virtualBack.getFromPoint().y, zBottomSoils= zBottomSoils, KSoils= kSoils, qUnif= qUnif, xcSet= None, vDir= None)
slopeOfBackfillSurface= math.radians(20)

# Compute force on a surface.
forces= soil.getForces2D(virtualBack, numDiv= 10, beta= slopeOfBackfillSurface)

# Compute design value.
gammaQ= 1.3
EaQd= gammaQ*forces.getResultant().getModulus()
EaQdRef= gammaQ*kSoils[0]*qUnif*virtualBack.getLength()
ratio1= abs(EaQd-EaQdRef)/EaQdRef

'''
print(EaQd)
print(EaQdRef)
print(ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
