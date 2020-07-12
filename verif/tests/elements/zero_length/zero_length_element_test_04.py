# -*- coding: utf-8 -*-
from __future__ import print_function
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
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Model definition
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(1,1,1)
nod= nodes.newNodeXYZ(1,1,1)

# Materials definition
kx= typical_materials.defElasticMaterial(preprocessor, "kx",KX)
ky= typical_materials.defElasticMaterial(preprocessor, "ky",KY)
kz= typical_materials.defElasticMaterial(preprocessor, "kz",KZ)


''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= kx.name
elements.dimElem= 3 # Dimension of element space
zl= elements.newElement("ZeroLength",xc.ID([1,2]))
zl.clearMaterials()
zl.setMaterial(0,"kx")
zl.setMaterial(1,"ky")
zl.setMaterial(2,"kz")

    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(1,0,0.0) # Node 1
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(1,2,0.0)
spc= constraints.newSPConstraint(1,3,0.0)
spc= constraints.newSPConstraint(1,4,0.0)
spc= constraints.newSPConstraint(1,5,0.0)
spc= constraints.newSPConstraint(2,3,0.0) # Node 2
spc= constraints.newSPConstraint(2,4,0.0)
spc= constraints.newSPConstraint(2,5,0.0)


# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([FX,FY,FZ,0,0,0]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

nod2= nodes.getNode(2)
deltax= nod2.getDisp[0]
deltay= nod2.getDisp[1]
deltaz= nod2.getDisp[2] 
nod1= nodes.getNode(1)
RX= nod1.getReaction[0]
RY= nod1.getReaction[1]
RZ= nod1.getReaction[2] 


ratio1= -RX/FX
ratio2= (KX*deltax)/FX
ratio3= -RY/FY
ratio4= (KY*deltay)/FY
ratio5= -RZ/FZ
ratio6= (KZ*deltaz)/FZ

''' 
print("RX= ",RX)
print("dx= ",deltax)
print("RY= ",RY)
print("dy= ",deltay)
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
print("ratio4= ",(ratio4))
print("ratio5= ",(ratio5))
print("ratio6= ",(ratio6))
   '''
  
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5) & (abs(ratio4-1.0)<1e-5)  & (abs(ratio5-1.0)<1e-5) & (abs(ratio6-1.0)<1e-5) :
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
