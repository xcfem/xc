# -*- coding: utf-8 -*-
''' home made test
    Reference:  Cálculo de estructuras por el método de los elemen-
    tos finitos. 1991. E. Oñate, page 165, example 5.3

    isbn={9788487867002}
    url={https://books.google.ch/books?id=lV1GSQAACAAJ}

'''


import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Coeficiente de dilatación of the steel
A= 4e-4 # bar area expressed in square meters
I= (2e-2)**4/12 # Cross section moment of inertia (m4)
AT= 10 # Incremento de temperatura expressed in grados centígrados
gammaF= 1.5

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0.0,0.0)
nod= nodes.newNodeXY(L,0.0)


# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")


# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)


# Elements definition
elements= preprocessor.getElementLoader
elements.defaultTransformation= "lin"
elements.defaultMaterial= "scc"
elements.dimElem= 2 # Dimension of element space
elements.defaultTag= 1 #Tag for next element.
beam2d= elements.newElement("elastic_beam_2d",xc.ID([1,2]))

    
# Constraints
constraints= preprocessor.getConstraintLoader

#
spc= constraints.newSPConstraint(1,0,0.0)
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(1,2,0.0)
spc= constraints.newSPConstraint(2,0,0.0)
spc= constraints.newSPConstraint(2,1,0.0)
spc= constraints.newSPConstraint(2,2,0.0)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns
ts= casos.newTimeSeries("linear_ts","ts")
casos.currentTimeSeries= "ts"
lp0= casos.newLoadPattern("default","0")
lp0.gammaF= gammaF
#casos.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([1])
defTermica= xc.DeformationPlane(alpha*AT)
eleLoad.backEndDeformationPlane= defTermica
eleLoad.frontEndDeformationPlane= defTermica
#We add the load case to domain.
casos.addToDomain("0")

analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



elem1= elements.getElement(1)
elem1.getResistingForce()
axil1= elem1.getN1
axil2= elem1.getN2


N= (-gammaF*E*A*alpha*AT)
ratio= ((axil2-N)/N)

''' 
print "N= ",N
print "axil1= ",axil1
print "axil2= ",axil2
print "ratio= ",ratio
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
