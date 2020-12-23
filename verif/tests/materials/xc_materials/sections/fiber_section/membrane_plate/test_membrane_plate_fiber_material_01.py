# -*- coding: utf-8 -*-
''' Test membrane plate fiber material. Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
rhoMat= 500
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25,rho= rhoMat)

plateFiber= typical_materials.defMembranePlateFiberSection(preprocessor, name= "plateFiber", h= 0.2, nDMaterial= elast3d);


rhoPlate= plateFiber.rho
rhoRef= rhoMat*plateFiber.h

ratio1= abs(rhoRef-plateFiber.rho)/rhoRef

'''
print('h= ', plateFiber.h)
print('rho= ', rhoPlate)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-10):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
