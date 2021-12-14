# -*- coding: utf-8 -*-
''' Home made test. «rotate» method verification for a 3D elastic section.'''

from __future__ import print_function

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
import math

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
L= 1 # Bar length (m)

# Load
F= 1e3 # Load magnitude (kN)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

nodes.defaultTag= 1 # First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)

lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Materials definition
scc= typical_materials.defElasticSection3d(preprocessor, "scc",E,G,A,Iz,Iy,J)
scc.sectionProperties.rotate(math.radians(90))


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
elements.defaultTag= 1
beam= elements.newElement("ForceBeamColumn3d",xc.ID([1,2]))

# Constraints
modelSpace.fixNode000_000(1)


# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0') 
lp0.newNodalLoad(2,xc.Vector([0,0,-F,0,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)


nodes= preprocessor.getNodeHandler 
nod2= nodes.getNode(2)
delta= nod2.getDisp[2]  # z displacement of node 2

elements= preprocessor.getElementHandler

elem1= elements.getElement(1)
elem1.getResistingForce()
scc0= elem1.getSections()[0]

M= scc0.getStressResultantComponent("Mz")


deltateor= (-F*L**3/(3*E*Iy))
ratio1= abs(delta-deltateor)/deltateor
ratio2= abs((F*L)-M)/(F*L)

''' 
print("delta= ",delta)
print("deltaTeor= ",deltateor)
print("ratio1= ",ratio1)
print("M= ",M)
print("MTeor= ",(F*L))
print("ratio2= ",ratio2)
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-9) & (ratio2<1e-15)): 
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


