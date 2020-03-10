# -*- coding: utf-8 -*-
# Test from Ansys manual
# Reference:  Strength of Material, Part I, Elementary Theory & Problems, pg. 98, problem 4

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 30e6 # Young modulus (psi)
l= 20*12 # Bar length in inches
a= 10*12 # Cantilever length
h= 30 # Beam cross-section depth in inches.
A= 50.65 # viga area in square inches.
I= 7892 # Inertia of the beam section in inches to the fourth power.
w= 10000/12 # Load per unit length (pounds)/pulgada.

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
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
elements.defaultTransformation= "lin"
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

# Solution procedure
import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/../../aux/solu_transf_handler2.py")
    

nod3= nodes.getNode(3)
delta= nod3.getDisp[1] 

elem2= elements.getElement(2)
elem2.getResistingForce()
sigma= elem2.getN2/A+elem2.getM2/I*h/2.0



# print delta
# print sigma
ratio1= (delta/0.182)
ratio2= (sigma/(-11400))

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<3e-3) & (abs(ratio2-1.0)<1e-3):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

