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
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nod1= nodes.newNodeXY(0.0,0.0)
nod2= nodes.newNodeXY(L,0.0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast"
elements.dimElem= 2 # Dimension of element space
truss= elements.newElement("Truss",xc.ID([nod1.tag,nod2.tag]))
truss.sectionArea= A

    
# Constraints
constraints= preprocessor.getBoundaryCondHandler

#
spc1= constraints.newSPConstraint(nod1.tag,0,0.0)
spc2= constraints.newSPConstraint(nod1.tag,1,0.0)
spc3= constraints.newSPConstraint(nod2.tag,0,0.0)
spc4= constraints.newSPConstraint(nod2.tag,1,0.0)

    
# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("linear_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
#lPatterns.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("truss_strain_load")
eleLoad.elementTags= xc.ID([truss.tag])
eleLoad.eps1= alpha*AT
eleLoad.eps2= alpha*AT
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

result= modelSpace.analyze(calculateNodalReactions= False)


elements= preprocessor.getElementHandler

truss.getResistingForce()
N= (-E*A*alpha*AT)
ratio= ((truss.getN()-N)/N)

# print "ratio= ",ratio

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
