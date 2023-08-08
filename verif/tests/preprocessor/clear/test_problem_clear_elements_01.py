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
from materials import typical_materials

E= 30e6 # Young modulus
l= 10 # Bar length

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
elements= preprocessor.getElementHandler

# Define nodes.
n1= nodeHandler.newNodeXY(0,0)
n2= nodeHandler.newNodeXY(0,1.0)
# Define material.
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
# Define elements.
elements.dimElem= 2 # Bars defined in a two dimensional space.
elements.defaultMaterial= elast.name
truss= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
truss.sectionArea= 1

## Get number of nodes and elements.
nn0= preprocessor.getDomain.getMesh.getNumNodes()
ne0= preprocessor.getDomain.getMesh.getNumElements()

# Clear all.
feProblem.clearAll()

## Get number of nodes and elements.
nn1= preprocessor.getDomain.getMesh.getNumNodes()
ne1= preprocessor.getDomain.getMesh.getNumElements()

# Define new nodes.
nA= nodeHandler.newNodeXY(2,0)
nB= nodeHandler.newNodeXY(2,1.0)
nC= nodeHandler.newNodeXY(2,2.0)

# Define new material.
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
# Define new elements.
elements.defaultMaterial= elast.name
trussA= elements.newElement("Truss",xc.ID([nA.tag,nB.tag]))
trussA.sectionArea= 1
trussB= elements.newElement("Truss",xc.ID([nB.tag,nC.tag]))
trussB.sectionArea= 1


nn2= preprocessor.getDomain.getMesh.getNumNodes()
ne2= preprocessor.getDomain.getMesh.getNumElements()

err= abs(nn0-2)+abs(nn1)+abs(nn2-3)
err= abs(ne0-1)+abs(ne1)+abs(ne2-2)

'''
print(nA.tag, nB.tag, nC.tag)
print(trussA.tag, trussB.tag)
print(nn0, nn1, nn2, err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if err==0:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


