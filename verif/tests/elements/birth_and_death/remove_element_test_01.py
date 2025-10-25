# -*- coding: utf-8 -*-
''' Trivial test cheking element removal.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
l= 10.0 
nodeTags= list()
for i in range(0,4):
    newNode= nodes.newNodeXY(i*l,0)
    nodeTags.append(newNode.tag)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
# Materials definition
E= 30e6 # Young modulus (psi)
A= 50.65 # Beam cross-section area in square inches.
I= 7892 # Inertia of the beam section in inches to the fourth power.
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

# Elements definition
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(scc)
elementList= list()
n0Tag= nodeTags[0]
h= 30 # Beam cross-section depth in inches.
for n1Tag in nodeTags[1:]:
    elem= modelSpace.newElement("ElasticBeam2d",[n0Tag,n1Tag])
    elem.h= h
    elementList.append(elem)
    n0Tag= n1Tag

numberOfElementsBefore= modelSpace.getNumberOfElements()

elementsToRemove= [1]
for i in elementsToRemove:
    elementTag= elementList[i].tag
    elementList[i]= None # Avoid calls to a non-existent element.
    modelSpace.removeElement(elementTag)
# Pop removed elements from list.
for i in reversed(elementsToRemove):
    elementList.pop(i)

xi= list()
for element in elementList:
    xi.append(element.getPosCentroid(True).x)

numberOfElementsAfter= modelSpace.getNumberOfElements()
numberOfDeletedElements= numberOfElementsBefore-numberOfElementsAfter
testOK= (numberOfDeletedElements==1)
testOK= (math.sqrt((xi[0]-l/2.0)**2+(xi[1]-5/2.0*l)**2)<1e-15)

'''                    
print(numberOfElementsBefore)
print(numberOfElementsAfter)
print(numberOfDeletedElements)
print(nodeTags)
print(xi)
print(testOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

