# -*- coding: utf-8 -*-
''' Check that the displayElementValueDiagram method works. The test does
 not verify the output contents, only that the method runs and the file is
 created.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from postprocess import output_handler

# Problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics3D(nodes)

# Define mesh.
## Nodes.
fromNodes= list()
toNodes= list()
L= 10.0
for i in range(0,5):
    fromNodes.append(modelSpace.newNode(0.0, 10*i, 0))
    toNodes.append(modelSpace.newNode(L, 10*i, 0))
    
## Elements.
### Define materials.
steel= EC2_materials.S500B
dummyMaterial= steel.defElasticMaterial(preprocessor= preprocessor)
modelSpace.setDefaultMaterial(dummyMaterial)
modelSpace.setElementDimension(3) # Truss defined in a three-dimensional space.
for n0, n1 in zip(fromNodes, toNodes):
    newTruss= modelSpace.newElement("Truss", [n0.tag, n1.tag])
    area= float(n0.tag/100.0+0.01)
    newTruss.sectionArea= area
    
# Graphic stuff.
oh= output_handler.OutputHandler(modelSpace)

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayElementValueDiagram(itemToDisp= 'sectionArea', caption= 'Test', setToDisplay= None, fileName= outputFileName)

# oh.displayElementValueDiagram(itemToDisp= 'sectionArea', caption= 'Test', setToDisplay= None, fileName= None)
# Check that file exists
testOK= os.path.isfile(outputFileName)

import os
from misc_utils import log_messages as lmsg
if(testOK) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
