# -*- coding: utf-8 -*-
''' Verification of removeSPConstraint method.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Material properties
E= 2.1e6*9.81/1e-4 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

# Cross section properties (IPE-80)
A= 7.64e-4 # Cross section area (m2)
Iy= 80.1e-8 # Cross section moment of inertia (m4)
Iz= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)

# Geometry
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude (kN)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Materials
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G;
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
seccion= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "seccion",sectionProperties)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= "lin"
elements.defaultMaterial= "seccion"
elements.defaultTag= 1 #Tag for the next element.
beam3d= elements.newElement("ElasticBeam3d",xc.ID([1,2]));

# Constraints
modelSpace.fixNode000_000(1)
spc= modelSpace.constraints.newSPConstraint(2,1,0.0)
spcTag= spc.tag
# \fix[2,1,20]{ \valor{0.0} }

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,-F,0,0,0,0]))
#We add the load case to domain.
lPatterns.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)


nodes= preprocessor.getNodeHandler 
nod2= nodes.getNode(2)
delta0= nod2.getDisp[1]  # Node 2 yAxis displacement




modelSpace.constraints.removeSPConstraint(spcTag)
#remove_fix{20}



# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)



nodes= preprocessor.getNodeHandler 
nod2= nodes.getNode(2)
delta= nod2.getDisp[1]  # Node 2 yAxis displacement

elem1= elements.getElement(1)
elem1.getResistingForce()
M= elem1.getMy1
MTeor= F*L
V= elem1.getVz

deltateor= (-F*L**3/(3*E*Iy))
ratio1= ((delta-deltateor)/deltateor)
ratio2= abs((M-MTeor)/MTeor)
ratio3= (V/F)

''' 
print "delta0= ",delta0
print "delta= ",delta
print "deltaTeor= ",deltateor
print "ratio1= ",ratio1
print "M= ",M
print "ratio2= ",ratio2
print "V1= ",V
print "ratio3= ",ratio3
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(delta0)<1e-11) & (abs(ratio1)<1e-9) & (ratio2<1e-5) & (abs(ratio3-1.0)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
