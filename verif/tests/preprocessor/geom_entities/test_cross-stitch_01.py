# -*- coding: utf-8 -*-
''' Create nodes along two lines without creating elements on them.'''

from __future__ import print_function

import geom
import xc
import math
import os
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDiv= 10
CooMax= 10

b= 0.05 # Cross section width in meters
h= 0.10 # Cross section depth in meters
A= b*h # Cross section area en m2
I= 1/12.0*b*h**3 # Moment of inertia in m4
E=2.0E11 # Elastic modulus en N/m2
L=10 # Length of bars


# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

pt1= modelSpace.newKPoint(0.0,0.0,0.0)
pt2= modelSpace.newKPoint(CooMax,0.0,0.0)
pt3= modelSpace.newKPoint(0.0,L,0.0)
pt4= modelSpace.newKPoint(CooMax,L,0.0)

l1= modelSpace.newLine(pt1, pt2)
l1.nDiv= NumDiv
l2= modelSpace.newLine(pt3, pt4)
l2.nDiv= NumDiv

# Create only nodes on lines (seedElement not set).
feProblem.setVerbosityLevel(0) # Don't print warning messages about element see.
l1.genMesh(xc.meshDir.I)
l2.genMesh(xc.meshDir.I)
feProblem.setVerbosityLevel(1) # print warnings again-

# Create elements.
## Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)

## Material definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)
modelSpace.setDefaultMaterial(scc)
## Create elements.
for i in range(1,NumDiv+2):
    n1= l1.getNodeI(i)
    n2= l2.getNodeI(i)
    beam2d= modelSpace.newElement("ElasticBeam2d", [n1.tag, n2.tag])
    beam2d.h= h

modelSpace.fixNodesLine(line= l1)

# Load definition.
## Define load pattern
lp0= modelSpace.newLoadPattern(name= '0')
## Load values.
nNodes= l2.getNumNodes
for i in range(1,nNodes+1):
    n= l2.getNodeI(i)
    nodeTag= n.tag
    xNod= n.getCoo[0]
    F= (100*xNod+10)
    lp0.newNodalLoad(nodeTag,xc.Vector([0,F,0]))

modelSpace.addLoadCaseToDomain(lp0.name)
nCargasNod= lp0.getNumNodalLoads

# Solution procedure
analysis= predefined_solutions.simple_static_linear(feProblem)
analOk= analysis.analyze(1)

nNodes= l2.getNumNodes
errDisp= 0.0
for i in range(1,nNodes+1):
    n= l2.getNodeI(i)
    F= (100*n.getCoo[0]+10)
    vTeor= (F*L/E/A)
    errDisp= errDisp + (vTeor-n.getDisp[1])**2

errDisp= math.sqrt(errDisp)

xcTotalSet= modelSpace.getTotalSet()

constraints= preprocessor.getBoundaryCondHandler
numSPs= constraints.getNumSPs
nNodes= xcTotalSet.getNumNodes
elements= xcTotalSet.getElements
nElem= elements.size
cumple= True
vteor2= (CooMax/NumDiv)**2
lteor= math.sqrt(3*vteor2)
for e in elements:
    ratio1= (lteor/e.getCoordTransf.getInitialLength)
    cumple= (abs(ratio1-1.0)<1e-5) & cumple 

nNodTeor= 2*(NumDiv+1)
ratio1= (nNodes-nNodTeor)
nElemTeor= nNodTeor/2
ratio2= (nElem-nElemTeor)
nSPTeor= 3*nElemTeor
ratio3= (numSPs-nSPTeor)
ratio4= (nCargasNod-nElem)
ratio5= (errDisp)

''' 
print("number of nodes: ",nNodes)
print("number of boundary conditions: ",numSPs)
print("number of loads on nodes: ",nCargasNod)
print("number of elements: ",nElem)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("ratio5= ",ratio5)
print("analOk= ",analOk)
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1) < 1e-15) & (abs(ratio2) < 1e-15) & (abs(ratio3) < 1e-15) & (abs(ratio4) < 1e-15) & (abs(ratio5) < 1e-11) & (analOk == 0.0) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLoads()

