# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

KX= 1000 # Spring constant
KY= 2000 # Spring constant
KZ= 3000 # Spring constant
FX= 1 # Force magnitude
FY= 2 
FZ= 3

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(1,1,1)

    
# Materials definition
kx= typical_materials.defElasticMaterial(preprocessor, "kx",KX)
ky= typical_materials.defElasticMaterial(preprocessor, "ky",KY)
kz= typical_materials.defElasticMaterial(preprocessor, "kz",KZ)


nodosApoyados= [1]

fixedNodes= list()
sz= len(nodosApoyados)
for i in range(0,sz):
  nodeTag= nodosApoyados[i]
  fixedNodeTag, elemTag= modelSpace.setBearing(nodeTag,["kx","ky","kz"])
  fixedNodes.append(fixedNodeTag)
tagNodoFijo= fixedNodes[0]

# Constraints
constraints= preprocessor.getConstraintLoader

#
spc= constraints.newSPConstraint(1,3,0.0) # Nodo 1
spc= constraints.newSPConstraint(1,4,0.0)
spc= constraints.newSPConstraint(1,5,0.0)


# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(1,xc.Vector([FX,FY,FZ,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")


# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



nodes.calculateNodalReactions(True)
nod1= nodes.getNode(1)
deltax= nod1.getDisp[0]
deltay= nod1.getDisp[1]
deltaz= nod1.getDisp[2]
RX= nodes.getNode(tagNodoFijo).getReaction[0]
RY= nodes.getNode(tagNodoFijo).getReaction[1]
RZ= nodes.getNode(tagNodoFijo).getReaction[2] 


ratio1= -RX/FX
ratio2= (KX*deltax)/FX
ratio3= -RY/FY
ratio4= (KY*deltay)/FY
ratio5= -RZ/FZ
ratio6= (KZ*deltaz)/FZ

'''
print "RX= ",RX
print "dx= ",deltax
print "RY= ",RY
print "dy= ",deltay
print "RZ= ",RZ
print "dz= ",deltaz
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
print "ratio5= ",(ratio5)
print "ratio6= ",(ratio6)
   '''
  
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5) & (abs(ratio4-1.0)<1e-5)  & (abs(ratio5-1.0)<1e-5) & (abs(ratio6-1.0)<1e-5) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
