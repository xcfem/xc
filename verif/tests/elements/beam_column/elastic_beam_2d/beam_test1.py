# -*- coding: utf-8 -*-
# Test from Ansys manual VM2

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

# Reference:  Strength of Materials, Part I, Elementary Theory and Problems, pg. 98, problem 4
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

n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(a,0.0)
n3= nodes.newNodeXY(a+(l/2),0.0)
n4= nodes.newNodeXY(a+l,0.0)
n5= nodes.newNodeXY(2*a+l,0.0)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
    
# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
#  syntax: beam2d_02[<tag>] 

beam2dA= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
beam2dA.h= h
        
beam2dB= elements.newElement("ElasticBeam2d",xc.ID([n2.tag,n3.tag]))
beam2dB.h= h
        
beam2dC= elements.newElement("ElasticBeam2d",xc.ID([n3.tag,n4.tag]))
beam2dC.h= h
        
beam2dD= elements.newElement("ElasticBeam2d",xc.ID([n4.tag,n5.tag]))
beam2dD.h= h
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(n2.tag,0,0.0) # Node 2
spc= constraints.newSPConstraint(n2.tag,1,0.0)
spc= constraints.newSPConstraint(n4.tag,1,0.0) # Node 4


# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("beam2d_uniform_load")
eleLoad.elementTags= xc.ID([beam2dA.tag,beam2dD.tag]) 
eleLoad.transComponent= -w
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
    

delta= n3.getDisp[1] 
deltaRef=.182

elements= preprocessor.getElementHandler

beam2dB.getResistingForce()
sigma= beam2dB.getN2/beam2dB.sectionProperties.A+beam2dB.getM2/beam2dB.sectionProperties.I*beam2dB.h/2.0
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
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
