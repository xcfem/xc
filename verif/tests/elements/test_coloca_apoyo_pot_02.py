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

from model import fix_node_6dof
from materials import apoyosPot
from model import predefined_spaces
from model import define_apoyos
from model import apoyo_pot
from solution import predefined_solutions

from model import fix_node_6dof

diamPot= 993e-3


# Problem type
# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
teflon= apoyosPot.defineMaterialTeflon(preprocessor, diamPot,"teflonK")

nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(1,1,1)
nod= nodes.newNodeXYZ(1,1,1)



apoyo_pot.colocaApoyoFicticioPotDeslizanteXNodos(preprocessor, 1,2,1,"teflonK")
# Constraints
constraints= preprocessor.getConstraintLoader

fix_node_6dof.fixNode6DOF(constraints,1)
spc= constraints.newSPConstraint(2,3,0.0) # Nodo 2
spc= constraints.newSPConstraint(2,4,0.0)
spc= constraints.newSPConstraint(2,5,0.0)


# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([FX,FY,FZ,MX,MY,MZ]))
#We add the load case to domain.
casos.addToDomain("0")


# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

R= apoyo_pot.getReacApoyoFicticioPotDeslizanteXNodos(preprocessor, 1)

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
