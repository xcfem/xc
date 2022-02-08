# -*- coding: utf-8 -*-
''' Check that copy constructor work as intended (see priority of constructors in python_interface.tcc)'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom

pA= geom.Pos3d(0.0,0.0,0.0)
pB= geom.Pos3d(1.0,0.0,0.0)
vB= geom.Vector3d(0.0,1e3,0.0)
vA= geom.Vector3d(0.0,0,0.0)

svs1= geom.SlidingVectorsSystem3d(geom.SlidingVector3d(pA, vA)+geom.SlidingVector3d(pB, vB))

svs2= geom.SlidingVectorsSystem3d(geom.SlidingVector3d(pA, vA))
svs2+= geom.SlidingVector3d(pB, vB)
svs3= geom.SlidingVector3d(pA, vA)+geom.SlidingVector3d(pB, vB)

ratio1= (svs1.getResultant()-svs2.getResultant()).getModulus()
ratio2= (svs1.getResultant()-svs3.getResultant()).getModulus()
ratio3= abs(svs1.getMoment().z-svs2.getMoment().z)
ratio4= abs(svs1.getMoment().z-svs3.getMoment().z)

'''
print(svs1.getMoment().z)
print(svs1)
print(svs2)
print(svs3)
print(ratio1)
print(ratio2)
print(ratio3)
print(ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-13) and (ratio2<1e-13) and (ratio3<1e-13) and (ratio4<1e-13)):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

