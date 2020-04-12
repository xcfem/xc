# -*- coding: utf-8 -*-
# home made test

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 30e6 # Young modulus (psi)
l= 20*12 # Bar length in inches
h= 30 # Beam cross-section depth in inches.
A= 50.65 # viga area in square inches.
I= 7892 # Inertia of the beam section in inches to the fourth power.
F= 1000.0 # Force

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(l,0.0)
nod= nodes.newNodeXY(2*l,0.0)
nod= nodes.newNodeXY(3*l,0.0)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
    

# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= "lin"
elements.defaultMaterial= "scc"
#  sintaxis: beam2d_02[<tag>] 
elements.defaultTag= 1 #Tag for next element.
beam2d= elements.newElement("ElasticBeam2d",xc.ID([1,2]))
beam2d.h= h
        
beam2d= elements.newElement("ElasticBeam2d",xc.ID([3,4]))
beam2d.h= h
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(1,0,0.0) # Node 1
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(1,2,0.0)
spc= constraints.newSPConstraint(4,0,0.0) # Node 4
spc= constraints.newSPConstraint(4,1,0.0)
spc= constraints.newSPConstraint(4,2,0.0)


setTotal= preprocessor.getSets.getSet("total")
setTotal.killElements() # deactivate the elements

mesh= preprocessor.getDomain.getMesh
mesh.setDeadSRF(0.0)
mesh.freezeDeadNodes("congela") # Constraint inactive nodes.

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,F,F]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

import os
os.system("rm -r -f /tmp/test14.db")
db= feProblem.newDatabase("BerkeleyDB","/tmp/test14.db")
db.save(100)
feProblem.clearAll()
feProblem.setVerbosityLevel(0) #Dont print warning messages
                            #about pointers to material.
db.restore(100)
feProblem.setVerbosityLevel(1) #Print warnings again 


# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)


nodes.calculateNodalReactions(True,1e-7)
nod1= nodes.getNode(1)
deltax1= nod1.getDisp[0] 
deltay1= nod1.getDisp[1] 
nod2= nodes.getNode(2)
deltax2= nod2.getDisp[0] 
deltay2= nod2.getDisp[1] 
R1= nod1.getReaction[0] 
R2= nod2.getReaction[0] 


setTotal= preprocessor.getSets.getSet("total")
setTotal.aliveElements()
mesh= preprocessor.getDomain.getMesh
mesh.meltAliveNodes("congela") # Reactivate inactive nodes.


# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

db.save(105)
feProblem.clearAll()
feProblem.setVerbosityLevel(0) #Dont print warning messages
                            #about pointers to material.
db.restore(105)
feProblem.setVerbosityLevel(1) #Print warnings again 


nodes.calculateNodalReactions(True,1e-7)
nod1= nodes.getNode(1)
deltaxB1= nod1.getDisp[0] 
deltayB1= nod1.getDisp[1] 
nod2= nodes.getNode(2)
deltaxB2= nod2.getDisp[0] 
deltayB2= nod2.getDisp[1] 
RB1= nod1.getReaction[0] 
nod2= nodes.getNode(2)
RB2= nod2.getReaction[0] 




ratio1= (R1)
ratio2= ((R2+F)/F)
ratio3= ((RB1+F)/F)
ratio4= (RB2)

''' 
print "R1= ",R1
print "R2= ",R2
print "dx2= ",deltax2
print "dy2= ",deltay2
print "RB1= ",RB1
print "RB2= ",RB2
print "dxB2= ",deltaxB2
print "dyB2= ",deltayB2
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
os.system("rm -rf /tmp/test14.db") # Your garbage you clean it

