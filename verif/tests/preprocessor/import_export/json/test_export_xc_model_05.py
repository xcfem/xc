# -*- coding: utf-8 -*-
''' Test the getDict/setFromDict methods. Check that the object properties 
    are taken into account.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import geom
import xc

testValueA= geom.Vector3d(1,0,0)
testValueB= "test"

# Test problem.
fePrbA= xc.FEProblem()
prepA=  fePrbA.getPreprocessor

# Create point with property.
points= prepA.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0,0,0))
pt1.setProp("testValueA", testValueA)
pt1.setProp("testValueB", testValueB)

# Get dictionary.
mbtDict= prepA.getMultiBlockTopology.getDict()

# Create new problem.
fePrbB= xc.FEProblem()
prepB= fePrbB.getPreprocessor

# Populate from the previous dictionary.
mbt= prepB.getMultiBlockTopology
mbt.setFromDict(mbtDict)

# Get the corresponding points.
pointsB=  prepB.getMultiBlockTopology.getPoints
ptB1= pointsB.get(pt1.tag)
propValueA= ptB1.getProp('testValueA')
propValueB= ptB1.getProp('testValueB')

ok= (propValueA==testValueA) and (propValueB==testValueB)

'''
print(mbtDict)
print(propValueA)
print(propValueB)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ok):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    

