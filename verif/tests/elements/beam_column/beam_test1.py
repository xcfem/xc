# -*- coding: utf-8 -*-
# Test from Ansys manual VM2

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

# Reference:  Strength of Material, Part I, Elementary Theory and Problems, pg. 98, problem 4
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials


E= 30e6 # Young modulus (psi)
l= 20*12 # Bar length in inches
a= 10*12 # Cantilever length
h= 30 # Beam cross-section depth in inches.
A= 50.65 # Beam cross-section area in square inches.
I= 7892 # Inertia of the beam section in inches to the fourth power.
w= 10000/12.0 # Load per unit length (pounds)/pulgada.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)


nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(a,0.0)
nod= nodes.newNodeXY(a+(l/2),0.0)
nod= nodes.newNodeXY(a+l,0.0)
nod= nodes.newNodeXY(2*a+l,0.0)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
    
# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= "scc"
#  sintaxis: beam2d_02[<tag>] 
elements.defaultTag= 1 #Tag for next element.
beam2d= elements.newElement("ElasticBeam2d",xc.ID([1,2]))
beam2d.h= h
        
beam2d= elements.newElement("ElasticBeam2d",xc.ID([2,3]))
beam2d.h= h
        
beam2d= elements.newElement("ElasticBeam2d",xc.ID([3,4]))
beam2d.h= h
        
beam2d= elements.newElement("ElasticBeam2d",xc.ID([4,5]))
beam2d.h= h
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(2,0,0.0) # Node 2
spc= constraints.newSPConstraint(2,1,0.0)
spc= constraints.newSPConstraint(4,1,0.0) # Node 4


# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
#lPatterns.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("beam2d_uniform_load")
eleLoad.elementTags= xc.ID([1,4]) 
eleLoad.transComponent= -w
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
    

nodes= preprocessor.getNodeHandler
nod3= nodes.getNode(3)
delta= nod3.getDisp[1] 
deltaRef=.182

elements= preprocessor.getElementHandler

elem2= elements.getElement(2)
elem2.getResistingForce()
sigma= elem2.getN2/elem2.sectionProperties.A+elem2.getM2/elem2.sectionProperties.I*elem2.h/2.0
sigmaRef= -11400


ratio1= abs(delta-deltaRef)/deltaRef
ratio2= abs(sigma-sigmaRef)/sigmaRef

'''
print('deltaRef= ', deltaRef)
print('delta= ', delta)
print('ratio1= ',ratio1)
print('ratio2= ',ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<3e-3 and abs(ratio2)<1e-3):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
