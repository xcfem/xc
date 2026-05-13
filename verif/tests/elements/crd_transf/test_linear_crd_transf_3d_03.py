# -*- coding: utf-8 -*-
''' Check that the seed element coordinate transformation is updated when
    modelSpace.setDefaultCoordTransf is called.
'''

from __future__ import print_function
from __future__ import division
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Problem type

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Geometric transformation.
linA= modelSpace.newLinearCrdTransf("linA",xc.Vector([0,1,0]))

# Materials
section= typical_materials.defElasticSection3d(preprocessor, "section",1,1,1,1,1,1)

# K-points.
p1= modelSpace.newKPoint(0,0,0)
p2= modelSpace.newKPoint(5,5,5)
p3= modelSpace.newKPoint(10,10,10)

# Lines.
l1= modelSpace.newLine(p1, p2)
l1.nDiv= 1 # Only one element along the line.
l2= modelSpace.newLine(p2, p3)
l1.nDiv= 1 # Only one element along the line.


# Elements definition
modelSpace.setDefaultMaterial(section)
modelSpace.setDefaultCoordTransf(linA) # Coordinate transformation for the new elements
seedElem= modelSpace.newSeedElement('ElasticBeam3d')

# Generate mesh on l1.
l1.genMesh(xc.meshDir.I)

# Another geometric transformation.
linB= modelSpace.newLinearCrdTransf("linB",xc.Vector([0,0,1]))
modelSpace.setDefaultCoordTransf(linB) # Coordinate transformation for the new elements

# Generate mesh on l1.
l2.genMesh(xc.meshDir.I)

crdTransfA= l1.elements[0].getCoordTransf
crdTransfB= l2.elements[0].getCoordTransf

testOK= (crdTransfA.name==linA.name) and (crdTransfB.name==linB.name)

'''
print(crdTransfA.name)
print(crdTransfB.name)
print(testOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

