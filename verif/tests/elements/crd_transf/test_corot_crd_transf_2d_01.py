# -*- coding: utf-8 -*-
''' Home made test.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(5,5)

# Geometric transformations
lin= modelSpace.newCorotCrdTransf("lin")

# Materials
section= typical_materials.defElasticSection2d(preprocessor, "section",1,1,1)
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name # Coordinate transformation for the new elements
elements.defaultMaterial= section.name
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))

crdTransf= beam2d.getCoordTransf
iVector= crdTransf.getIVector
jVector= crdTransf.getJVector
vILocal= crdTransf.getVectorLocalCoordFromGlobal(iVector)
vJLocal= crdTransf.getVectorLocalCoordFromGlobal(jVector)
ratio1= (vILocal-xc.Vector([1,0])).Norm()
ratio2= (vJLocal-xc.Vector([0,1])).Norm()

'''
print("I vector:", iVector)
print("J vector:", jVector)
print("local coordinates I vector:",vILocal)
print('ratio1= ', ratio1)
print("local coordinates J vector:",vJLocal)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
