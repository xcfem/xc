# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


L= 2.0 # Size of element edge.
t= 0.2 # Element thickness.
E= 2.1e6 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
# Load
F= 10 # Load magnitude

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0,0)
nod= nodes.newNodeXYZ(L,0,0)
nod= nodes.newNodeXYZ(L,L,0)
nod= nodes.newNodeXYZ(0,L,0)


# Materials definition
nmb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,0.0,t)

elements= preprocessor.getElementHandler
elements.defaultMaterial= "memb1"
elements.defaultTag= 1
elem= elements.newElement("ShellMITC4",xc.ID([1,2,3,4]))


# Constraints
modelSpace.fixNode000_FFF(1)
spc= modelSpace.constraints.newSPConstraint(2,1,0.0)
spc= modelSpace.constraints.newSPConstraint(2,2,0.0)
spc= modelSpace.constraints.newSPConstraint(3,2,0.0)
spc= modelSpace.constraints.newSPConstraint(4,2,0.0)

# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(3,xc.Vector([0,F,0,0,0,0]))
lp0.newNodalLoad(4,xc.Vector([0,-F,0,0,0,0]))
#We add the load case to domain.
lPatterns.addToDomain("0")



# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)


n1Medio= 0.0
n2Medio= 0.0
n12Max= 0.0
n12Min= 0.0

# print "G1= ",vectorG1
elem.getResistingForce()
mats= elem.getPhysicalProperties.getVectorMaterials #Materials at gauss points.
#Gauss points iterator
for m in mats:
  n1Medio= n1Medio+m.getStressResultantComponent("n1")
  n2Medio= n2Medio+m.getStressResultantComponent("n2")
  n12Max= max(n12Max,m.getStressResultantComponent("n12"))
  n12Min= min(n12Min,m.getStressResultantComponent("n12"))

n1Medio= n1Medio/4
n2Medio= n2Medio/4


n1MedioTeor= 0.0
n2MedioTeor= 0.0
ratio1= abs((n1Medio-n1MedioTeor))
ratio2= abs((n2Medio-n2MedioTeor))
ratio3= abs((n12Max+n12Min))

''' 
print "n1Medio= ",n1Medio
print "n1MedioTeor= ",n1MedioTeor
print "ratio1= ",ratio1
print "n2Medio= ",n2Medio
print "n2MedioTeor= ",n2MedioTeor
print "ratio2= ",ratio2
print "n12Max= ",n12Max
print "n12Min= ",n12Min
print "ratio3= ",ratio3
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-14) & (abs(ratio2)<1e-14) & (abs(ratio3)<1e-13) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
