# -*- coding: utf-8 -*-
# home made test

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

E= 2.1e9 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
h= .1 # Thickness.
L= 1.0 # Side length.
I= 1/12.0*L*h**3 # Cross section moment of inertia (m4)
dens= 1.33 # Density kg/m2.
F= 1000 # Force


# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(L,0,0)
n3= nodes.newNodeXYZ(L,L,0)
n4= nodes.newNodeXYZ(0,L,0)

n5= nodes.newNodeXYZ(0,0,L)
n6= nodes.newNodeXYZ(L,0,L)
n7= nodes.newNodeXYZ(L,L,L)
n8= nodes.newNodeXYZ(0,L,L)

# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "memb1"
elemA= elements.newElement("ShellMITC4",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))
elemB= elements.newElement("ShellMITC4",xc.ID([n5.tag,n6.tag,n7.tag,n8.tag]))
    
# Constraints
modelSpace.fixNode000_000(n1.tag)
modelSpace.fixNode000_000(n4.tag)
modelSpace.fixNode000_000(n5.tag)
modelSpace.fixNode000_000(n8.tag)


setTotal= preprocessor.getSets.getSet("total")
setTotal.killElements() # deactivate the elements

mesh= feProblem.getDomain.getMesh
mesh.setDeadSRF(0.0) # set stress reduction factor for element deactivation.
mesh.freezeDeadNodes('locked') # Constraint inactive nodes.


# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(n2.tag,xc.Vector([F,0,F,0,0,0]))
lp0.newNodalLoad(n3.tag,xc.Vector([F,0,F,0,0,0]))
lp0.newNodalLoad(n6.tag,xc.Vector([F,0,F,0,0,0]))
lp0.newNodalLoad(n7.tag,xc.Vector([F,0,F,0,0,0]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

deltax1= n1.getDisp[0] 
deltay1= n1.getDisp[1] 
deltaz1= n1.getDisp[2] 

deltax2= n2.getDisp[0] 
deltay2= n2.getDisp[1] 
deltaz2= n2.getDisp[2] 

deltax3= n3.getDisp[0] 
deltay3= n3.getDisp[1] 
deltaz3= n3.getDisp[2] 

R1= n1.getReaction[0] 
R2= n2.getReaction[0] 



# print("Revive.\n")
setTotal.aliveElements()
mesh.meltAliveNodes('locked') # Reactivate inactive nodes.

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

deltaxB1= n1.getDisp[0] 
deltayB1= n1.getDisp[1] 
deltazB1= n1.getDisp[2] 

deltaxB2= n2.getDisp[0] 
deltayB2= n2.getDisp[1]
deltazB2= n2.getDisp[2] 

deltaxB3= n3.getDisp[0] 
deltayB3= n3.getDisp[1]
deltazB3= n3.getDisp[2] 

RB1= n1.getReaction[0] 
RB2= n2.getReaction[0] 



dXTeor= 2*F/(E*h*L)
dZTeor= 2*F*L**3/(3.0*E*I)

ratio1= (R1/F)
ratio2= ((R2+F)/F)
ratio3= ((RB1+F)/F)
ratio4= (RB2)
ratio5= ((dXTeor-deltaxB3)/dXTeor)
ratio6= ((dZTeor-deltazB3)/dZTeor)


''' 
print("R1= ",R1)
print("R2= ",R2)
print("dx1= ",deltax1)
print("dy1= ",deltay1)
print("dz1= ",deltaz1)
print("dx2= ",deltax2)
print("dy2= ",deltay2)
print("dz2= ",deltaz2)
print("RB1= ",RB1)
print("RB2= ",RB2)
print("dXTeor= ",(dXTeor))
print("dZTeor= ",(dZTeor))
print("dxB1= ",deltaxB1)
print("dyB1= ",deltayB1)
print("dzB1= ",deltazB1)
print("dxB2= ",deltaxB2)
print("dyB2= ",deltayB2)
print("dzB2= ",deltazB2)
print("dxB3= ",deltaxB3)
print("dyB3= ",deltayB3)
print("dzB3= ",deltazB3)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("ratio5= ",ratio5)
print("ratio6= ",ratio6)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5) & (abs(ratio5)<0.1)):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

