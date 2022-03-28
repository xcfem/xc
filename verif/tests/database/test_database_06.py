# -*- coding: utf-8 -*-
from __future__ import print_function
''' Taken from page 114 of the article Development of Membrane, Plate and
 Flat Shell Elements in Java '''
''' The error is about 15% (quite high) it seems that the element formulation
    doesn't works very well with "skinny" elements. '''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 6.0 # Beam length expressed in inches.
h= 0.8 # Beam cross-section depth expressed in inches.
t= 1 # Beam cross-section width expressed in inches.
E= 30000 # Young modulus of the material expressed in ksi.
nu= 0.3 # Poisson's ratio.
# Load
F= 10 # Load magnitude en kips

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(L/3,0,0)
n3= nodes.newNodeXYZ(2*L/3,0,0)
n4= nodes.newNodeXYZ(L,0,0)
n5= nodes.newNodeXYZ(0,h,0)
n6= nodes.newNodeXYZ(L/3,h,0)
n7= nodes.newNodeXYZ(2*L/3,h,0)
n8= nodes.newNodeXYZ(L,h,0)


# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,0.0,h)

elements= preprocessor.getElementHandler
elements.defaultMaterial= memb1.name
elemA= elements.newElement("ShellMITC4",xc.ID([n1.tag,n2.tag,n6.tag,n5.tag]))
elemB= elements.newElement("ShellMITC4",xc.ID([n2.tag,n3.tag,n7.tag,n6.tag]))
elemC= elements.newElement("ShellMITC4",xc.ID([n3.tag,n4.tag,n8.tag,n7.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler

modelSpace.fixNode000_FFF(n1.tag)
spc= modelSpace.constraints.newSPConstraint(n2.tag, 2,0.0)
spc= modelSpace.constraints.newSPConstraint(n3.tag, 2,0.0)
spc= modelSpace.constraints.newSPConstraint(n4.tag, 2,0.0)
modelSpace.fixNode000_FFF(n5.tag)
spc= modelSpace.constraints.newSPConstraint(n6.tag, 2,0.0)
spc= modelSpace.constraints.newSPConstraint(n7.tag, 2,0.0)
spc= modelSpace.constraints.newSPConstraint(n8.tag, 2,0.0)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n8.tag, xc.Vector([0,-F,0,0,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

mat= elemA.getPhysicalProperties.getVectorMaterials[0]
K11A= mat.getTangentStiffness().at(1,1)

# Store node and element tags
n3Tag= n3.tag
n8Tag= n8.tag

import os
os.system("rm -f /tmp/test06.db")
db= feProblem.newDatabase("SQLite","/tmp/test06.db")
db.save(2120)
feProblem.clearAll()
db.restore(2120)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
mat= ele1.getVectorMaterials[0]
K11D= mat.getTangentStiffness().at(1,1)


nodes= preprocessor.getNodeHandler
n8= nodes.getNode(n8Tag)
UX8= n8.getDisp[0] # Node 8 xAxis displacement
UY8= n8.getDisp[1] # Node 8 yAxis displacement

n3= nodes.getNode(n3Tag)
UX3= n3.getDisp[0] # Node 3 xAxis displacement
UY3= n3.getDisp[1] # Node 3 yAxis displacement

UX8SP2K= 0.016110
UY8SP2K= -0.162735
UX3SP2K= -0.014285
UY3SP2K= -0.084652


# Differences from the results attributed to SAP-2000 in that article.
ratio1= abs(((UX8-UX8SP2K)/UX8SP2K))
ratio2= abs(((UY8-UY8SP2K)/UY8SP2K))
ratio3= abs(((UX3-UX3SP2K)/UX3SP2K))
ratio4= abs(((UY3-UY3SP2K)/UY3SP2K))
ratio5= abs(((K11D-K11A)/K11A))

''' 
print("UX8= ",UX8)
print("UX8SP2K= ",UX8SP2K)
print("UY8= ",UY8)
print("UY8SP2K= ",UY8SP2K)
print("UX3= ",UX3)
print("UY3= ",UY3)

print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("ratio5= ",ratio5)
  '''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.15) & (abs(ratio2)<0.15) & (abs(ratio3)<0.15) & (abs(ratio4)<0.15) & (abs(ratio5)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
os.system("rm -f /tmp/test06.db") # Your garbage you clean it
