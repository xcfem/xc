# -*- coding: utf-8 -*-
''' Trivial test of a rectangular quad region.'''
from __future__ import print_function
from __future__ import division

import math
import xc
from materials import typical_materials

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Create the simplest quad region.
testMat= typical_materials.defElasticMaterial(preprocessor, "testMat",1e6)
coordinates= xc.Matrix([[-10,-5], [10,-5], [10,5], [-10,5]])
quadSectRegion= xc.QuadSectRegion(testMat, 1, 32, coordinates)

# Compute and test some derived values.
pMin= quadSectRegion.pMin
pMax= quadSectRegion.pMax
refModulus= math.sqrt(20**2+10**2)
ratio1= abs((pMax-pMin).getModulus()-refModulus)/refModulus
area= quadSectRegion.getArea()
ratio2= abs(area-20*10)

'''
print('pMin= ', pMin)
print('pMax= ', pMax)
print('ratio1= ', ratio1)
print('area= ', area)
print('ratio2= ', ratio2)
'''

from misc_utils import log_messages as lmsg
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) and (abs(ratio2)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

