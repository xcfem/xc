# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom

# Define sliding vector system
svd1=geom.SlidingVectorsSystem3d()

# Defina a sliding vector.
ptoAplic=geom.Pos3d(1,1,0)
testVector=geom.Vector3d(0,0,1)
vec= geom.SlidingVector3d(ptoAplic,testVector)
svd1+=vec # add it to the system.

# Another sliding vector.
ptoAplic=geom.Pos3d(-1,1,0)
vec=geom.SlidingVector3d(ptoAplic,testVector)
svd1+=vec # add it to the system.

# And another one
ptoAplic=geom.Pos3d(-1,-1,0)
vec=geom.SlidingVector3d(ptoAplic,testVector)
svd1+=vec # add it to the system

# Again...
ptoAplic=geom.Pos3d(1,-1,0)
vec=geom.SlidingVector3d(ptoAplic,testVector)
svd1+=vec # add it to the system

Res= svd1.getResultant()
Mom= svd1.getMoment()

#It seems there is a bug in svd.getResultant() it returns a SlidingVectorsSystem3d instead of a Vector3d.
#print('type<Res>= ', type(Res))
#print('Res= ', Res)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if Res.x == 0 and Res.y == 0 and Res.z == 4 and Mom.x == 0 and Mom.y == 0 and Mom.z == 0: 
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

