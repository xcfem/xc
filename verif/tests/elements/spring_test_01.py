# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

K= 1000 # Spring constant
l= 100 # Distance between nodes
F= 1 # Force magnitude

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials


# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l,0.0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",K)
    
''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast.name
elements.dimElem= 2 # Dimension of element space
elements.defaultTag= 1
#  sintaxis: Spring[<tag>] 
spring= elements.newElement("Spring",xc.ID([n1.tag,n2.tag]))
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc1= constraints.newSPConstraint(n1.tag,0,0.0) # Node 1
spc2= constraints.newSPConstraint(n1.tag,1,0.0)
spc3= constraints.newSPConstraint(n2.tag,1,0.0) # Node 2



# Load case definition.
lPattern= '0'
lp0= modelSpace.newLoadPattern(name= lPattern)

# we check that loads are cummulated by the way.
lp0.newNodalLoad(n2.tag,xc.Vector([F/2.0,0]))
lp0.newNodalLoad(n2.tag,xc.Vector([F/2.0,0]))
modelSpace.addLoadCaseToDomain(lPattern) # Append load pattern to domain.
# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

deltax= n2.getDisp[0] 
deltay= n2.getDisp[1] 
R= n1.getReaction[0] 

elements= preprocessor.getElementHandler
spring.getResistingForce()
Ax= spring.getMaterial().getStrain() # Spring elongation


ratio1= (F+R/F)
ratio2= ((K*deltax-F)/F)
ratio3= ((deltax-Ax)/Ax)
''' 
print("R= ",R)
print("dx= ",deltax)
print("dy= ",deltay)
print("Ax= ",Ax)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
 '''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
