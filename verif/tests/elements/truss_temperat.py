# -*- coding: utf-8 -*-
''' Home made test
    Reference:  Cálculo de estructuras por el método de los elemen-
    tos finitos. 1991. E. Oñate, page 165, example 5.3

    isbn={9788487867002}
    url={https://books.google.ch/books?id=lV1GSQAACAAJ}

'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials


L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
AT= 10 # Temperature increment (Celsius degrees)

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0.0,0.0)
nod= nodes.newNodeXY(L,0.0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
    
# Elements definition
elements= preprocessor.getElementLoader
elements.defaultMaterial= "elast"
elements.dimElem= 2 # Dimension of element space
elements.defaultTag= 1 #Tag for the next element.
truss= elements.newElement("truss",xc.ID([1,2]));
truss.area= A
 
    
# Constraints
constraints= preprocessor.getConstraintLoader

#
spc= constraints.newSPConstraint(1,0,0.0)
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(2,0,0.0)
spc= constraints.newSPConstraint(2,1,0.0)

    
# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns
ts= casos.newTimeSeries("linear_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("truss_temp_load")
eleLoad.elementTags= xc.ID([1])
eleLoad.eps1= alpha*10
eleLoad.eps2= alpha*10
#We add the load case to domain.
casos.addToDomain("0")

analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


elements= preprocessor.getElementLoader

elem1= elements.getElement(1)
elem1.getResistingForce()
N= (-E*A*alpha*AT)
ratio= ((elem1.getN()-N)/N)

# print "ratio= ",ratio

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
