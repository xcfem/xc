# -*- coding: utf-8 -*-
# home made test

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

E= 2.1e6 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.

q= -2

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod1= nodes.newNodeXYZ(0,0,0)
nod2= nodes.newNodeXYZ(1,0,0)
nod3= nodes.newNodeXYZ(1,1,0)
nod4= nodes.newNodeXYZ(0,1,0)


# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)


elements= preprocessor.getElementHandler
elements.defaultMaterial= "memb1"
elem= elements.newElement("ShellMITC4",xc.ID([nod1.tag,nod2.tag,nod3.tag,nod4.tag]))


# Constraints
modelSpace.fixNode000_FFF(nod1.tag)
modelSpace.fixNode000_FFF(nod2.tag)
modelSpace.fixNode000_FFF(nod3.tag)
modelSpace.fixNode000_FFF(nod4.tag)

# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
#lPatterns.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("shell_uniform_load")
eleLoad.elementTags= xc.ID([elem.tag]) 
eleLoad.transComponent= q
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)


nodes.calculateNodalReactions(True,1e-7)

RN1= nod1.getReaction[2] 
RN2= nod2.getReaction[2] 
RN3= nod3.getReaction[2] 
RN4= nod4.getReaction[2] 
RTeor= q/4.0

ratio1= abs((RN1+RTeor)/RTeor)
ratio2= abs((RN2+RTeor)/RTeor)
ratio3= abs((RN3+RTeor)/RTeor)
ratio4= abs((RN4+RTeor)/RTeor)



'''
print "RTeor= ", RTeor
print "RN1= ", RN1
print "ratio1= ",ratio1
print "RN2= ", RN2
print "ratio2= ",ratio2
print "RN3= ", RN3
print "ratio3= ",ratio3
print "RN4= ", RN4
print "ratio4= ",ratio4
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-12) & (ratio4 < 1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

