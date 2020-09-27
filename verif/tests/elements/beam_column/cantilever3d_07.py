# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

'''Cantilever under horizontal uniform load in local x direction.'''

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
f= 1.5e3 # Load magnitude en N/m

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 # First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Materials
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
section= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "section",sectionProperties)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
elements.defaultTag= 1 # Tag for next element.
beam3d= elements.newElement("ElasticBeam3d",xc.ID([1,2]))
# end of element definition

# Constraints
modelSpace.fixNode000_000(1)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID([1]) 
eleLoad.axialComponent= f
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

nodes= preprocessor.getNodeHandler 
nod2= nodes.getNode(2)
delta= nod2.getDisp[0]  # Node 2 xAxis displacement

elements= preprocessor.getElementHandler
elem1= elements.getElement(1)
elem1.getResistingForce()
N1= elem1.getN1 # axial force at the back end.
N2= elem1.getN2 # axial force at the front end.



deltateor= (f*L**2/(2*E*A))
ratio1= (delta/deltateor)
N1teor= (f*L)
ratio2= (N1/N1teor)
ratio3= (abs(N2)<1e-3)

'''
print("delta= ",delta)
print("deltateor= ",deltateor)
print("ratio1= ",ratio1)
print("N1= ",N1)
print("N1teor= ",N1teor)
print("ratio2= ",ratio2)
print("N2= ",N2)
print("N2teor= ",0)
print("ratio3= ",ratio3})
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
