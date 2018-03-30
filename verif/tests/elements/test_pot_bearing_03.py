# -*- coding: utf-8 -*-
# Home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

FX= 1.0 # Force magnitude
FY= 2.0 
FZ= 3.0
MX= 4.0
MY= 5.0 
MZ= 6.0

import xc_base
import geom
import xc

from model import predefined_spaces
from materials import bridge_bearings
from solution import predefined_solutions



diamPot= 993e-3


# Problem type
# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
pot= bridge_bearings.PTFEPotBearing(diamPot)
pot.defineMaterials(preprocessor)

nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod1= nodes.newNodeXYZ(1,1,1)
nod2= nodes.newNodeXYZ(1,1,1)



newElement= pot.putOnYBetweenNodes(modelSpace, nod1.tag,nod2.tag)

# Constraints
modelSpace.fixNode000_000(1)
spc= modelSpace.constraints.newSPConstraint(nod2.tag,3,0.0) # Node 2
spc= modelSpace.constraints.newSPConstraint(nod2.tag,4,0.0)
spc= modelSpace.constraints.newSPConstraint(nod2.tag,5,0.0)


# Loads definition
cargas= preprocessor.getLoadHandler

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(nod2.tag,xc.Vector([FX,FY,FZ,MX,MY,MZ]))
#We add the load case to domain.
casos.addToDomain("0")


# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)

R= bridge_bearings.get_reaction_on_pot(preprocessor, newElement.tag)

ratio1= abs(R[0]+FX)/FX
ratio2= abs(R[1]+FY)/FY
ratio3= abs(R[2]+FZ)/FZ

''' 
print "RX= ",R[0]
print "RY= ",R[1]
print "RZ= ",R[2]
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-15) & (ratio2<1e-15) & (ratio3<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
