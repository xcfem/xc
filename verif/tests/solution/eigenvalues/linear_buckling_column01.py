# -*- coding: utf-8 -*-
# Tomado del Ejemplo B46 del SOLVIA Verification Manual
''' NO SE PORQUÉ, NO DA MUY BUENOS RESULTADOS NI CON ESTA FORMULACIÓN NI CON LA COROTACIONAL

   The problem is probably related with the fact that the elements are unable
   to reproduce the little delta effect (see example test_pdelta_01.xc).
  '''
from __future__ import division
import xc_base
import geom
import xc

from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 4.0 # Longitud de la columna en meters
b= 0.2 # Cross section width en meters
h= 0.2 # Cross section depth en meters
A= b*h # Cross section area en m2
I= 1/12.0*b*h**3 # Momento de inercia en m4
E=30E9 # Elastic modulus en N/m2
P= -100 # Carga vertical sobre la columna.

NumDiv= 4

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)


nodes.newSeedNode()
# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newPDeltaCrdTransf2d("lin")


# Seed element definition
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "scc"
seedElemLoader.defaultTransformation= "lin"
seedElemLoader.defaultTag= 1 #Number for the next element will be 1.
beam2d= seedElemLoader.newElement("elastic_beam_2d",xc.ID([0,0]))
beam2d.h= h
beam2d.rho= 0.0

puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(0.0,L,0.0))
lines= preprocessor.getCad.getLines
lines.defaultTag= 1
l= lines.newLine(1,2)
l.nDiv= NumDiv


setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)
# Constraints
constraints= preprocessor.getConstraintLoader

#
spc= constraints.newSPConstraint(1,0,0.0) # Node 2,gdl 0 # Back end node.
spc= constraints.newSPConstraint(1,1,0.0) # Node 2,gdl 1
spc= constraints.newSPConstraint(2,0,0.0) # Node 2,gdl 0 # Front end node.

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,P,0]))

#We add the load case to domain.
casos.addToDomain("0")


# Procedimiento de solución
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/../../aux/solu_linear_buckling.py")



eig1= analysis.getEigenvalue(1)
nod2= nodes.getNode(2)
deltay= nod2.getDisp[1] 

deltayTeor= P*L/(E*A)
ratio1= deltay/deltayTeor
blCalc= eig1*P
blTeor= -1*math.pi**2*E*I/(L**2)
ratio2= (blCalc-blTeor)/blTeor

''' 
print "deltay= ",(deltay)
print "deltayTeor= ",(deltayTeor)
print "eig1= ",(eig1)
print "ratio1= ",(ratio1)
print "blCalc= ",(blCalc/1e6)," MN \n"
print "blTeor= ",(blTeor/1e6)," MN \n"
print "ratio2= ",(ratio2)
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2)<0.06):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
