# -*- coding: utf-8 -*-
''' Test made from the example 7.2 of the book: 
    Introducción Al Estudio Del Elemento Finito en Ingeniería. 
    Segunda Edición. Tirupathi R. Chandrupatla, Ashok D. Belegundu. 
    Pearson Educación, 1999
    isbn={9789701702604},
    url={https://books.google.ch/books?id=auQwAAAACAAJ},
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

E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density
t= 1
F=1000

# Problem type
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Don't pring warnings
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)


n1= nodes.newNodeIDXY(1,0,0)
n2= nodes.newNodeIDXY(2,1,0)
n3= nodes.newNodeIDXY(3,1,1)
n4= nodes.newNodeIDXY(4,0,1)


elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast2d"
tri31= elements.newElement("tri31",xc.ID([1,2,3]))
tri31.thickness= t
tri31= elements.newElement("tri31",xc.ID([3,4,1]))
tri31.thickness= t

# Constraints
constraints= preprocessor.getBoundaryCondHandler

spc= constraints.newSPConstraint(1,0,0.0)
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(4,0,0.0)
spc= constraints.newSPConstraint(4,1,0.0)

# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0]))
lp0.newNodalLoad(3,xc.Vector([F,0]))

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
analOk= analisis.analyze(1)


nodes.calculateNodalReactions(True,1e-7)
 
# print "reac node 1: ",reac
R1x= n1.getReaction[0]
R1y= n1.getReaction[1]

# print "reac node 5: ",reac
R4x= n4.getReaction[0]
R4y= n4.getReaction[1]

ratio1= abs(((R1x+F)/F))
ratio2= abs(((R4x+F)/F))
ratio3= abs(((R1y+R4y)/F))

''' 
print "R1x= ",R1x
print "R1y= ",R1y
print "ratio1= ",ratio1
print "R4x= ",R4x
print "R4y= ",R4y
print "ratio2= ",ratio2
print "ratio3= ",ratio3
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (analOk == 0.0):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
