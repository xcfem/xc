# -*- coding: utf-8 -*-
# home made test
# Reference:  Expresiones de la flecha el el Prontuario de
# Estructuras Metálicas del CEDEX. Apartado 3.3 Punctual load on cantilever.
# ISBN: 84-7790-336-0
# url={https://books.google.ch/books?id=j88yAAAACAAJ},

from __future__ import print_function
from __future__ import division

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)


    
# Materials definition
E= 2e6 # Elastic modulus
L= 20 # Bar length.
h= 0.30 # Beam cross-section depth.
b= 0.2 # Beam cross-section width.
A= b*h # Cross section area.
I= b*h**3/12 # Inertia of the beam section in inches to the fourth power.
x= 0.5 # Relative abscissae where the punctual load is applied.
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

# Mesh

## Node definition
nod1= nodes.newNodeXY(0,0)
nod2= nodes.newNodeXY(L,0.0)

# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin")

## Element definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
#  sintaxis: beam2d_02[<tag>] 
beam2d= elements.newElement("ElasticBeam2d",xc.ID([nod1.tag,nod2.tag]))
beam2d.sectionProperties.h= h

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000(nod1.tag)


# Load definition
P= 1e3 # punctual load.

## Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
nod2.newLoad(xc.Vector([0,-P,0]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

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
if abs(ratio1)<1e-12:
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
