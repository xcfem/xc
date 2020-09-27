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
l= 20*12 # Bar length in inches

E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
A= 50.65 # Beam cross-section area in square inches.
Iz= 7892 # Inertia of the beam section in inches to the fourth power.
Iy= 7892 # Inertia of the beam section in inches to the fourth power.
J= Iz/100.0 # Cross section torsion constant
F= 1000 # Force

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(l,0,0)
n3= nodes.newNodeXYZ(2*l,0,0)
n4= nodes.newNodeXYZ(3*l,0,0)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
    
# Materials definition
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
scc= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "section",sectionProperties)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
#  syntax: beam2d_02[<tag>] 
beamA= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))
        
beamB= elements.newElement("ElasticBeam3d",xc.ID([n3.tag,n4.tag]))
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
modelSpace.fixNode000_000(n1.tag) # Node 1
modelSpace.fixNode000_000(n4.tag) # Node 4


setTotal= preprocessor.getSets.getSet("total")
modelSpace.deactivateElements(setTotal)


# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([F,F,0,0,F,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

deltax1= n1.getDisp[0] 
deltay1= n1.getDisp[1] 

deltax2= n2.getDisp[0] 
deltay2= n2.getDisp[1] 

R1= n1.getReaction[0] 
R2= n2.getReaction[0]

Na= beamA.getN()
Nb= beamB.getN()


# Return elements to life.
modelSpace.activateElements(setTotal)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

deltaxB1= n1.getDisp[0] 
deltayB1= n1.getDisp[1] 

deltaxB2= n2.getDisp[0] 
deltayB2= n2.getDisp[1] 

RB1= n1.getReaction[0] 
RB2= n2.getReaction[0] 

NaB= beamA.getN()
NbB= beamB.getN()

ratio1= (R1)
ratio2= ((R2+F)/F)
ratio3= ((RB1+F)/F)
ratio4= (RB2)

''' 
print("R1= ",R1)
print("R2= ",R2)
print("Na= ",Na)
print("Nb= ",Nb)
print("dx2= ",deltax2)
print("dy2= ",deltay2)
print("RB1= ",RB1)
print("RB2= ",RB2)
print("NaB= ",NaB)
print("NbB= ",NbB)
print("dxB2= ",deltaxB2)
print("dyB2= ",deltayB2)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

