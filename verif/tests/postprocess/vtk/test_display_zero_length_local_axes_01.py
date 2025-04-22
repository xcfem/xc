# -*- coding: utf-8 -*-
'''Verifies that the code that displays the axes of the zero length elements
does not crash.'''


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

K= 1000 # Spring constant

import os
import xc
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Model definition
n1= nodes.newNodeXY(1,1)
n2= nodes.newNodeXY(1,1)

# Materials definition
k= typical_materials.defElasticMaterial(preprocessor, "k",K)


''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= k.name
elements.dimElem= 2 # Dimension of element space
elements.deraultTag= 1
zl1= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))
zl1.setupVectors(xc.Vector([1,1,0]),xc.Vector([-1,1,0]))

'''
print(zl1.getIVector)
print(zl1.getJVector)
'''
  
# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
# oh.displayLocalAxes()

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayLocalAxes(fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
