# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

E= 2.1e9 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
h= .1 # Thickness.
L= 1.0 # Lado.
I= 1/12.0*L*h**3 # Cross section moment of inertia (m4)
dens= 1.33 # Density kg/m2.
F= 1000 # Force


# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeLoader

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0,0)
nod= nodes.newNodeXYZ(L,0,0)
nod= nodes.newNodeXYZ(L,L,0)
nod= nodes.newNodeXYZ(0,L,0)

nod= nodes.newNodeXYZ(0,0,L)
nod= nodes.newNodeXYZ(L,0,L)
nod= nodes.newNodeXYZ(L,L,L)
nod= nodes.newNodeXYZ(0,L,L)

# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)

# Elements definition
elements= preprocessor.getElementLoader
elements.defaultMaterial= "memb1"
elem= elements.newElement("ShellMITC4",xc.ID([1,2,3,4]))
elem= elements.newElement("ShellMITC4",xc.ID([5,6,7,8]))
    
# Constraints
modelSpace.fixNode000_000(1)
modelSpace.fixNode000_000(4)
modelSpace.fixNode000_000(5)
modelSpace.fixNode000_000(8)


setTotal= preprocessor.getSets.getSet("total")
setTotal.killElements() # deactivate the elements

mesh= feProblem.getDomain.getMesh
mesh.setDeadSRF(0.0)
mesh.freezeDeadNodes("bloquea") # Constraint inactive nodes.


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0,F,0,0,0]))
lp0.newNodalLoad(3,xc.Vector([F,0,F,0,0,0]))
lp0.newNodalLoad(6,xc.Vector([F,0,F,0,0,0]))
lp0.newNodalLoad(7,xc.Vector([F,0,F,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)



nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader
nod1= nodes.getNode(1)
deltax1= nod1.getDisp[0] 
deltay1= nod1.getDisp[1] 
deltaz1= nod1.getDisp[2] 

nod2= nodes.getNode(2)
deltax2= nod2.getDisp[0] 
deltay2= nod2.getDisp[1] 
deltaz2= nod2.getDisp[2] 

nod3= nodes.getNode(3)
deltax3= nod3.getDisp[0] 
deltay3= nod3.getDisp[1] 
deltaz3= nod3.getDisp[2] 

R1= nod1.getReaction[0] 
R2= nod2.getReaction[0] 



# print "Revive.\n"
setTotal.aliveElements()
mesh.meltAliveNodes("bloquea") # Reactivate inactive nodes.

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)


nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader
nod1= nodes.getNode(1)
deltaxB1= nod1.getDisp[0] 
deltayB1= nod1.getDisp[1] 
deltazB1= nod1.getDisp[2] 

nod2= nodes.getNode(2)
deltaxB2= nod2.getDisp[0] 
deltayB2= nod2.getDisp[1]
deltazB2= nod2.getDisp[2] 

nod3= nodes.getNode(3)
deltaxB3= nod3.getDisp[0] 
deltayB3= nod3.getDisp[1]
deltazB3= nod3.getDisp[2] 

RB1= nod1.getReaction[0] 
RB2= nod2.getReaction[0] 



dXTeor= 2*F/(E*h*L)
dZTeor= 2*F*L**3/(3.0*E*I)

ratio1= (R1/F)
ratio2= ((R2+F)/F)
ratio3= ((RB1+F)/F)
ratio4= (RB2)
ratio5= ((dXTeor-deltaxB3)/dXTeor)
ratio6= ((dZTeor-deltazB3)/dZTeor)


''' 
print "R1= ",R1
print "R2= ",R2
print "dx1= ",deltax1
print "dy1= ",deltay1
print "dz1= ",deltaz1
print "dx2= ",deltax2
print "dy2= ",deltay2
print "dz2= ",deltaz2
print "RB1= ",RB1
print "RB2= ",RB2
print "dXTeor= ",(dXTeor)
print "dZTeor= ",(dZTeor)
print "dxB1= ",deltaxB1
print "dyB1= ",deltayB1
print "dzB1= ",deltazB1
print "dxB2= ",deltaxB2
print "dyB2= ",deltayB2
print "dzB2= ",deltazB2
print "dxB3= ",deltaxB3
print "dyB3= ",deltayB3
print "dzB3= ",deltazB3
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
print "ratio5= ",ratio5
print "ratio6= ",ratio6
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5) & (abs(ratio5)<0.1):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

