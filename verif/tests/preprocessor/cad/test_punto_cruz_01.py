# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_3dof
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDiv= 10
CooMax= 10

b= 0.05 # Cross section width en meters
h= 0.10 # Cross section depth en meters
A= b*h # Cross section area en m2
I= 1/12.0*b*h**3 # Momento de inercia en m4
E=2.0E11 # Elastic modulus en N/m2
L=10 # Longitud de las barras


# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodes)

nodes.defaultTag= 1
nodes.newSeedNode()
# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")

elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
 

puntos= preprocessor.getCad.getPoints
pt1= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt2= puntos.newPntIDPos3d(2,geom.Pos3d(CooMax,0.0,0.0))
pt3= puntos.newPntIDPos3d(3,geom.Pos3d(0.0,L,0.0))
pt4= puntos.newPntIDPos3d(4,geom.Pos3d(CooMax,L,0.0))
lines= preprocessor.getCad.getLines
lines.defaultTag= 1
l1= lines.newLine(1,2)
l1.nDiv= NumDiv
l2= lines.newLine(3,4)
l2.nDiv= NumDiv


# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

setTotal= preprocessor.getSets.getSet("total")
nodosTotal= setTotal.getNodes

setL1= preprocessor.getSets.getSet("l1")
setL1.genMesh(xc.meshDir.I)


setL2= preprocessor.getSets.getSet("l2")
setL2.genMesh(xc.meshDir.I)

# for n in nodosTotal:
#   print "node tag: ", n.tag

elementos.defaultMaterial= "scc"
for i in range(1,NumDiv+2):
  n1= l1.getNodeI(i)
  n2= l2.getNodeI(i)
  #print "i= ", i, "n1= ", n1.tag, "n2= ", n2.tag
  beam2d= elementos.newElement("elastic_beam_2d",xc.ID([n1.tag,n2.tag]))
  beam2d.h= h



fix_node_3dof.fijaNodosLinea3GDL(preprocessor.getConstraintLoader,l1)


# Casos de carga
cargas= preprocessor.getLoadLoader
#Contenedor de hipótesis de carga:
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"

l2= preprocessor.getSets.getSet("l2")

nNodes= l2.getNumNodes
for i in range(1,nNodes+1):
  n= l2.getNodeI(i)
  tagNodo= n.tag
  xNod= n.getCoo[0]
  F= (100*xNod+10)
  lp0.newNodalLoad(tagNodo,xc.Vector([0,F,0]))

casos.addToDomain("0")
nCargasNod= lp0.getNumNodalLoads

''' 
nodalLoads= lp0.getNodalLoads
for nl in nodalLoads:
  print "nodo: ",numNod
'''


# Procedimiento de solución
analisis= predefined_solutions.simple_static_linear(prueba)
analOk= analisis.analyze(1)

nNodes= l2.getNumNodes
# print "size= ",size
errDisp= 0.0
for i in range(1,nNodes+1):
  n= l2.getNodeI(i)
  F= (100*n.getCoo[0]+10)
  vTeor= (F*L/E/A)
  errDisp= errDisp + (vTeor-n.getDisp[1])**2

errDisp= math.sqrt(errDisp)

constraints= preprocessor.getConstraintLoader
numSPs= constraints.getNumSPs
nNodes= setTotal.getNumNodes
elementos= setTotal.getElements
nElem= elementos.size
cumple= 1
vteor2= (CooMax/NumDiv)**2
lteor= math.sqrt(3*vteor2)
for e in elementos:
  # print "  elem: ",tag," nod. I: ",nod(0).tag," nod. J: ",nod(1).tag," L= ",length
  # print "lteor: ",(lteor)
  ratio1= (lteor/e.getCoordTransf.getInitialLength)
  cumple= (abs(ratio1-1.0)<1e-5) & (cumple) 
  # print "cumple: ",(cumple)

nNodTeor= 2*(NumDiv+1)
ratio1= (nNodes-nNodTeor)
nElemTeor= nNodTeor/2
ratio2= (nElem-nElemTeor)
nSPTeor= 3*nElemTeor
ratio3= (numSPs-nSPTeor)
ratio4= (nCargasNod-nElem)
ratio5= (errDisp)

''' 
print "number of nodes: ",nNodes
print "number of boundary conditions: ",numSPs
print "number of loads on nodes: ",nCargasNod
print "number of elements: ",nElem
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
print "ratio5= ",ratio5
print "analOk= ",analOk
 '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1) < 1e-15) & (abs(ratio2) < 1e-15) & (abs(ratio3) < 1e-15) & (abs(ratio4) < 1e-15) & (abs(ratio5) < 1e-11) & (analOk == 0.0) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
