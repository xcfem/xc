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
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
I= (2e-2)**4/12 # Cross section moment of inertia (m4)
AT= 10 # Temperature increment (Celsius degrees)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

n1= nodes.newNodeXY(0.0,0.0)
n2= nodes.newNodeXY(L,0.0)

# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin")

# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= "lin"
elements.defaultMaterial= "scc"
elements.dimElem= 2 # Dimension of element space
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(n1.tag,0,0.0)
spc= constraints.newSPConstraint(n1.tag,1,0.0)
spc= constraints.newSPConstraint(n1.tag,2,0.0)
spc= constraints.newSPConstraint(n2.tag,0,0.0)
spc= constraints.newSPConstraint(n2.tag,1,0.0)
spc= constraints.newSPConstraint(n2.tag,2,0.0)

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("linear_ts","ts")
#ts.factor= 1.0
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
#\set_current_load_pattern{"0"}
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([beam2d.tag])
thermalDeformation= xc.DeformationPlane(alpha*AT)
eleLoad.backEndDeformationPlane= thermalDeformation
eleLoad.frontEndDeformationPlane= thermalDeformation

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

result= modelSpace.analyze(calculateNodalReactions= False)

beam2d.getResistingForce()
axil1= beam2d.getN1
axil2= beam2d.getN2

N= (-E*A*alpha*AT)
ratio= ((axil2-N)/N)

# print "N= ",N
# print "axil1= ",axil1
# print "axil2= ",axil2
# print "ratio= ",ratio

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
