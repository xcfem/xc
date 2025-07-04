# -*- coding: utf-8 -*-
''' Check tributary ElasticFoundation class. Test based on equilibrium considerations.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from scipy.constants import g
import xc
from model import predefined_spaces
from materials import typical_materials
from materials.ec2 import EC2_materials

# Problem geometry
foundationWidth= 10.0
foundationLength= 10.0
elementSize= 1.0

# Problem type
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

pt0= modelSpace.newKPoint(-foundationLength/2.0, -foundationWidth/2.0, 0)
pt1= modelSpace.newKPoint(foundationLength/2.0, -foundationWidth/2.0, 0)
pt2= modelSpace.newKPoint(foundationLength/2.0, foundationWidth/2.0, 0)
pt3= modelSpace.newKPoint(-foundationLength/2.0, foundationWidth/2.0, 0)

slab= modelSpace.newQuadSurface(pt0, pt1, pt2, pt3)
slab.setElemSizeIJ(elementSize, elementSize)

## Mesh generation.
xcTotalSet= modelSpace.getTotalSet()
xcTotalSet.conciliaNDivs()
### Define materials.
concrete= EC2_materials.C30
slabDepth= 0.60 # thickness.
slabMaterial= concrete.defElasticMembranePlateSection(preprocessor= preprocessor, name= "slabMaterial", thickness= slabDepth)
## Generate mesh.
modelSpace.setDefaultMaterial(slabMaterial)
modelSpace.newSeedElement('ShellMITC4')
xcTotalSet.genMesh(xc.meshDir.I)

slabSet= modelSpace.defSet(setName= 'slabSet', surfaces= [slab])
slabSet.fillDownwards()

# Compute tributary areas.
slabSet.resetTributaries()
slabSet.computeTributaryAreas(False)

testOK= True
visitedNodes= set()

# Check results.
for p in [pt0, pt1, pt2, pt3]:
    n= p.getNode()
    a= n.getTributaryArea()
    testOK= testOK and (abs(a-0.25)<1e-6)
    visitedNodes.add(n.tag)

for l in slabSet.lines:
    for n in l.nodes:
        if not n.tag in visitedNodes:
            a= n.getTributaryArea()
            testOK= testOK and (abs(a-0.5)<1e-6)
            visitedNodes.add(n.tag)
for n in slabSet.nodes:
    if not n.tag in visitedNodes:
        a= n.getTributaryArea()
        testOK= testOK and (abs(a-1.0)<1e-6)
        visitedNodes.add(n.tag)
        
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
