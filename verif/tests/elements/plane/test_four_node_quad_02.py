# -*- coding: utf-8 -*-
from __future__ import print_function
# Taken from page 114 of the article Development of Membrane, Plate and
# Flat Shell Elements in Java

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 6.0 # Beam length expressed in inches.
h= 0.8 # Beam cross-section depth expressed in inches.
t= 1 # Beam cross-section width expressed in inches. In the article t= 0.2 was written by mistake.
E= 30000 # Young modulus of the material expressed in ksi.
nu= 0.3 # Poisson's ratio.
# Load
F= 10 # Load magnitude en kips

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)

n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(L/3,0)
n3= nodes.newNodeXY(2*L/3,0)
n4= nodes.newNodeXY(L,0)
n5= nodes.newNodeXY(0,h)
n6= nodes.newNodeXY(L/3,h)
n7= nodes.newNodeXY(2*L/3,h)
n8= nodes.newNodeXY(L,h)


# Materials definition
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,0.0)

elements= preprocessor.getElementHandler
elements.defaultMaterial= elast2d.name
quad1= elements.newElement("FourNodeQuad",xc.ID([n1.tag,n2.tag,n6.tag,n5.tag]))
quad1.thickness= t
quad2= elements.newElement("FourNodeQuad",xc.ID([n2.tag,n3.tag,n7.tag,n6.tag]))
quad2.thickness= t
quad3= elements.newElement("FourNodeQuad",xc.ID([n3.tag,n4.tag,n8.tag,n7.tag]))
quad3.thickness= t

# Constraints
constraints= preprocessor.getBoundaryCondHandler

spc= constraints.newSPConstraint(n1.tag,0,0.0)
spc= constraints.newSPConstraint(n1.tag,1,0.0)
spc= constraints.newSPConstraint(n5.tag,0,0.0)
spc= constraints.newSPConstraint(n5.tag,1,0.0)

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n8.tag,xc.Vector([0,-F]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution
analOk= modelSpace.analyze(calculateNodalReactions= True)
 
# print("reac node 1: ",reac)
Fx= n1.getReaction[0]
Fy= n1.getReaction[1]
# \print{"Fx= ",Fx
# print("Fy= ",Fy})

disp= n3.getDisp
UX3= disp[0] # Node 3 xAxis displacement
UY3= disp[1] # Node 3 yAxis displacement


# print("reac node 5: ",reac)
Fx= (Fx+n5.getReaction[0])
Fy= (Fy+n5.getReaction[1])
# \print{"Fx= ",Fx
# print("Fy= ",Fy})

UX8= n8.getDisp[0] # Node 8 xAxis displacement
UY8= n8.getDisp[1] # Node 8 yAxis displacement


UX8SP2K= 0.016110
UY8SP2K= -0.162735
UX3SP2K= -0.014285
UY3SP2K= -0.084652


# Differences from the results attributed to SAP-2000 in that article.
ratio1= abs(((UX8-UX8SP2K)/UX8SP2K))
ratio2= abs(((UY8-UY8SP2K)/UY8SP2K))
ratio3= abs(((UX3-UX3SP2K)/UX3SP2K))
ratio4= abs(((UY3-UY3SP2K)/UY3SP2K))
ratio5= abs(((Fy-F)/F))

''' 
print("Fx= ",Fx)
print("Fy= ",Fy)
print("UX8= ",UX8)
print("UX8SP2K= ",UX8SP2K)
print("UY8= ",UY8)
print("UY8SP2K= ",UY8SP2K)
print("UX3= ",UX3)
print("UX3SP2K= ",UX3SP2K)
print("UY3= ",UY3)
print("UY3SP2K= ",UX3SP2K)

print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("ratio5= ",ratio5)
print("analOk= ",analOk)
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5) & (abs(ratio5)<1e-12) & (analOk == 0.0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
