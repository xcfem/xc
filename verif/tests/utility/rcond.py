# -*- coding: utf-8 -*-
'''Estimation of the reciprocal of the condition number using the 1-norm.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc

data= [[1,1],[1,0]]

testMatrix= xc.Matrix(data)

rcond= testMatrix.RCond()
oneNorm= testMatrix.OneNorm()
invMatrix= testMatrix.getInverse()
invOneNorm= invMatrix.OneNorm()
rcondTheor= 1.0/(oneNorm*invOneNorm)
ratio1= (rcond-rcondTheor)/rcondTheor

#print 'rcond= ', rcond

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
