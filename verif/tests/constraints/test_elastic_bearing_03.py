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
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod= nodes.newNodeXYZ(1,1,1)

    
# Materials definition
kx= typical_materials.defElasticMaterial(preprocessor, "kx",KX)
ky= typical_materials.defElasticMaterial(preprocessor, "ky",KY)
kz= typical_materials.defElasticMaterial(preprocessor, "kz",KZ)


pinnedNodes= [nod.tag]

fixedNodes= list()
sz= len(pinnedNodes)
for i in range(0,sz):
  nodeTag= pinnedNodes[i]
  fixedNode, elem= modelSpace.setBearing(nodeTag,["kx","ky","kz"])
  fixedNodes.append(fixedNode.tag)
fixedNodeTag= fixedNodes[0]

# Constraints
constraints= preprocessor.getBoundaryCondHandler

#
spc= constraints.newSPConstraint(nod.tag,3,0.0) # Node 1
spc= constraints.newSPConstraint(nod.tag,4,0.0)
spc= constraints.newSPConstraint(nod.tag,5,0.0)


# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(nod.tag,xc.Vector([FX,FY,FZ,0,0,0]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)


# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)



nodes.calculateNodalReactions(True,1e-7)
deltax= nod.getDisp[0]
deltay= nod.getDisp[1]
deltaz= nod.getDisp[2]
RX= fixedNode.getReaction[0]
RY= fixedNode.getReaction[1]
RZ= fixedNode.getReaction[2] 


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
