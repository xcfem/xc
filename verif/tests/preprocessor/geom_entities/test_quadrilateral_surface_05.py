# -*- coding: utf-8 -*-
''' Check the revolve method. Home made test'''

from __future__ import division
from __future__ import print_function

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

#    4                    3
#    +--------------------+
#    |                    |
#    |                    |
#    |                    |
#    |                    |
#    |                    |
#    |                    |
#    |                    |
#    +--------------------+
#   1                      2

L= 1
pt1= modelSpace.newKPoint(0,0,0)
pt2= modelSpace.newKPoint(L,0,0)
pt3= modelSpace.newKPoint(L,L,0)
pt4= modelSpace.newKPoint(0,L,0)

s1= modelSpace.newSurface([pt1,pt2,pt3,pt4])
iVector0= s1.getIVector
s1.revolve(1)
iVector1= s1.getIVector
s1.revolve(1)
iVector2= s1.getIVector
s1.revolve(1)
iVector3= s1.getIVector

err= (iVector0-geom.Vector3d(1,0,0)).getModulus2()
err+= (iVector1-geom.Vector3d(0,1,0)).getModulus2()
err+= (iVector2-geom.Vector3d(-1,0,0)).getModulus2()
err+= (iVector3-geom.Vector3d(0,-1,0)).getModulus2()
err= math.sqrt(err)

'''
print(iVector0)
print(iVector1)
print(iVector2)
print(iVector3)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-8):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
  
# Graphic stuff.
#from postprocess import output_handler
#oh= output_handler.OutputHandler(modelSpace)

#oh.displayBlocks()
