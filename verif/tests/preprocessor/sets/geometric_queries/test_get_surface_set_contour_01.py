# -*- coding: utf-8 -*-
''' Home made test.
    Obtaining the contour of an element set. '''

from __future__ import print_function

import xc
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# This is the surface set:
#
#      8+---+7
#   4   | c |
#   +---+---+6
#   | a | b |
#   +---+---+
#   1   2   5

# Define points.
p1= modelSpace.newKPoint(0,0,0)
p2= modelSpace.newKPoint(1,0,0)
p3= modelSpace.newKPoint(1,1,0)
p4= modelSpace.newKPoint(0,1,0)
p5= modelSpace.newKPoint(2,0,0)
p6= modelSpace.newKPoint(2,1,0)
p7= modelSpace.newKPoint(2,2,0)
p8= modelSpace.newKPoint(1,2,0)

# Define surfaces.
a= modelSpace.newQuadSurface(p1, p2, p3, p4)
b= modelSpace.newQuadSurface(p2, p5, p6, p3)
c= modelSpace.newQuadSurface(p3, p6, p7, p8)

totalSet= preprocessor.getSets.getSet('total')
polygons= totalSet.surfaces.getContours()

length= polygons[0].getLength()

ratio1= (length-8.0)/8.0
pointSequences= totalSet.surfaces.getContoursPointSequences()
pointSequenceTags= list()
refPointSequenceTags= [p1.tag, p2.tag, p5.tag, p6.tag, p7.tag, p8.tag, p3.tag, p4.tag]
for n_seq in pointSequences:
    for n in n_seq:
        pointSequenceTags.append(n.tag)

testOK= (abs(ratio1)<1e-9) and (pointSequenceTags==refPointSequenceTags)

'''
print("lentgh= ",length)
print("ratio1= ",ratio1)
print(pointSequenceTags)
print(refPointSequenceTags)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
