# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test
'''Save and restore methods verification.'''

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

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)

lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
    
# Materials definition
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J)


elementHandler= preprocessor.getElementHandler
elementHandler.defaultTransformation= lin.name
elementHandler.defaultMaterial= scc.name
#  sintaxis: ElasticBeam3d[<tag>] 
elementHandler.defaultTag= 1 # Tag for next element.
beam3d= elementHandler.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))

modelSpace.fixNode000_000(n1.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([F,0,0,0,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

lp1= modelSpace.newLoadPattern(name= '1')
lp1.newNodalLoad(n2.tag,xc.Vector([F,0,0,0,0,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp1.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

import os
os.system("rm -f /tmp/test03.db")
db= feProblem.newDatabase("BerkeleyDB","/tmp/test03.db")
db.save(100)
tagN1= n1.tag
tagN2= n2.tag
feProblem.clearAll()
db.restore(100)

nodes= preprocessor.getNodeHandler

nod1= nodes.getNode(tagN1)
nod2= nodes.getNode(tagN2)
preprocessor.getNodeHandler.calculateNodalReactions(False,1e-6)
R= nod1.getReaction[0]
delta= nod2.getDisp[0]  # x displacement of node 2


elem1= elementHandler.getElement(1)
elem1.getResistingForce()
N1= elem1.getN1

deltateor= (2*F*L/(E*A))
ratio1= (delta/deltateor)
ratio2= (N1/(2*F))
ratio3= abs(R+2.0*F)

''' 
print("delta= ",delta)
print("deltateor= ",deltateor)
print("ratio1= ",ratio1)
print("N1= ",N1)
print("ratio2= ",ratio2)
print('R= ', R)
print('ratio3= ', ratio3)
   '''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (ratio3<1e-5):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')

os.system("rm -rf /tmp/test03.db") # Your garbage you clean it
