# -*- coding: utf-8 -*-
''' Trivial test for ConcentratedLoad objects.

See section 11-13 in:

  "Foundation Analysis and Design". Fifth Edition. Joseph E. Bowles. 
   McGraw-Hill 1982
   ISBN= 9780070067707
   LCCN= lc81013649
   url= https://books.google.es/books?id=eNtRAAAAMAAJ
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import math
from geotechnics import boussinesq

# Define loaded area
concentratedLoad= boussinesq.ConcentratedLoad(p= geom.Pos3d(0.5,0.5,0))


# Compute stress increment.
testPoints= [geom.Pos3d(0.25,0.25,-1), geom.Pos3d(.25,0.75,-1), geom.Pos3d(0.75,0.25,-1), geom.Pos3d(0.75,0.75,-1)]
unitVectorDirs= 4*[geom.Vector3d(0,0,1)] # Vertical stresses.
vertStresses= concentratedLoad.getStressIncrement(P= -1e3, points= testPoints, unitVectorDirs= unitVectorDirs)

# Check results
## Vertical stresses.
vStress= -355.6805199633012
refVertStresses= 4*[vStress]
err= 0.0
for v, rv in zip(vertStresses, refVertStresses):
    err+= (v.z-rv)**2

## radial stresses .
unitVectorDirs= [geom.Vector3d(-1,-1,0).normalized(),
                 geom.Vector3d(-1,1,0).normalized(),
                 geom.Vector3d(1,-1,0).normalized(),
                 geom.Vector3d(1,1,0).normalized()] # Horizontal (x) stresses.
rStress= concentratedLoad.getStressIncrement(P= -1e3, points= testPoints, unitVectorDirs= unitVectorDirs, eta= 1.0)

# Equation 11.20a:
r2= 0.25**2+0.25**2
R= math.sqrt(r2+1**2)
refValue= -1e3/math.pi/2.0*(3*r2*1/R**5-(1-2*1.0)/R/(R+1))
refRStress= 4*[refValue]
for v, rv, uv in zip(rStress, refRStress, unitVectorDirs):
    #err+= (v.x**2+v.y**2-rv)**2
    sigma_r= v.dot(uv)
    err+= (sigma_r-rv)**2
err+= math.sqrt(err)
    
'''
print(vertStresses)
print(refVertStresses)
print(rStress)
print(refRStress)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
