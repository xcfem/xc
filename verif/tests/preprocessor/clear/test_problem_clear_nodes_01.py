# -*- coding: utf-8 -*-
''' Checks the clearAll method.
Home made test'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler

n1= nodeHandler.newNodeXY(0,0)
n2= nodeHandler.newNodeXY(0,1.0)

nn0= preprocessor.getDomain.getMesh.getNumNodes()

feProblem.clearAll()

nn1= preprocessor.getDomain.getMesh.getNumNodes()

nA= nodeHandler.newNodeXY(2,0)
nB= nodeHandler.newNodeXY(2,1.0)
nC= nodeHandler.newNodeXY(3,1.0)


nn2= preprocessor.getDomain.getMesh.getNumNodes()

err= abs(nn0-2)+abs(nn1)+abs(nn2-3)

'''
print(nA.tag, nB.tag, nC.tag)
print(nn0, nn1, nn2, err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if err==0:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


