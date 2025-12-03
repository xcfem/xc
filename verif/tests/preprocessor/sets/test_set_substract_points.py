# -*- coding: utf-8 -*-
''' Test sets substract method with points.'''

import xc
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodeHandler)

# Create points.
p0= modelSpace.newKPoint(0,0)
p1= modelSpace.newKPoint(1,0)
p2= modelSpace.newKPoint(2,0)
p3= modelSpace.newKPoint(3,1)

# Create sets.
s1= modelSpace.defSet('s1', points= [p0, p1, p2, p3])
s2= modelSpace.defSet('s2', points= [p1, p3])

# Check results.
numPointsBefore= len(s1.points)
s1.substract(s2.points)
numPointsAfter= len(s1.points)
testOK= (((numPointsBefore-numPointsAfter)-len(s2.points))==0)
tagsPointsAfter= [ p.tag for p in s1.points]
testOK= testOK and (p0.tag in tagsPointsAfter) and (p2.tag in tagsPointsAfter)

'''
print('numPointsBefore= ', numPointsBefore)
print('numPointsAfter= ', numPointsAfter)
print('tagsPointsAfter= ', tagsPointsAfter)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
