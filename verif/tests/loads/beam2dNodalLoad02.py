# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test
# Reference:  deflection formulas from the publication "Prontuario de
# Estructuras Metálicas del CEDEX". Section 3.3 Punctual load on cantilever.
# ISBN: 84-7790-336-0
# url={https://books.google.ch/books?id=j88yAAAACAAJ},

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials.sections import section_properties
from materials.sections import def_secc_aggregation

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 2e9 # Elastic modulus
L= 20 # Bar length.
h= 0.30 # Beam cross-section depth.
b= 0.2 # Beam cross-section width.
A= b*h # Cross section area.
I= b*h**3/12 # Beam inertia inches^4.
x= 0.5 # Relative abscissae where the punctual load is applied.
P= 1e3 # punctual load.

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler

sectionTest= section_properties.RectangularSection("sectionTest",b,h) # Section geometry.
sectionTestMaterial=typical_materials.MaterialData(name='sectionTestMaterial',E=E,nu=0.3,rho=2500) # Section material.

modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
# Definimos el material
def_secc_aggregation.def_secc_aggregation2d(preprocessor, sectionTest,sectionTestMaterial)
nodes.defaultTag= 1 # First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(L,0.0)

# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin")
    
# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name# Coordinate transformation for the new elements
elements.defaultMaterial= sectionTest.sectionName
elements.defaultTag= 1 # Tag for next element.
beam2d= elements.newElement("ForceBeamColumn2d",xc.ID([1,2]))
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000(1)


# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(2,xc.Vector([0,-P,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)


nod2= nodes.getNode(2)
delta= nod2.getDisp[1] 


deltaTeor= (-P*L**3/3/E/I)
ratio1= ((delta-deltaTeor)/deltaTeor)

''' 
print("delta= ",delta)
print("deltaTeor= ",deltaTeor)
print("ratio1= ",ratio1)
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<5e-4:
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
