from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

# Define plane.
o=geom.Pos3d(0,0,0)
p1=geom.Pos3d(1,0,0)
p2=geom.Pos3d(0,1,0)
plane=geom.Plane3d(o,p1,p2)

# Define vectors.
v1= geom.Vector3d(10, 10, 10) # First octant.
v2= geom.Vector3d(-10, 10, 10) # Second octant.
v3= geom.Vector3d(-10, -10, 10) # Third octant.
v4= geom.Vector3d(10, -10, 10) # Fourth octant.
v5= geom.Vector3d(10, 10, -10) # Fifth octant.
v6= geom.Vector3d(-10, 10, -10) # Sixth octant.
v7= geom.Vector3d(-10, -10, -10) # Seventh octant.
v8= geom.Vector3d(10, -10, -10) # Eigtht octant.

vectors= [v1, v2, v3, v4, v5, v6, v7, v8]

# Compute projections.
projections= list()
for v in vectors:
    projections.append(plane.getProjection(v))

# Check results
normal= plane.getNormal()
ref_projections= list()
for v in vectors:
    projection= v-v.dot(normal)*normal
    ref_projections.append(projection)

error= 0.0
for proj, ref_proj in zip(projections, ref_projections):
    zero= proj-ref_proj
    error+= zero.getModulus2()
error= math.sqrt(error)

# print(error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(error<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
