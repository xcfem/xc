# -*- coding: utf-8 -*-
''' Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

KX= 1000 # Spring constant
KY= 2000 # Spring constant
KT= 3000 # Spring constant
l= 1e-7 # Distance between nodes
FX= 1 # Force magnitude
FY= 2 
FT= 3 # Moment magnitude

import xc
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Model definition
n1= nodes.newNodeXY(1,1)
n2= nodes.newNodeXY(1,1)

# Materials definition
kx= typical_materials.defElasticMaterial(preprocessor, "kx",KX)
ky= typical_materials.defElasticMaterial(preprocessor, "ky",KY)
kt= typical_materials.defElasticMaterial(preprocessor, "kt",KT)


''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= kx.name
elements.dimElem= 3 # Dimension of element space
zl= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))
zl.clearMaterials()
zl.setMaterial(0,"kx")
zl.setMaterial(1,"ky")
zl.setMaterial(2,"kt")

   
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(n1.tag,0,0.0) # Node 1
spc= constraints.newSPConstraint(n1.tag,1,0.0)
spc= constraints.newSPConstraint(n1.tag,2,0.0)


# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([FX,FY,FT]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

deltax= n2.getDisp[0]
deltay= n2.getDisp[1]
deltat= n2.getDisp[2] 
RX= n1.getReaction[0]
RY= n1.getReaction[1]
RT= n1.getReaction[2] 


ratio1= -RX/FX
ratio2= (KX*deltax)/FX
ratio3= -RY/FY
ratio4= (KY*deltay)/FY
ratio5= -RT/FT
ratio6= (KT*deltat)/FT

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
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayLocalAxes()
