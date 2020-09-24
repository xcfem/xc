# -*- coding: utf-8 -*-
# home made test

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
l= 20*12 # Bar length in inches

E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
A= 50.65 # Beam cross-section area in square inches.
Iz= 7892 # Inertia of the beam section in inches to the fourth power.
Iy= 7892 # Inertia of the beam section in inches to the fourth power.
J= Iz/100.0 # Cross section torsion constant
F= 1000 # Force

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(l,0,0)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
    
# Materials definition
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
scc= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "section",sectionProperties)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
#  syntax: beam2d_02[<tag>] 
beam= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))
        
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
modelSpace.fixNode000_000(n1.tag) # Node 1


setTotal= preprocessor.getSets.getSet("total")


# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(n2.tag,xc.Vector([-F,0,0,0,0,0]))
lp1= lPatterns.newLoadPattern("default","1")
lp1.newNodalLoad(n2.tag,xc.Vector([-F,0,0,0,0,0]))

#We add the load case to domain and solve.
lPatterns.addToDomain(lp0.name)
result= modelSpace.analyze(calculateNodalReactions= True)

R1= n1.getReaction[0] 
R2= n2.getReaction[0]

N= beam.getN()
modelSpace.deactivateElements(setTotal)

#We add the load case to domain and solve.
result= modelSpace.analyze(calculateNodalReactions= True)

R1B= n1.getReaction[0] 
R2B= n2.getReaction[0]

NB= beam.getN()

lPatterns.addToDomain(lp1.name)
result= modelSpace.analyze(calculateNodalReactions= True)

R1C= n1.getReaction[0] 
R2C= n2.getReaction[0]

NC= beam.getN()

# Return elements to life.
modelSpace.activateElements(setTotal)
result= modelSpace.analyze(calculateNodalReactions= True)

R1D= n1.getReaction[0] 
R2D= n2.getReaction[0]

ND= beam.getN()

ratio1= abs(R1-F)/F
ratio2= abs(R2)
ratio3= abs(N+F)/F

ratio4= abs(R1B)
ratio5= abs(R2B-F)/F
ratio6= abs(NB)/F

ratio7= abs(R1C)
ratio8= abs(R2C-2*F)/F
ratio9= abs(R1D-2*F)

ratio10= abs(R1D-2*F)/F
ratio11= abs(R2D)
ratio12= abs(ND+2*F)/F

ratios= [ratio1, ratio2, ratio3, ratio4, ratio5, ratio6, ratio7, ratio8, ratio9, ratio10, ratio11, ratio12]

err= 0.0
for r in ratios:
  err+= r*r

err= math.sqrt(err)

''' 
print('R1= ', R1, ' R2= ', R2, 'N= ', N)
print('R1B= ', R1B, ' R2B= ', R2B, 'NB= ', NB)
print('R1C= ', R1C, ' R2C= ', R2C, 'NC= ', NC)
print('R1D= ', R1D, ' R2D= ', R2D, 'ND= ', ND)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("ratio5= ",ratio5)
print("ratio6= ",ratio6)
print("ratio7= ",ratio7)
print("ratio8= ",ratio8)
print("ratio9= ",ratio9)
print("ratio10= ",ratio10)
print("ratio11= ",ratio11)
print("ratio12= ",ratio12)
print("err= ",err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-8):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

