# -*- coding: utf-8 -*-
''' Detection of unconstrained DOF using the locateEquationNumber method in 
    PredefinedSpaces class. Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
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

# Geometry
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude (kN)

# Create problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
##  Set problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
## Define mesh.

### Define nodes.
n1= nodes.newNodeXY(0,0.0)
n2= nodes.newNodeXY(L,0.0)

### Define elements.
lin= modelSpace.newLinearCrdTransf("lin") # Coordinate transformation.

#### Define material.
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz; 
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section", sectionProperties)

#### Define element.
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name # Set coordinate transformation.
elements.defaultMaterial= section.name # Set material.
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))

### Define constraints.
modelSpace.fixNode00F(n1.tag) # Mechanism.

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag, xc.Vector([F,0,0])) # Positive force along x axis
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution 0 N
analysis= predefined_solutions.simple_static_linear(feProblem)
feProblem.errFileName= "/tmp/erase.err" # Don't print errors.
result= analysis.analyze(1)
feProblem.errFileName= "cerr" # Print errors if any

# Some solvers store the information returned by the underlying
# FORTRAN or C libraries in a property called "info"
info= None
solver= analysis.linearSOE.solver
if(solver.hasProp("info")):
    info= solver.getProp("info")

# Search for the node that corresponds to the equation number.
unconstrainedNode= modelSpace.locateEquationNumber(eqNumber= -result)

# Check result
ok= (unconstrainedNode.tag==n2.tag)

modelSpace.fixNodeF0F(unconstrainedNode.tag) # Add constraint.
result= analysis.analyze(1) # Solve again.

# Check result after fixing the constraints.
dx= n2.getDisp[0]
dxRef= F*L/E/A
err= abs(dx-dxRef)/dxRef



'''
print('result: ', result)
print('info: ', info)
print('ok: ', ok)
print('unconstrained node tag: ', unconstrainedNode.tag)
print('n2.tag: ', n2.tag)
print('dx= ', dx*1e3, 'mm')
print('dxRef= ', dxRef*1e3, 'mm')
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ok and (abs(err)<1e-5)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
