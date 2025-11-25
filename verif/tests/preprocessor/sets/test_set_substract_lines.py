# -*- coding: utf-8 -*-
''' Test sets substract method with lines.'''

import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Create FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodeHandler)

# Create points.
p0= modelSpace.newKPoint(0,0)
p1= modelSpace.newKPoint(1,0)
p2= modelSpace.newKPoint(2,0)
p3= modelSpace.newKPoint(3,1)
p4= modelSpace.newKPoint(4,1)

# Create lines.
lineA= modelSpace.newLine(p0, p1)
lineB= modelSpace.newLine(p1, p2)
lineC= modelSpace.newLine(p2, p3)
lineD= modelSpace.newLine(p3, p4)

# Create sets.
s1= modelSpace.defSet('s1', lines= [lineA, lineB, lineC, lineD])
s1.fillDownwards()
s2= modelSpace.defSet('s2', lines= [lineB, lineC])
s2.fillDownwards()

# Check results.
numPointsBefore= len(s1.points)
numLinesBefore= len(s1.lines)
s1.substract(s2.lines)
s1.substract(s2.points)
s1.fillDownwards()
numPointsAfter= len(s1.points)
tagsPointsAfter= [p.tag for p in s1.points]
numLinesAfter= len(s1.lines)
tagsLinesAfter= [l.tag for l in s1.lines]

testOK= (((numPointsBefore-numPointsAfter)-1)==0)
for p in [p0, p1, p3, p4]:
    testOK and (p.tag in tagsPointsAfter)
testOK= testOK and (((numLinesBefore-numLinesAfter)-len(s2.lines))==0)
for l in [lineA, lineD]:
    testOK and (l.tag in tagsLinesAfter)

'''
print('numPointsBefore= ', numPointsBefore)
print('numPointsAfter= ', numPointsAfter)
print('tagsPointsAfter= ', tagsPointsAfter)
print('numLinesBefore= ', numLinesBefore)
print('numLinesAfter= ', numLinesAfter)
print('tagsLinesAfter= ', tagsLinesAfter)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
