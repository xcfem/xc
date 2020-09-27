# -*- coding: utf-8 -*-
from __future__ import print_function
# Home made test
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


# Model definition

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod1= nodes.newNodeXYZ(1,1,1)

    
# Define materials

kx= typical_materials.defElasticMaterial(preprocessor, "kx",KX)
ky= typical_materials.defElasticMaterial(preprocessor, "ky",KY)
kz= typical_materials.defElasticMaterial(preprocessor, "kz",KZ)


fixedNode, newElemen= modelSpace.setBearing(nod1.tag,["kx","ky","kz"])
  
# Constraints
constraints= preprocessor.getBoundaryCondHandler

#
spc= constraints.newSPConstraint(nod1.tag,3,0.0) # Node 1
spc= constraints.newSPConstraint(nod1.tag,4,0.0)
spc= constraints.newSPConstraint(nod1.tag,5,0.0)


# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nod1.tag,xc.Vector([FX,FY,FZ,0,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

deltax= nod1.getDisp[0]
deltay= nod1.getDisp[1]
deltaz= nod1.getDisp[2] 

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
    print("RX= ",RX)
    print("dx= ",deltax)
    print("RY= ",RY)
    print("dy= ",deltay)
    print("RZ= ",RZ)
    print("dz= ",deltaz)
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
