# -*- coding: utf-8 -*-
from __future__ import print_function

import xc_base
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

nodes.defaultTag= 1

# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin")
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
 

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(CooMax,0.0,0.0))
pt3= points.newPoint(geom.Pos3d(0.0,L,0.0))
pt4= points.newPoint(geom.Pos3d(CooMax,L,0.0))
lines= preprocessor.getMultiBlockTopology.getLines
l1= lines.newLine(pt1.tag,pt2.tag)
l1.nDiv= NumDiv
l2= lines.newLine(pt3.tag,pt4.tag)
l2.nDiv= NumDiv


# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

setTotal= preprocessor.getSets.getSet("total")

feProblem.setVerbosityLevel(0) # Dont print(warning messages about element seed.)
setL1= preprocessor.getSets.getSet(l1.name)
setL1.genMesh(xc.meshDir.I)


setL2= preprocessor.getSets.getSet(l2.name)
setL2.genMesh(xc.meshDir.I)
feProblem.setVerbosityLevel(1) # print(warnings again )

# totalSetNodes= setTotal.getNodes
# for n in totalSetNodes:
#   print("node tag: ", n.tag)

elements.defaultMaterial= scc.name
for i in range(1,NumDiv+2):
  n1= l1.getNodeI(i)
  n2= l2.getNodeI(i)
  # print("i= ", i, "n1= ", n1.tag, "n2= ", n2.tag)
  beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
  beam2d.h= h

modelSpace.fixNodesLine(line= l1)

# Load patterns
# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')


nNodes= l2.getNumNodes
for i in range(1,nNodes+1):
  n= l2.getNodeI(i)
  nodeTag= n.tag
  xNod= n.getCoo[0]
  F= (100*xNod+10)
  lp0.newNodalLoad(nodeTag,xc.Vector([0,F,0]))

modelSpace.addLoadCaseToDomain(lp0.name)
nCargasNod= lp0.getNumNodalLoads

''' 
nodalLoads= lp0.getNodalLoads
for nl in nodalLoads:
    print("node: ",numNod)
'''


# Solution procedure
analysis= predefined_solutions.simple_static_linear(feProblem)
analOk= analysis.analyze(1)

nNodes= l2.getNumNodes
# print("size= ",size)
errDisp= 0.0
for i in range(1,nNodes+1):
  n= l2.getNodeI(i)
  F= (100*n.getCoo[0]+10)
  vTeor= (F*L/E/A)
  errDisp= errDisp + (vTeor-n.getDisp[1])**2

errDisp= math.sqrt(errDisp)

constraints= preprocessor.getBoundaryCondHandler
numSPs= constraints.getNumSPs
nNodes= setTotal.getNumNodes
elements= setTotal.getElements
nElem= elements.size
cumple= 1
vteor2= (CooMax/NumDiv)**2
lteor= math.sqrt(3*vteor2)
for e in elements:
  # print("  elem: ",tag," nod. I: ",nod(0).tag," nod. J: ",nod(1).tag," L= ",length)
  # print("lteor: ",(lteor))
  ratio1= (lteor/e.getCoordTransf.getInitialLength)
  cumple= (abs(ratio1-1.0)<1e-5) & (cumple) 
  # print("cumple: ",(cumple))

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
