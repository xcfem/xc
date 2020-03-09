# -*- coding: utf-8 -*-
'''Estimation of the reciprocal of the condition number using the 1-norm.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
import numpy as np

data= [[1,1],[1,0]]

testMatrix= xc.Matrix(data)
npMatrix= np.array(data)
npInverse= np.linalg.inv(npMatrix)

rcond= testMatrix.RCond()
oneNorm= testMatrix.OneNorm()
npNorm= np.linalg.norm(npMatrix,1)
invMatrix= testMatrix.getInverse()
invOneNorm= invMatrix.OneNorm()
invNpMatrixNorm= np.linalg.norm(npInverse,1)
rcondTheor= 1.0/(oneNorm*invOneNorm)
rcondFrobenius= np.linalg.cond(npMatrix,'fro')
rcondNumpy= 1.0/(npNorm*invNpMatrixNorm)
ratio1= (rcond-rcondTheor)/rcondTheor
ratio2= (rcond-rcondNumpy)/rcondNumpy

# print('data= ',testMatrix)
# print('oneNorm= ',oneNorm)
# print('npNorm= ',npNorm)
# print('invMatrix= ',invMatrix)
# print('npInverse= ',npInverse)
# print('invOneNorm= ',invOneNorm)
# print('invNpMatrixNorm= ',invNpMatrixNorm)
# print('rcondTheor= ', rcondTheor)
# print('rcondFrobenius= ', rcondFrobenius)
# print('rcondNumpy= ', rcondNumpy)
# print('rcond= ', rcond)

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) and (abs(ratio2)<1e-5):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
