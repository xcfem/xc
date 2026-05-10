# -*- coding: utf-8 -*-
''' Home made test. Check displayStrongWeakAxis method. The test does not 
verify the output contents, only that the method runs and the file is
 created.
'''


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import sys
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from misc_utils import log_messages as lmsg

# Geometry
width= .001
depth= .01
nDivIJ= 2
nDivJK= 2
y0= 0
z0= 0
L= 1.5 # Bar length (m)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)


lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Materials definition
fy= 275e6 # Yield stress of the steel
E= 210e9 # Young modulus of the steel.
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)

import os
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
  pth= "."
auxModulePath= pth+"/../../aux"
sys.path.append(auxModulePath)
import test_quad_region as tqr

# Definition of a new empty fiber section named 'quadFibers' and stored in a
# Python variable of the same name (surprisingly enough).
quadFibers= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr() # Fiber section representation
                                                     # of 'quadFibers'
testQuadRegion= tqr.get_test_quad_region(preprocessor, y0, z0, width, depth, nDivIJ, nDivJK)
fiberSectionRepr.setGeomNamed(testQuadRegion.name) # We assign the geometry (regions and rebars)
                                                  # to the fiber section representation
                                                  # of 'quadFibers'
quadFibers.setupFibers() # Create the fibers from the information contained in
                         # the geometry.


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= quadFibers.name # Material name for the element (the fiber section).
beam3d= elements.newElement("ForceBeamColumn3d",xc.ID([n1.tag,n2.tag]))

# Constraints
modelSpace.fixNode000_000(n1.tag)

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
# oh.displayStrongWeakAxis()

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayStrongWeakAxis(fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
os.remove(outputFileName) # Clean after yourself.
