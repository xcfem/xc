# -*- coding: utf-8 -*-
''' home made test
    Reference:  Cálculo de estructuras por el método de los elemen-
    tos finitos. 1991. E. Oñate, page 165, example 5.3

    isbn={9788487867002}
    url={https://books.google.ch/books?id=lV1GSQAACAAJ}

'''

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

L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
Iy= 80.1e-8 # Cross section moment of inertia (m4)
Iz= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)
AT= 10 # Temperature increment (Celsius degrees)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Define nodes.
n1= nodes.newNodeXYZ(0.0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)

# Define materials.
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J)

# Elements definition
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
elements.dimElem= 2 # Dimension of element space
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n1.tag, n2.tag]))
    
# Constraints
modelSpace.fixNode000_000(n1.tag)
modelSpace.fixNode000_000(n2.tag)

    
# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
#\set_current_load_pattern{"0"}
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([beam3d.tag])
thermalDeformation= xc.DeformationPlane(alpha*AT)
eleLoad.backEndDeformationPlane= thermalDeformation
eleLoad.frontEndDeformationPlane= thermalDeformation

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

# Get internal forces.
axil1= beam3d.getN1
axil2= beam3d.getN2

N= (-E*A*alpha*AT)
ratio= ((axil2-N)/N)

# print("N= ",N)
# print("axil1= ",axil1)
# print("axil2= ",axil2)
# print("ratio= ",ratio)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
